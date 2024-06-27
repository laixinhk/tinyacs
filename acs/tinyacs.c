/* Tiny ACS test tool for developers.
 * Copyright (C) 2022-2024 Laixin LIU <liulaixin@hotmail.com>
 *
 * This file is part of Tiny ACS.
 *
 * Tiny ACS (Auto Configuration Server) is an open source
 * TR-069(CPE WAN Management Protocol) test tool for developers.
 * Tiny ACS is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 * 
 * Tiny ACS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tiny ACS; see the file LICENSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h> 
#include <sys/stat.h> 
#include <stdbool.h>
#include <time.h>
#include <strings.h>

#include "plugin/httpda.h"
#include "cwmpH.h"

#include "cwmp.nsmap" //included "stdsoap2.h"


ACSHandler* pacsh = NULL;

int authentication(struct soap *soap)
{
    call_in();
    if (soap->userid && soap->passwd)	/* Basic authentication: we may want to reject this since the password was sent in the clear */
    { if (!strcmp(soap->userid, acsh->userid)
            && !strcmp(soap->passwd, acsh->passwd))
    {
        info("Basic authentication succeeded!\n");
        return SOAP_OK;
    }
    else
        notice("Basic authentication failure!\n");
    }
    else if (soap->authrealm && soap->userid)
    {
        /* simulate database lookup on userid to find passwd */
        debug("Digest authentication:\nauthrealm:%s\nuserid:%s\n", soap->authrealm, soap->userid);
        if (!strcmp(soap->authrealm, acsh->authrealm) && !strcmp(soap->userid, acsh->userid))
        {
            debug("Digest authentication:realm and userid are correct!\n");
            if (!http_da_verify_post(soap, acsh->passwd))
            {
                info("Digest authentication succeeded!\n");
                return SOAP_OK;
            }
            else
            {
                notice("Digest authentication failure!\n");
            }
        }
    }
    notice("Authentication needed!\n");
    soap->authrealm = acsh->authrealm;
    call_out();
    return 401; /* Not authorized, challenge digest authentication with httpda plugin */
}

int filefilter(const struct dirent * dir)
{
    return (DT_REG == dir->d_type);
}

void scan_rpc_dir(struct soap *soap)
{
    call_in();
    struct dirent **namelist;

    acsh->lastfile_sent = false;
    acsh->filecount = 0;
    acsh->filetosend = 0;

    acsh->filecount = scandir(acsh->rpc_dir, &namelist, filefilter, alphasort);
    if(acsh->filecount < 0)
    {
        perror("scandir");
        call_out();
        return;
    }
    acsh->namelist = namelist;
    notice("Scandir:%s\n",acsh->rpc_dir);
    call_out();
    return;
}

int send_request(struct soap *soap)
{
    call_in();
    FILE *fp = NULL;

    char full_path[1024] = {0};
    char str[1024];

    soap->encodingStyle = ""; /* use SOAP encoding style */
    soap_serializeheader(soap);
    soap_response(soap, SOAP_OK);//http headers

    memset(full_path, 0, sizeof(full_path));
    memset(str, 0, sizeof(str));

    if(acsh->filecount == 0)
    {
        notice("ERROR No files need to be sent\n");
        soap_end_send(soap);
        soap->error = SOAP_STOP;
        return SOAP_STOP;
    }
    if(acsh->lastfile_sent)
    {
        notice("ERROR Last file has been sent\n");
        soap_end_send(soap);
        soap->error = SOAP_STOP;
        return SOAP_STOP;
    }
    strcat(full_path, acsh->rpc_dir);
    strcat(full_path, "/");
    strcat(full_path, acsh->namelist[acsh->filetosend]->d_name);
    notice("File to be send: %s\n", full_path);
    fp = fopen(full_path , "r");
    if(fp == NULL) {
        notice("Error opening file:%s\n", full_path);
        soap_end_send(soap);
        soap->error = SOAP_STOP;
        return SOAP_STOP;
    }
    memset(str, 0, sizeof(str));
    while( fgets (str, 1024, fp) != NULL ) {
        soap_send(soap, str);
    }
    fclose(fp);
    notice("File sent\n");
    free(acsh->namelist[acsh->filetosend]);
    acsh->filetosend++;
    if(acsh->filetosend == acsh->filecount)
    {
        acsh->lastfile_sent = true;
    }
    soap_end_send(soap);
    call_out();
    return SOAP_STOP;
}

SOAP_FMAC1
int
    SOAP_FMAC2
soap_begin_serve_acs(struct soap *soap)//modify from soap_begin_serve()
{
    call_in();
    int ret = 0;
    soap_begin(soap);
    if (soap_begin_recv(soap))
    {
        if(soap->error == SOAP_NO_DATA)//handle SOAP_NO_DATA error
        {
            debug("SOAP_NO_DATA\n");
            if(acsh->authenticate && SOAP_OK != authentication(soap))
            {
                info("authentication failure\n");
                soap->keep_alive = 0;    //set Connection: close
                soap_response(soap, 401);
                soap_end_send(soap);
                soap_closesock(soap);
            }
            else if( acsh->request_times > 0 && acsh->filecount > 0)
            {
                acsh->request_times--;
                info("loop time = %d\n", acsh->request_times);
                send_request(soap);
            }
            else
            {
                if(acsh->request_times <= 0)
                    info("loop stopped.\n");
                soap->keep_alive = 0;    //set Connection: close
                soap_response(soap, SOAP_OK);
                soap_end_send(soap);
                soap_closesock(soap);
            }
            soap->error = SOAP_STOP;
            ret = SOAP_STOP;
            call_out();
            return ret;
        }
        else  if (soap->error < SOAP_STOP)
        {
            ret = soap_send_fault(soap);
            call_out();
            return ret;
        }
        ret = soap_closesock(soap);
        call_out();
        return ret;
    }
    if (soap_envelope_begin_in(soap)
            || soap_recv_header(soap)
            || soap_body_begin_in(soap))
    {

        if (soap->error < SOAP_STOP)
        {

            ret = soap_send_fault(soap);
            call_out();
            return ret;
        }
        ret = soap_closesock(soap);
        call_out();
        return ret;
    }
    call_out();
    return SOAP_OK;
}


SOAP_FMAC5 int SOAP_FMAC6 soap_serve_acs(struct soap *soap)
{
    call_in();
    int ret = 0;

    do
    {
        debug("soap_serve_acs in do while begin \n");
        char localtime_in_httpheader[1024] = {0};
        struct timeval _tv;
        struct tm _tm;
        gettimeofday(&_tv, NULL);
        localtime_r(&_tv.tv_sec, &_tm);
        snprintf(localtime_in_httpheader, 1024, "Current-Time: %04d-%02d-%02d %02d:%02d:%02d.%06ld", (int)_tm.tm_year+1900, (int)_tm.tm_mon+1, (int)_tm.tm_mday, (int)_tm.tm_hour, (int)_tm.tm_min, (int)_tm.tm_sec, (long)_tv.tv_usec);
        soap->http_extra_header = localtime_in_httpheader;
        if (soap_begin_serve_acs(soap))
        {
            if (soap->error >= SOAP_STOP)
                continue;
            ret = soap->error;
            call_out();
            return ret;
        }
        if ((soap_serve_request(soap) || (soap->fserveloop && soap->fserveloop(soap))) && soap->error && soap->error < SOAP_STOP)
        {

            if(soap->error == SOAP_NO_METHOD)
            {
                ret = soap_recv_fault(soap, 1);//handle <SOAP-ENV:Fault>
                soap->keep_alive = 0;    //set Connection: close
                soap_response(soap, SOAP_OK);
                soap_end_send(soap);
                soap_closesock(soap);

                soap->error = SOAP_STOP;
                ret = SOAP_STOP;
                call_out();
                return ret;
            }

            ret = soap_send_fault(soap);
            call_out();
            return ret;

        }

        debug("soap_serve_acs in do while end \n");
    } while (soap->keep_alive);
    debug("soap_serve_acs jump out of do while\n");

    call_out();
    return SOAP_OK;
}

/**
 * Function to check whether a directory exists or not.
 * It returns 1 if given path is directory and  exists 
 * otherwise returns 0.
 */
int is_dir_exists(const char *path)
{
    struct stat stats;

    stat(path, &stats);

    // Check for file existence
    if (S_ISDIR(stats.st_mode))
        return 1;

    return 0;
}

void hint(char* appname)
{
    fprintf(stderr, "Use \"%s -h\" to get more information.\n", appname);
}

void usage(void)
{
    fprintf(stderr, "tinyacs %s - a tiny acs test tool\n", TINYACS_VERSION);
    fprintf(stderr, "Release-Date: %s\n", BUILD_DATE);
    fprintf(stderr, "%s\n", "tinyacs is copyrighted by Laixin LIU in 2024.");
    fprintf(stderr, "%s\n", "Licensed under GPLv2. See source distribution for detailed copyright notices.");
    fprintf(stderr, "%s\n", "");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    -h, --help                  Print this help and exit.\n");
    fprintf(stderr, "    -v, --version               Print the version and exit.\n");
    fprintf(stderr, "    -p, --port=PORT             The PORT that tinyacs will listen on.\n");
    fprintf(stderr, "                                Default PORT is 10240.\n");
    fprintf(stderr, "    -c, --auth                  Enable HTTP authentication.\n");
    fprintf(stderr, "    -u, --userid=USERNAME       USERNAME that the CPE will use to send to tinyacs.\n");
    fprintf(stderr, "    -w, --passwd=PASSWORD       The PASSWORD that the CPE will use to send to tinyacs.\n");
    fprintf(stderr, "    -s, --https                 Accept HTTPS connection instead of HTTP.\n");
    fprintf(stderr, "    -k, --keyfile=KEYFILE       KEYFILE (private key and certificate) for HTTPS connections use.\n");
    fprintf(stderr, "                                Default KEYFILE is \"ssl_server/KeyAndCA.pem\".\n");
    fprintf(stderr, "        --keypass=KEYPASS       KEYPASS to read KEYFILE.\n");
    fprintf(stderr, "                                Default KEYPASS is NULL.\n");
    fprintf(stderr, "        --cacert=CACERT         CACERT file to authenticate clients, for HTTPS connections use.\n");
    fprintf(stderr, "                                Default CACERT is NULL.\n");
    fprintf(stderr, "        --capath=CAPATH         CAPATH directory to authenticate clients, for HTTPS connections use.\n");
    fprintf(stderr, "                                Default CAPATH is NULL.\n");
    fprintf(stderr, "    -d, --debuglevel=LEVEL      For debugging. Default LEVEL is \"notice\", possible options:\n");
    fprintf(stderr, "                                \"error\", \"notice\", \"info\", \"debug\".\n");
    fprintf(stderr, "    -r, --rpcdir=DIR            A DIR containing a set of RPC request files that \n");
    fprintf(stderr, "                                will be sent to the CPE once tinyacs receives INFORM.\n");
    fprintf(stderr, "                                Default DIR is rpc.\n");
    fprintf(stderr, "    -l, --loop=NUMBER           Send the request files under the DIR directory cyclically, \n");
    fprintf(stderr, "                                Send only once by default(loop=0). -1 for infinite loop.\n");
    fprintf(stderr, "    -t, --servetimeout=TIMEOUT  Service TIMEOUT in seconds.\n");
    fprintf(stderr, "                                Default TIMEOUT is 0(Never timeout).\n");
    fprintf(stderr, "    -f, --tracefile=FILE        A FILE name for saving plain HTTP logs between tinyacs and CPE.\n");
    fprintf(stderr, "                                Default FILE name is \"trace.log\".\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Examples:\n");
    fprintf(stderr, "    tinyacs --port 10240 --https --keyfile ssl_server/KeyAndCA.pem \\\n");
    fprintf(stderr, "                           --cacert ssl_server/server_ca.pem  \\\n");
    fprintf(stderr, "                           --auth --userid SN001 --passwd secret  \\\n");
    fprintf(stderr, "                           --rpcdir rpc --servetimeout=3600 --tracefile trace.log\n");
    fprintf(stderr, "    tinyacs -d info -r rpc -l -1 -f trace.log\n");
    fprintf(stderr, "    tinyacs -t 3600\n");
}

static int parseCommandLine(int argc, char** argv, struct soap *soap)
{
    call_in();
    int c = 0;
    struct option long_options[] = {
        { "help",       no_argument,       0, 'h'},
        { "version",    no_argument,       0, 'v'},
        { "port",       required_argument, 0, 'p'},
        { "userid",     required_argument, 0, 'u'},
        { "passwd",     required_argument, 0, 'w'},
        { "auth",       no_argument,       0, 'c'},
        { "https",      no_argument,       0, 's'},
        { "keyfile",    required_argument, 0, 'k'},
        { "keypass",    required_argument, 0,  0 },
        { "cacert",     required_argument, 0,  1 },
        { "capath",     required_argument, 0,  2 },
        { "debuglevel", required_argument, 0, 'd'},
        { "rpcdir",     required_argument, 0, 'r'},
        { "loop",       required_argument, 0, 'l'},
        { "servetimeout",required_argument, 0, 't'},
        { "tracefile",  required_argument, 0, 'f'},
        {0, 0, 0, 0},
    };
    /* getopt_long stores the option index here. */
    int option_index = 0;

    while(1){

        c = getopt_long(argc, argv, "hvp:u:w:csk:d:r:l:t:f:", long_options, &option_index);
        /* Detect the end of the options. */
        if (c == -1)
            break;
        switch (c){
            case 'p':
                if(NULL != optarg)
                    acsh->listening_port = atoi(optarg);
                if(0 >= acsh->listening_port || 65535 <= acsh->listening_port)
                {
                    fprintf(stderr, "Invalid listening port, reset to 10240.");
                    acsh->listening_port = 10240;
                }
                break;
            case 'u':
                strncpy(acsh->userid, optarg, sizeof(acsh->userid) - 1 );
                break;
            case 'w':
                strncpy(acsh->passwd, optarg, sizeof(acsh->passwd) - 1 );
                break;
            case 'c':
                acsh->authenticate = true;
                break;
            case 's':
                acsh->https = true;
                break;
            case 'k':
                if(optarg != NULL)
                    strncpy(acsh->keyfile, optarg, sizeof(acsh->keyfile) - 1 );
                break;
            case  0 :
                if(optarg != NULL)
                    strncpy(acsh->keypass, optarg, sizeof(acsh->keypass) - 1 );
                break;
            case  1 :
                if(optarg != NULL)
                    strncpy(acsh->cacert, optarg, sizeof(acsh->cacert) - 1 );
                break;
            case  2 :
                if(optarg != NULL)
                    strncpy(acsh->capath, optarg, sizeof(acsh->capath) - 1 );
                break;
            case 'd':
                if(optarg == NULL)
                    acsh->debuglevel = DEBUG_LEVEL_NOTICE;
                else if(strncasecmp(optarg, "error", sizeof("error")) == 0)
                    acsh->debuglevel = DEBUG_LEVEL_ERROR;
                else if(strncasecmp(optarg, "notice", sizeof("notice")) == 0)
                    acsh->debuglevel = DEBUG_LEVEL_NOTICE;
                else if(strncasecmp(optarg, "info", sizeof("info")) == 0)
                    acsh->debuglevel = DEBUG_LEVEL_INFO;
                else if(strncasecmp(optarg, "debug", sizeof("debug")) == 0)
                    acsh->debuglevel = DEBUG_LEVEL_DEBUG;
                else
                    acsh->debuglevel = DEBUG_LEVEL_NOTICE;
                break;
            case 'r':
                strncpy(acsh->rpc_dir, optarg, sizeof(acsh->rpc_dir) - 1 );
                break;
            case 'l':
                //loop == -1 , infinite loop,                ->request_times = INT_MAX;
                //loop == 0, no loop, default value          ->request_times = 1;
                //loop == 1, loop 1 times,                   ->request_times = 2;
                //loop == 2 , ...
                if(atoi(optarg) == -1 )
                    acsh->request_times = INT_MAX;
                else if (atoi(optarg) >= 0)
                    acsh->request_times = atoi(optarg) + 1;
                else
                {
                    fprintf(stderr, "%s\n", "Invalid loop times! use default settings.");
                    acsh->request_times = 1;
                }
                break;
            case 't':
                if(optarg != NULL)
                {
                    if(atoi(optarg) < 0 )
                    {
                        fprintf(stderr, "%s\n", "Invalid service timeout settings! reset to default.");
                        acsh->servetimeout = 0;
                    }
                    else
                    acsh->servetimeout = atoi(optarg);
                }
                break;
            case 'f':
                if(optarg != NULL)
                    strncpy(acsh->tracefile, optarg, sizeof(acsh->tracefile) - 1 );
                break;
            case 'v':
                fprintf(stderr, "%s\n", TINYACS_VERSION);
                exit(0);
                break;
            case '?':
                /* getopt_long already printed an error message. */
                break;
            default :
                usage();
                exit(-1);
        }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        fprintf(stderr, "Non-option ARGV-elements: ");
        while (optind < argc)
            fprintf(stderr, "%s ", argv[optind++]);
        fprintf(stderr, "\n");
        hint(argv[0]);
        exit(-1);
    }

    if(acsh->authenticate && '\0' == acsh->userid[0])
    {
        fprintf(stderr, "Authentication selected, But username not specified.\n");
        hint(argv[0]);
        exit(-1);
    }
    if(acsh->authenticate && '\0' == acsh->passwd[0])
    {
        fprintf(stderr, "Authentication selected, But password not specified.\n");
        hint(argv[0]);
        exit(-1);
    }
    if(acsh->https && (access(acsh->keyfile, F_OK) != 0))
    {
        fprintf(stderr, "HTTPS selected but invalid keyfile.\n");
        hint(argv[0]);
        exit(-1);
    }
    if(acsh->https && '\0' != acsh->cacert[0] && (access(acsh->cacert, F_OK) != 0))
    {
        fprintf(stderr, "HTTPS selected, and cert specified but invalid.\n");
        hint(argv[0]);
        exit(-1);
    }
    if('\0' == acsh->tracefile[0])
    {
        fprintf(stderr, "Trace file not specified.\n");
        hint(argv[0]);
        exit(-1);
    }
    if('\0' == acsh->rpc_dir[0])
    {
        fprintf(stderr, "RPC directory not specified.\n");
        hint(argv[0]);
        exit(-1);
    }
    if(!is_dir_exists(acsh->rpc_dir))
    {
        fprintf(stderr, "RPC directory:%s not exist.\n", acsh->rpc_dir);
        hint(argv[0]);
        exit(-1);
    }
    call_out();
    return 0;
}

/**
 *
 *
 */

int main(int argc, char **argv)
{
    pacsh = malloc(sizeof(ACSHandler));
    if(NULL == pacsh)
    {
        error("Malloc failed\n");
        exit(-1);
    }
    call_in();
    struct soap *soap = soap_new1(SOAP_XML_INDENT); /* new context */
    if(NULL == soap)
    {
        error("Soap init failed\n");
        exit(-1);
    }
    soap->user = pacsh;

    /** Initialize default settings. */
    acsh->request_times = 1; 
    acsh->listening_port = 10240;
    acsh->servetimeout = 0;//Service never timeout.
    acsh->debuglevel = DEBUG_LEVEL_NOTICE;
    strncpy(acsh->keyfile, "ssl_server/KeyAndCA.pem", sizeof(acsh->keyfile) - 1 );
    strncpy(acsh->rpc_dir, "rpc", sizeof(acsh->rpc_dir) - 1 );
    strncpy(acsh->tracefile, "trace.log", sizeof(acsh->tracefile) - 1 );

    parseCommandLine(argc, argv, soap);

    acsh->authrealm = "Tiny ACS Test Tool";
    soap->x_frame_options = NULL;
    soap->bind_flags = (SO_REUSEADDR | SO_REUSEPORT);

#ifdef SOAP_DEBUG
    soap_set_test_logfile(soap, acsh->debuglevel? "/dev/stderr" : NULL);
    soap_set_sent_logfile(soap, acsh->tracefile);
    soap_set_recv_logfile(soap, acsh->tracefile);
#endif

    soap_set_mode(soap, SOAP_IO_KEEPALIVE);      /* enable HTTP keep-alive connection */
    soap_set_mode(soap, SOAP_C_UTFSTRING);      /* hold UTF-8 content  */
    soap->send_timeout = soap->recv_timeout = 60;/* max send and receive socket inactivity time (sec) */
    soap->transfer_timeout = 60;                 /* max time for send or receive of messages (sec) */
//  soap->connect_retry = 6;                     /*retry 6 more times (waiting 1, 2, 4, 8, 16, 32 seconds between retries) */

    //da handler
    if (soap_register_plugin(soap, http_da))//http_da_sha256() by default.
    {
        error("Soap http_da plugin init failed\n");
        exit(-1);
    }


    if (acsh->https)
        soap_ssl_init();

    if (acsh->https && soap_ssl_server_context(soap,
                SOAP_SSL_DEFAULT,
                acsh->keyfile,                               /* server keyfile (private key and certificate) */
                acsh->keypass[0]=='\0'?NULL:acsh->keypass,   /* password to read the private key in the keyfile */
                acsh->cacert[0]=='\0'?NULL:acsh->cacert,     /* cert to authenticate clients */
                acsh->capath[0]=='\0'?NULL:acsh->capath,     /* capath to trusted certificates */
                NULL,                                        /* DH/RSA: use 2048 bit RSA (default with NULL) */
                NULL,                                        /* no random data to seed randomness */
                NULL                                         /* no SSL session cache */
                ))
    {
        soap_print_fault(soap, stderr);
        exit(EXIT_FAILURE);
    }

    if (!soap_valid_socket(soap_bind(soap, "::", acsh->listening_port, 100)))
        soap_print_fault(soap, stderr);
    else
    {
        soap->accept_timeout = acsh->servetimeout;
        debug("Bind to port %d successful, service timeout in %d seconds\n", acsh->listening_port, soap->accept_timeout <= 0? -1:soap->accept_timeout);
        for (;;)
        {
            SOAP_SOCKET sock = soap_accept(soap);
            if (!soap_valid_socket(sock))
            {
                if (soap->errnum)
                    soap_print_fault(soap, stderr);
                else
                {
                    notice("Service timed out\n");
                    break;
                }
            }
            if (acsh->https && soap_ssl_accept(soap))
            {
                soap_print_fault(soap, stderr);
                break;
            }
            if (soap->ip)
                notice("Accepting %s connection with socket id: %d from IPv4 addr: %d.%d.%d.%d\n", acsh->https ? "https" : "http", sock, \
                    (int)(soap->ip>>24)&0xFF, (int)(soap->ip>>16)&0xFF, (int)(soap->ip>>8)&0xFF, (int)soap->ip&0xFF);
            else
            {
                struct sockaddr_in6 sa;
                char ip6str[INET6_ADDRSTRLEN];
                sa.sin6_addr.s6_addr32[0] = htonl(soap->ip6[0]);
                sa.sin6_addr.s6_addr32[1] = htonl(soap->ip6[1]);
                sa.sin6_addr.s6_addr32[2] = htonl(soap->ip6[2]);
                sa.sin6_addr.s6_addr32[3] = htonl(soap->ip6[3]);
                inet_ntop(AF_INET6, &(sa.sin6_addr), ip6str, INET6_ADDRSTRLEN);
                notice("Accepting %s connection with socket id: %d from IPv6 addr: %s\n", acsh->https ? "https" : "http", sock, ip6str);
            }
            scan_rpc_dir(soap);
            if (soap_serve_acs(soap))
            {
                if(NULL != acsh->namelist)
                {
                    free(acsh->namelist);
                    acsh->namelist = NULL;
                }
                if(soap->error == 401)
                {
                    soap_destroy(soap); /* delete deserialized objects */
                    soap_end(soap);     /* delete heap and temp data */
                    continue;
                }
                error("soap status: %d!\n", soap->status);
                error("soap error: %d!\n", soap->error);
                error("soap errnum: %d!\n", soap->errnum);
                error("soap length=%lu\n", soap->length);
                soap_print_fault(soap, stderr);
                
                //other handle-able errors
                if(soap_xml_error_check(soap->error)||
                   soap_soap_error_check(soap->error)||
                   soap_http_error_check(soap->error)||
                   soap_dime_error_check(soap->error)||
                   soap_mime_error_check(soap->error)||
                   soap_tcp_error_check(soap->error)||
                   soap_udp_error_check(soap->error)||
                   soap_ssl_error_check(soap->error)||
                   soap_zlib_error_check(soap->error)
                )
                {
                    soap_destroy(soap); /* delete deserialized objects */
                    soap_end(soap);     /* delete heap and temp data */
                    notice("Served\n\n");
                    continue;
                }
                //unable to handle errors
                if(soap->error)
                {
                    notice("Fatal error, exit.\n\n");
                    break;
                }
            }
            if(NULL != acsh->namelist)
            {
                free(acsh->namelist);
                acsh->namelist = NULL;
            }
            notice("Served\n\n");
            soap_destroy(soap); /* delete deserialized objects */
            soap_end(soap);     /* delete heap and temp data */
        }
    }

    soap_destroy(soap); /* delete deserialized objects */
    soap_end(soap);     /* delete heap and temp data */

    soap_free(soap);    /* we're done with the context, >= soap_done() */
    call_out();

    if(NULL != pacsh)
    {
        free(pacsh);
        pacsh = NULL;
    }

    return 0;
}
