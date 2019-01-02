/*
 * redisdb.cpp
 *
 *  Created on: 2018. 10. 2.
 *      Author: omegaman
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "redisdb.h"

CRedisDb::CRedisDb()
{
    this->m_qs.row_pos   = 0;
    this->m_qs.row_count = 0;
    this->m_qs.reply      = NULL;
}

CRedisDb::~CRedisDb()
{
}

void CRedisDb::InitDb( const char*  hostname,
                       int          port,
                       const char*  password,
                       AUTH_MODE    auth_mode )
{
    struct timeval tv  ={ 2, 500000 }; /* 2.5 seconds */

    CRedisDb::InitDb( hostname, port, password, auth_mode, tv ) ;
}


void CRedisDb::InitDb( const char*  hostname,
                     int          port,
                     const char*  password,
                     AUTH_MODE    auth_mode,
                     const struct timeval tv  )
{
    this->hostname  = (char *)hostname;
    this->port      = port;
    this->password  = (char *)password;
    this->auth_mode = auth_mode;
    this->tv        = tv;
}

bool CRedisDb::OpenConnect()
{

    c = redisConnectWithTimeout(hostname, port, this->tv );
    if ( c == NULL || c->err ) {
        if ( c ) {
            sprintf( this->error_string, "Connection error: %s\n", c->errstr );
            redisFree(c);
            c = NULL;
        }
        return false;
    }

    if( this->auth_mode == requirepass ) {
        // do require mode..
    } else {

    }
}

bool CRedisDb::ReConnect()
{
    CRedisDb::DisConnect();

    // or use ... redisReconnect(this->c);
    return CRedisDb::OpenConnect();
}

void CRedisDb::DisConnect()
{
    if( this->reply != NULL )   CRedisDb::FreeReply( this->reply );
    if( this->c     != NULL ) {
           redisFree( c );
           c = NULL;
    }
}

bool CRedisDb::RunCommandV(const char* format, va_list ap)
{
   this->reply =  ( redisReply * )redisvCommand( this->c, format, ap );

   return CRedisDb::IsError();
}

bool CRedisDb::RunCommand(const char* format, ...)
{
    va_list   args;
    va_start( args, format );

    this->reply = ( redisReply *)redisvCommand ( this->c,  format, args );
    va_end( args ) ;

    return CRedisDb::IsError();
}

bool CRedisDb::RunCommandArgv(int argc, const char** argv, const size_t* argvlen)
{
    this->reply = ( redisReply *)redisCommandArgv ( this->c, argc, argv, argvlen );

    return CRedisDb::IsError();
}

bool CRedisDb::IsError()
{

    if ( this->c->err || this->reply == NULL || this->reply->type ==  REDIS_REPLY_ERROR ) {
        return true;
    }

    return false;
}

bool CRedisDb::IsError( char *step )
{
    if ( this->c->err || this->reply == NULL || this->reply->type ==  REDIS_REPLY_ERROR ) {
        sprintf( this->error_string, "%s: %s", step,  c->errstr );
        return true;
    }
    //if ( this->reply->type ==  REDIS_REPLY_ERROR ) return true;

    return false;
}

void CRedisDb::SetErrMsg(char* msg, char *step)
{
    sprintf( this->error_string, "%s: %s", step,  msg );
}

void CRedisDb::SetErrMsg(char* msg)
{
    strncpy( this->error_string, msg, strlen(msg) );
}

bool CRedisDb::IsConnected()
{
    char response[256] = { 0, };

    this->reply = ( redisReply *) redisCommand( c,"PING" );
    strncpy( response, reply->str,  4 ); //expect "PONG"
    CRedisDb::FreeReply( this->reply  );
    this->reply = NULL;

    if( !strncmp(response, "PONG", 4) ) {
        return true;
    }

    return false;
}

// Hash type 의 레코드를 저장한다.
bool CRedisDb::PutHashSet( char *key, REDIS_HASH_REC *columns[], int columnnum )
{
     int       ret;
     bool      bc;

     redisReply **replies;

     for ( int i = 0; i < columnnum;  i++ )
     {
        printf(" ===== going to save columname=[%s], value=[%s]\n", columns[i]->field, columns[i]->value);
        ret = redisAppendCommand( this->c, "HSET %s %s %s", key, columns[i]->field, columns[i]->value );
        if ( ret == REDIS_ERR ) {
            this->SetErrMsg( this->c->errstr ,"redisAppendCommand" );
            return false;
        }
     }

     replies = (redisReply **) malloc(sizeof(redisReply*)*columnnum );

     bc = true;
     for ( int i = 0; i < columnnum; i++) {
          ret = redisGetReply( this->c, (void **)&replies[i] );
          if ( ret == REDIS_ERR ) {
                this->SetErrMsg( this->c->errstr , "redisGetReply" );
                bc = false;
          }
     }

     CRedisDb::FreeReplies( replies, columnnum );

     return bc;
}

int CRedisDb::GetHashSet( char  *key, REDIS_HASH_REC *record[] )
{
    int rowcount;
    int rowindex;

    rowcount =  CRedisDb::SelectHashSet( NULL, key );
    rowindex = 0;

    for(int i=0; i < rowcount; i++ )
    {
        record[rowindex] = (  REDIS_HASH_REC * ) malloc( sizeof( REDIS_HASH_REC ) );
        memset( record[rowindex], 0x00, sizeof( REDIS_HASH_REC ) );
        CRedisDb::Row2Str( NULL, i++, record[rowindex]->field );
        CRedisDb::Row2Str( NULL, i++, record[rowindex]->value );
        //CRedisDb::MoveNext( NULL );
        rowindex++;
    }
    CRedisDb::FreeQuerySelect( NULL);

    return rowindex;
}

int  CRedisDb::SelectHashSet (REDISQUERYSELECT *qs, char  *key )
{
    int       ret;
    bool      bc;
    int       col_index;
    int       col_count;

    REDISQUERYSELECT *tqs = (qs == NULL) ? &m_qs : qs;

    ret = redisAppendCommand( this->c, "HGETALL %s", key  );
    if ( ret == REDIS_ERR ) {
       this->SetErrMsg( this->c->errstr ,"GetHashSet:redisAppendCommand" );
       tqs->row_count = 0;
       tqs->row_pos   = 0;
       return 0;
    }

    ret            = redisGetReply( this->c , (void**)&tqs->reply);
    tqs->row_pos   = 0;
    tqs->row_count = tqs->reply->elements;  // number of rows returned.
    tqs->row.key   = key;

    return tqs->row_count ;
}

bool CRedisDb::Move( REDISQUERYSELECT *qs, int n )
{
    REDISQUERYSELECT *tqs = (qs == NULL) ? &m_qs : qs;

    if(tqs->row_pos+n < 0 || tqs->row_pos+n >= tqs->row_count)
        return false;

    tqs->row_pos += n;
}

bool CRedisDb::MoveNext( REDISQUERYSELECT *qs )
{
    return Move(qs,1);
}

bool CRedisDb::MovePre( REDISQUERYSELECT *qs )
{
    return Move(qs,-1);
}

bool CRedisDb::MoveFirst( REDISQUERYSELECT *qs )
{
    REDISQUERYSELECT *tqs = (qs == NULL) ? &m_qs : qs;
    tqs->row_pos = 0;
}

void CRedisDb::FreeQuerySelect( REDISQUERYSELECT *qs)
{
    REDISQUERYSELECT *tqs = (qs == NULL) ? &m_qs : qs;

    CRedisDb::FreeReply( tqs->reply );
}

void CRedisDb::Row2Str( REDISQUERYSELECT *qs,int iRow, char *dest )
{
    REDISQUERYSELECT *tqs = (qs == NULL) ? &m_qs : qs;

    if(tqs->row_count <= 0)
            return;
    if( iRow < tqs->row_count )
        strcpy(dest,tqs->reply->element[iRow]->str);
    else
        strcpy(dest,"");

}

void CRedisDb::FreeReply(redisReply* reply)
{
    if( reply != NULL ) {
        freeReplyObject(reply);
        reply = NULL;
    }
}
void CRedisDb::FreeReplies(  redisReply *replies[] , int num )
{
    for ( int i = 0; i < num; i++ )
    {
        if( replies[i] != NULL ) {
            freeReplyObject(replies[i]);
            replies[i] = NULL;
        }
    }

    if( replies != NULL ) {
        free( replies );
        replies = NULL;
    }
}
char* CRedisDb::GetLastErrMsg()
{
    return this->error_string;
}
