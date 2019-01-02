/*
 * redisdb.h
 *
 *  Created on: 2018. 10. 2.
 *      Author: omegaman
 */

#ifndef _REDISDB_H_
#define _REDISDB_H_

#include <stdio.h>
#include <stdarg.h>
#ifdef _WINDOWS
#include <windows.h>
#endif

#include <hiredis.h>
#include <stdarg.h> /* for va_list */

#define MAX_REDIS_COLUMN_NUM  70

enum AUTH_MODE
{
    requirepass    = 0,
    require_nopass = 1
};

typedef struct _stREDIS_HASH_REC_
{
    char   field[50];
    char   value[128];
} REDIS_HASH_REC ;

typedef struct _stREDIS_HASH_PREC_
{
    char   *key;
    char   *field;
    char   *value;
} REDIS_HASH_PREC ;

typedef struct  _stREDISQUERYSELECT_
{
  int              row_pos;
  int              row_count;
  redisReply       *reply;
  REDIS_HASH_PREC  row;
} REDISQUERYSELECT ;

class CRedisDb
{
protected:
    char           *hostname;
    int            port;
    char           *password;
    bool           auth_mode;
    struct timeval tv  ={ 2, 500000 }; /* 2.5 seconds */
    redisContext   *c;
    redisReply     *reply =  NULL;

    REDISQUERYSELECT m_qs;

    char error_string[ sizeof(c->errstr)* 2];
public:
    CRedisDb();
    ~CRedisDb();

    void InitDb(  const char  *hostname   ,
                  int          port       ,
                  const char   *password  ,
                  AUTH_MODE    auth_mode  );

    void InitDb(  const char  *hostname   ,
                  int          port       ,
                  const char   *password  ,
                  AUTH_MODE    auth_mode  ,
                  const struct timeval tv  );

    bool OpenConnect(  );
    bool ReConnect();
    void DisConnect();
    void FreeReply     ( redisReply   *reply );
    void PrepareReplies( redisReply   **reply, int num);
    void FreeReplies   ( redisReply   **reply, int num);

    bool RunCommandV   ( const char *format, va_list ap  );
    bool RunCommand    ( const char *format, ... );
    bool RunCommandArgv( int argc, const char **argv, const size_t *argvlen );

    bool PutHashSet( char *key, REDIS_HASH_REC *columns[], int columnnum );
    int  GetHashSet (char  *key, REDIS_HASH_REC *record[] );

    int  SelectHashSet (REDISQUERYSELECT *qs, char  *key );
    bool Move      ( REDISQUERYSELECT *qs, int n );
    bool MoveNext  ( REDISQUERYSELECT *qs );
    bool MovePre   ( REDISQUERYSELECT *qs );
    bool MoveFirst ( REDISQUERYSELECT *qs );
    bool MoveLast  ( REDISQUERYSELECT *qs );
    int  GetRowCount(REDISQUERYSELECT *qs);
    void Row2Str    (REDISQUERYSELECT *qs,int iRow, char *dest);
    void FreeQuerySelect( REDISQUERYSELECT *qs);

    bool IsConnected();
    bool IsError( char *step );
    bool IsError( );
    void SetErrMsg(char *msg);
    void SetErrMsg(char* msg, char *step);
    char *GetLastErrMsg();
};

#endif _REDISDB_H_
