#ifndef SOAPDEFS_H
#define SOAPDEFS_H

#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

#define SOAP_DEBUG
#define SOAP_MAXLOGS (2)
#define SOAP_INDEX_RECV (0)
#define SOAP_INDEX_SENT (0)
#define SOAP_INDEX_TEST (1)

#define DEBUG_LEVEL_ERROR  (0)
#define DEBUG_LEVEL_NOTICE (1)
#define DEBUG_LEVEL_INFO (2)
#define DEBUG_LEVEL_DEBUG (3)


typedef struct {
	char* authrealm;
	char userid[1024];
	char passwd[1024];
	int listening_port;
	char rpc_dir[1024];
	char tracefile[1024];
	int debuglevel;
	struct dirent **namelist;
	int filetosend;
	int filecount;
	int request_times;
	bool authenticate;
	bool https;
	char keyfile[1024];
	char keypass[1024];
	char cacert[1024];
	char capath[1024];
	int servetimeout;
	bool lastfile_sent;
}ACSHandler;

extern ACSHandler* pacsh;
#define acsh ((ACSHandler*)(soap->user))

//debug level

#  define DBGHEX(DBGFILE, MSG, LEN) \
{ if (soap && acsh->debuglevel >= DEBUG_LEVEL_DEBUG)\
  { if (!soap->fdebug[SOAP_INDEX_##DBGFILE])\
      soap_open_logfile(soap, SOAP_INDEX_##DBGFILE);\
    if (soap->fdebug[SOAP_INDEX_##DBGFILE])\
    { int i; char *s;\
      for (s = (char*)(MSG), i = (LEN); i; i--)\
        fprintf(soap->fdebug[SOAP_INDEX_##DBGFILE], "%2.2X  ", (int)*s++&0xFF);\
      fflush(soap->fdebug[SOAP_INDEX_##DBGFILE]);\
    }\
  }\
}


#  define SOAP_MESSAGE fprintf
#    define DBGLOG(DBGFILE, CMD) \
{ if (soap && acsh->debuglevel >= DEBUG_LEVEL_DEBUG)\
  { if (!soap->fdebug[SOAP_INDEX_##DBGFILE])\
      soap_open_logfile((struct soap*)soap, SOAP_INDEX_##DBGFILE);\
    if (soap->fdebug[SOAP_INDEX_##DBGFILE])\
    { FILE *fdebug = soap->fdebug[SOAP_INDEX_##DBGFILE];\
      struct timeval _tv;\
      struct tm _tm;\
      gettimeofday(&_tv, NULL);\
      localtime_r(&_tv.tv_sec, &_tm);\
      fprintf(fdebug, "%04d%02d%02d %02d:%02d:%02d.%06ld|", (int)_tm.tm_year+1900, (int)_tm.tm_mon+1, (int)_tm.tm_mday, (int)_tm.tm_hour, (int)_tm.tm_min, (int)_tm.tm_sec, (long)_tv.tv_usec);\
      fprintf(fdebug, "%s(%d): ", __FUNCTION__, __LINE__);CMD;\
      fflush(fdebug);\
    }\
  }\
}

#define debug(...)        ({ \
      if (pacsh && pacsh->debuglevel >= DEBUG_LEVEL_DEBUG)\
      { \
      struct timeval _tv;\
      struct tm _tm;\
      gettimeofday(&_tv, NULL);\
      localtime_r(&_tv.tv_sec, &_tm);\
      fprintf(stderr, "%04d%02d%02d %02d:%02d:%02d.%06ld|", (int)_tm.tm_year+1900, (int)_tm.tm_mon+1, (int)_tm.tm_mday, (int)_tm.tm_hour, (int)_tm.tm_min, (int)_tm.tm_sec, (long)_tv.tv_usec);\
      fprintf(stderr, "%s(%d): ", __FUNCTION__, __LINE__);\
      fprintf(stderr, __VA_ARGS__);\
      }\
      })

//info level
#define call_in()        ({ \
      if (pacsh && pacsh->debuglevel >= DEBUG_LEVEL_INFO)\
      { \
      struct timeval _tv;\
      struct tm _tm;\
      gettimeofday(&_tv, NULL);\
      localtime_r(&_tv.tv_sec, &_tm);\
      fprintf(stderr, "%04d%02d%02d %02d:%02d:%02d.%06ld|", (int)_tm.tm_year+1900, (int)_tm.tm_mon+1, (int)_tm.tm_mday, (int)_tm.tm_hour, (int)_tm.tm_min, (int)_tm.tm_sec, (long)_tv.tv_usec);\
      fprintf(stderr, "%s(%d): In\n", __FUNCTION__, __LINE__);\
      }\
      })

#define call_out()        ({ \
      if (pacsh && pacsh->debuglevel >= DEBUG_LEVEL_INFO)\
      { \
      struct timeval _tv;\
      struct tm _tm;\
      gettimeofday(&_tv, NULL);\
      localtime_r(&_tv.tv_sec, &_tm);\
      fprintf(stderr, "%04d%02d%02d %02d:%02d:%02d.%06ld|", (int)_tm.tm_year+1900, (int)_tm.tm_mon+1, (int)_tm.tm_mday, (int)_tm.tm_hour, (int)_tm.tm_min, (int)_tm.tm_sec, (long)_tv.tv_usec);\
      fprintf(stderr, "%s(%d): Out\n", __FUNCTION__, __LINE__);\
      }\
      })

#define info(...)        ({ \
      if (pacsh && pacsh->debuglevel >= DEBUG_LEVEL_INFO)\
      { \
      struct timeval _tv;\
      struct tm _tm;\
      gettimeofday(&_tv, NULL);\
      localtime_r(&_tv.tv_sec, &_tm);\
      fprintf(stderr, "%04d%02d%02d %02d:%02d:%02d.%06ld|", (int)_tm.tm_year+1900, (int)_tm.tm_mon+1, (int)_tm.tm_mday, (int)_tm.tm_hour, (int)_tm.tm_min, (int)_tm.tm_sec, (long)_tv.tv_usec);\
      fprintf(stderr, "%s(%d): ", __FUNCTION__, __LINE__);\
      fprintf(stderr, __VA_ARGS__);\
      }\
      })

//notice level
#define notice(...)        ({ \
      if (pacsh && pacsh->debuglevel >= DEBUG_LEVEL_NOTICE)\
      { \
      struct timeval _tv;\
      struct tm _tm;\
      gettimeofday(&_tv, NULL);\
      localtime_r(&_tv.tv_sec, &_tm);\
      fprintf(stderr, "%04d%02d%02d %02d:%02d:%02d.%06ld|", (int)_tm.tm_year+1900, (int)_tm.tm_mon+1, (int)_tm.tm_mday, (int)_tm.tm_hour, (int)_tm.tm_min, (int)_tm.tm_sec, (long)_tv.tv_usec);\
      fprintf(stderr, "%s(%d): ", __FUNCTION__, __LINE__);\
      fprintf(stderr, __VA_ARGS__);\
      }\
      })

//error level
#define error(...)        ({ \
      if (1)\
      { \
      struct timeval _tv;\
      struct tm _tm;\
      gettimeofday(&_tv, NULL);\
      localtime_r(&_tv.tv_sec, &_tm);\
      fprintf(stderr, "%04d%02d%02d %02d:%02d:%02d.%06ld|", (int)_tm.tm_year+1900, (int)_tm.tm_mon+1, (int)_tm.tm_mday, (int)_tm.tm_hour, (int)_tm.tm_min, (int)_tm.tm_sec, (long)_tv.tv_usec);\
      fprintf(stderr, "%s(%d): ", __FUNCTION__, __LINE__);\
      fprintf(stderr, __VA_ARGS__);\
      }\
      })

#endif
