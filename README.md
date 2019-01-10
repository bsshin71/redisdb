# redisdb C++ class
C++ wrapping class for convenient use of  redis

메모리 DB인 redis db를 사용하기 위한  C 언어 library 인   hiredis  library를  좀더 쉽게 사용할 수 있도록 C++ class 로 wrapping 하였다.



# 소스 구성

| 파일명          | 용도                                     |
| --------------- | ---------------------------------------- |
| makefile        | sample 소스 컴파일을 위한 makefile       |
| redisdb.h       | redisdb C++ class  header                |
| redisdb.cpp     | redisdb C++ class member function source |
| select_test.cpp | redisdb select 예제                      |
| insert_test.cpp | redisdb insert 예제                      |



# 설치 및 컴파일

hiredis C library가 필요하다. 

https://github.com/redis/hiredis  의 설명에 따라서  library를 build한다.  build가 성공하면 /usr/local/lib/libhiredis.a 에  hiredis  C-library가 생성된다.

생성된 hiredis  C-library  경로에 맞게 makefile 을 수정한다.

makefile  수정이 끝나면  테스트 프로그램을 build 한다.

```
make   # redis 에 간단한 data를 insert 하고 select 하는 예제프로그램을 생성한다.
```



# 실행

```
$ ./insert_test   # insert test 를 수행한다.
$ ./select_test   # select test 를 수행한다.
```

