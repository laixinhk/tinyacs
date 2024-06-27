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

#include <dirent.h> 
#include <sys/stat.h> 
#include <stdbool.h>

#include "cwmpH.h"


extern int authentication(struct soap *soap);
extern int send_request(struct soap *soap);

int cwmp__Fault(
        struct soap *soap,
        struct _cwmp__Fault *cwmp__Fault)
{
    call_in();

    soap->keep_alive = 0;    //set Connection: close
    soap_response(soap, SOAP_OK);
    soap_end_send(soap);
    soap_closesock(soap);
    call_out();
    return SOAP_STOP; 
}


int cwmp__Inform(
        struct soap *soap,
        struct cwmp__DeviceIdStruct *DeviceId, 
        struct EventList *Event, 
        unsigned int MaxEnvelopes, 
        time_t CurrentTime, 
        unsigned int RetryCount, 
        struct ParameterValueList *ParameterList, 
        struct _cwmp__InformResponse *response)
{
    call_in();

    if(acsh->authenticate && authentication(soap))
    {
        call_out();
        return 401;
    }

    for(int i = 0; i < ParameterList->__size; i++)
    {
        debug("%s, %s\n", ParameterList->__ptrParameterValueStruct[i][0].Name, \
                ParameterList->__ptrParameterValueStruct[i][0].Value);
        if(strstr(ParameterList->__ptrParameterValueStruct[i][0].Name, "ConnectionRequestURL"))
        {
            info("ConnectionRequestURL received form CPE, run the following command to trigger CPE to send INFORM: \n");
            info("curl --no-buffer --digest -u '$(username):$(password)' -H 'Expect:' -H 'Connection: close' %s\n", \
                    ParameterList->__ptrParameterValueStruct[i][0].Value);
            info("Obtain $(username) from Device.ManagementServer.ConnectionRequestUsername\n");
            info("Obtain $(password) from Device.ManagementServer.ConnectionRequestPassword\n");
        }
    }

    call_out();
    response->MaxEnvelopes = 1;
    return SOAP_OK;
}

int cwmp__GetParameterNamesResponse(
        struct soap *soap,
        struct _cwmp__GetParameterNamesResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}

int cwmp__GetParameterValuesResponse(//test OK
        struct soap *soap,
        struct _cwmp__GetParameterValuesResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}


int cwmp__SetParameterValuesResponse(
        struct soap *soap,
        struct _cwmp__SetParameterValuesResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}


int cwmp__GetParameterAttributesResponse(
        struct soap *soap,
        struct _cwmp__GetParameterAttributesResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}


int cwmp__SetParameterAttributesResponse(
        struct soap *soap,
        struct _cwmp__SetParameterAttributesResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}


int cwmp__DownloadResponse(
        struct soap *soap,
        struct _cwmp__DownloadResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}


int cwmp__UploadResponse(
        struct soap *soap,
        struct _cwmp__UploadResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}


int cwmp__AddObjectResponse(
        struct soap *soap,
        struct _cwmp__AddObjectResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}


int cwmp__DeleteObjectResponse(
        struct soap *soap,
        struct _cwmp__DeleteObjectResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}


int cwmp__GetRPCMethodsResponse(
        struct soap *soap,
        struct _cwmp__GetRPCMethodsResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}

int cwmp__ChangeDUStateResponse(
        struct soap *soap,
        struct _cwmp__ChangeDUStateResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}

int cwmp__RebootResponse(
        struct soap *soap,
        struct _cwmp__RebootResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}

int cwmp__FactoryResetResponse(
        struct soap *soap,
        struct _cwmp__FactoryResetResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}

int cwmp__ScheduleInformResponse(
        struct soap *soap,
        struct _cwmp__ScheduleInformResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}

int cwmp__ScheduleDownloadResponse(
        struct soap *soap,
        struct _cwmp__ScheduleDownloadResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}



int cwmp__CancelTransferResponse(
        struct soap *soap,
        struct _cwmp__CancelTransferResponse *response)
{
    call_in();

    if(acsh->lastfile_sent)
    {
        soap->keep_alive = 0;    //set Connection: close
        soap_response(soap, SOAP_OK);
        soap_end_send(soap);
        soap_closesock(soap);
    }
    else// send next request
    {
        send_request(soap);
    }
    call_out();
    return SOAP_STOP; 
}


int cwmp__TransferComplete(
        struct soap *soap,
        struct _cwmp__TransferCompleteResponse *response)
{
    call_in();
    call_out();
    return SOAP_OK;
}

int cwmp__DUStateChangeComplete(
        struct soap *soap,
        struct _cwmp__DUStateChangeCompleteResponse *response)
{
    call_in();
    call_out();
    return SOAP_OK;
}

int cwmp__echoString(struct soap *soap, char *arg, struct cwmp__echoString *response)
{
    call_in();
    response->arg = arg;

    if(acsh->authenticate && authentication(soap))
    {
        call_out();
        return 401;
    }

    call_out();
    return SOAP_OK;
}
