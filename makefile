REDIS_LIBDIR=/usr/local/lib
REDIS_INCLUDE=/usr/local/include/hiredis

INCLUDES+=-I${REDIS_INCLUDE}  -I.  -I/usr/include -I../../../kx.framework/include
LIB_PATH= -L${REDIS_LIBDIR}  -L/usr/local/lib -L../../../../lib

CFLAGS=-g -W -Wall 
TARGET=insert_test select_test

all: ${TARGET} 

insert_test: insert_test.cpp redisdb.cpp redisdb.h 
        g++ -c ${CFLAGS}  ${INCLUDES} redisdb.cpp
        g++ -c ${CFLAGS}  ${INCLUDES} insert_test.cpp
        g++ redisdb.o insert_test.o -o insert_test ${LIB_PATH} -ldl -lm -lrt -lhiredis -lkxcomn 
select_test: select_test.cpp redisdb.cpp redisdb.h 
        g++ -c ${CFLAGS}  ${INCLUDES} redisdb.cpp
        g++ -c ${CFLAGS}  ${INCLUDES} select_test.cpp
        g++ redisdb.o select_test.o -o select_test ${LIB_PATH} -ldl -lm -lrt -lhiredis -lkxcomn 
clean: 
        rm -rf *.o insert_test select_test
