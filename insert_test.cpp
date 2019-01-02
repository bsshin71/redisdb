/*
 * insert_test.cpp
 *
 *  Created on: 2018. 10. 3.
 *      Author: omegaman
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "redisdb.h"

void insert( int count )
{
        CRedisDb *rdb = new CRedisDb();
        REDIS_HASH_REC  *records[5];
        REDIS_HASH_REC  **getresult;
        int             rc_count;

        char *keyname = "orderbook:MTC";

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
        // insert test...
        rc_count = sizeof( records) / sizeof( REDIS_HASH_REC *);
        printf(" sizeof(records)=[%d] /sizeof( REDIS_HASH_REC *) = [%d]\n", sizeof(records), sizeof( REDIS_HASH_REC *) );
        for(int i=0; i < rc_count; i++ )
        {
            records[i] = (REDIS_HASH_REC *)malloc( sizeof(REDIS_HASH_REC) );
            memset( records[i], 0x00, sizeof(REDIS_HASH_REC) );

        }

        strcpy( records[0]->field, "coin_type");
        strcpy( records[0]->value, "MTC"      );

        strcpy( records[1]->field, "coin_pair" );
        strcpy( records[1]->value, "MTC-KRW"   );

        strcpy( records[2]->field, "update_dt" );
        strcpy( records[2]->value, "12345676"  );

        strcpy( records[3]->field, "ask_tot_cnt" );
        strcpy( records[3]->value, "11111111111" );

        strcpy( records[4]->field, "bid_tot_cnt" );
        strcpy( records[4]->value, "222222222"   );

for( int i=0; i < count; i++ )
{
        if( rdb->PutHashSet( keyname, records, rc_count ) ) {
            printf(" success to save records into redis...\n");
        } else {
            printf(" faild to save records into redis...errmsg=[%s]\n" , rdb->GetLastErrMsg() );
        }
}
        for(int i=0; i < rc_count; i++ )
        {
            free( records[i] );
        }


        rdb->DisConnect();

        delete(rdb);

}

int main(int argc, char *argv[])
{
    insert( 1000 );
}
