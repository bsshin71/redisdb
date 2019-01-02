/*
 * select_test.cpp
 *
 *  Created on: 2018. 10. 3.
 *      Author: omegaman
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "redisdb.h"
void select(int count)
{
    CRedisDb *rdb = new CRedisDb();
    REDIS_HASH_REC  *records[5];
    REDIS_HASH_REC  **getresult;
    int             rc_count;

    rdb->InitDb("127.0.0.1", 6379, "", require_nopass );

    if ( ! rdb->OpenConnect() ) {
        printf("success to connect redis\n");
    } else {
        printf("failed to connect redis errmsg=[%s]\n", rdb->GetLastErrMsg() );
    }

    if( rdb->IsConnected() ) {
        printf(" Ping success..db is alive..\n");
    } else {
        printf(" Ping failed..db is dead..\n");
    }

for(int i =0 ; i < count; i++ )
{
    // select test
     int rows = rdb->SelectHashSet(NULL, "orderbook:MTC");
     printf("returned rows=%d\n", rows );
     for(int i = 0; i < rows; i++ )
     {
         char tempcol[128];
         rdb->Row2Str( NULL, i, tempcol );
         printf(" get rows[%d] = [%s]\n", i, tempcol );
         rdb->MoveNext(NULL);
     }
     rdb->FreeQuerySelect( NULL);
}


    rdb->DisConnect();

    delete(rdb);

}
int main(int argc, char *argv[])
{
    select( 10 );
}
