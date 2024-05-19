# ugalib

It's a rainy day, you're stuck in your cave and the loneliness is starting to eat away at your mental health?  
The mammoths are getting smarter and you have to coordinate your hunts more strategically, but you haven't developed language yet?  
Fear no more, cause with `ugalib` your friends are only an `uga_send` away!


### contents

* [getting started](#getting_started)
  * [installation](#installation)
  * [usage](#usage)
* [the library](#the_library)
  * [overview](#overview)
  * [types](#types)
  * [string](#string)
  * [string_view](#string_view)
  * [vector](#vector)
  * [list](#list)
  * [err](#err)
  * [fs](#fs)
  * [socket](#socket)
  * [talk](#talk)
  * [multiplexing](#multiplexing)

## <a name = "getting_started"></a> getting started<br/>

### <a name = "installation"></a> installation<br/>

install with:
```bash
git clone https://github.com/eddieavd/ugalib
cd ugalib

make         # builds library, examples and tests, runs tests
# or
make libonly # pretty self-explanatory
# or
make test    # builds library and tests, runs tests

make install
```
installs headers to `/usr/include/ugalib` and library to `/usr/lib/libugalib.a`  
_<sub>(nothing is configurable at the moment, feel free to change things in the Makefile)</sub>_

uninstall with:
```bash
make clean    # removes local build files
# or
make cleanall # removes build files and any installed files
```

### <a name = "usage"></a> usage<br/>

```c
// main.c

#include <uganet.h>


void handle_connection ( uga_socket client ) ;

int main ( void )
{
    uga_socket socket = uga_sock_create_and_listen( "8080", 1 ) ;
    uga_print_abort_if_err() ;

    uga_socket client = uga_accept( socket ) ;
    uga_print_abort_if_err() ;

    handle_connection( client ) ;

    uga_sock_close( socket ) ;

    return 0 ;
}
```

```bash
cc main.c -o server -lugalib
./server
```

## <a name = "the_library"></a> the library<br/>

### <a name = "overview"></a> overview<br/>

ugalib networking closely follows [Beej's Guide to Network Programming](https://beej.us/guide/bgnet) and additionally provides centralized error handling and other luxuries such as dynamic strings (don't forget to call the destructor!). 

### <a name = "types"></a> types<br/>

the library provides some basic types (eg. sized integers) and wrappers for low level sockets API structs (eg. `uga_addrinfo`). 

### <a name = "string"></a> string<br/>

the `uga_string` can be created from an existing c string, a pointer and a length, or default, copy or move constructed from another `uga_string` (kinda like c++'s special member funcs).

```c
uga_string str = uga_str_create_from_1( "cstring" ) ;

uga_string copy = uga_str_copy( &str ) ;
uga_string move = uga_str_move( &str ) ;

assert( uga_str_empty( &str ) ) ;

uga_str_append( &copy, &move ) ;

printf( STR_FMT, STR_ARG( copy ) ) ; // prints 'cstringcstring'
```

### <a name = "string_view"></a> string_view<br/>

a tiny string_view that comes in handy for parsing stuff

```c
...
uga_string source ; // holds a bunch of source code

uga_string_view view = uga_sv_create_from( &source ) ;

while( !uga_sv_empty( &view ) )
{
    uga_string_view line = uga_sv_chop_to_delimiter( &view, '\n' ) ;

    if( !parse_line( line ) )
    {
        uga_sv_unchop_left( &view, line.size ) ;
    }
}
...
```

### <a name = "vector"></a> vector<br/>

The `uga_vector` is a generic contiguous container with dynamic memory management.

```c
uga_vector vector = uga_vec_create_0( i32_t ) ;

for( i32_t i = 0; i < 10; ++i )
{
    uga_vec_push_back( &vector, &i ) ;
}
...
for( i32_t i = 0; i < 10; ++i )
{
    i32_t * val = uga_vec_at( &vector, i ) ;
    ...
}
...
uga_vec_destroy( &vec ) ;
```

You can also store types which have custom destructors:

```c
uga_vector vector = uga_vec_create_d_0( uga_string, uga_str_destroy_void ) ;

for( i32_t i = 0; i < 10; ++i )
{
    uga_string str = get_some_string() ;
    uga_vec_push_back( &vector, &str ) ;
}
...
uga_vec_destroy( &vec ) ; // calls string destructors before deallocating
```

### <a name = "list"></a> list<br/>

`ugalib` provides singly and doubly linked list implementations:

```c
uga_sl_list slist = uga_sl_list_create( i32_t ) ;
uga_dl_list dlist = uga_dl_list_create( i32_t ) ;

for( i32_t i = 0; i < 10; ++i )
{
    i32_t val = get_some_val() ;
    uga_sl_list_push_back ( &slist, &val ) ;
    uga_dl_list_push_front( &dlist, &val ) ;
}
...
uga_sl_list_destroy( &slist ) ;
uga_dl_list_destroy( &dlist ) ;
```

### <a name = "err"></a> err<br/>

After a call to a library function, you can check for errors by calling `uga_had_errs()`. You can also handle errors automatically by calling one of `uga_print_if_err()`, `uga_abort_if_err()` or `uga_print_abort_if_err()` after a library call.

```c
uga_socket = uga_sock_create_and_listen( "8080", 16 ) ;
uga_print_abort_if_err() ;
```

To get more details on the error, you can fetch the error data with `uga_current_error()`. The returned `uga_error` struct will contain `uga_errtype` and `uga_err_category`. For a basic error string, you can call `uga_strerror( uga_errtype )` or simply `uga_print_errtype()`. A detailed description can be printed with `uga_print_error()` or obtained by casting the `uga_error` to the corresponding error struct according to the `uga_err_category`.

```c
uga_string string = uga_str_create_from_1( "string" ) ; // size == 6

char val = uga_str_at( &string, 10 ) ; // index out of bounds

if( uga_current_error().cat == UGA_CAT_MEM )
{
    uga_error_mem const * errdata = uga_get_mem_errdata() ;

    // errdata->size is the size of the allocated block
    // errdata->pos  is the index we tried accessing
}
```

### <a name = "fs"></a> fs<br/>

`uga_fs.h` provides some basic filesystem utilities for checking filesizes, reading from and writing to files.

```c
char const * filename = "somefile.txt" ;

uga_string data = uga_str_create_0() ;

uga_fs_read_file_into_str( filename, &data ) ;

// do stuff to contents

uga_fs_write_file_str( filename, &data ) ;
```

### <a name = "socket"></a> socket<br/>

creating a new socket can be done like this:
```c
uga_socket sock = uga_sock_create( "www.github.com", "443" ) ;
```
connecting and listening is also straightforward:
```c
uga_socket connected = uga_sock_create_and_connect( "www.github.com", "https" ) ;
...
uga_socket srv = uga_sock_create_and_listen( "8080", 16 ) ;
```

`ugalib` uses a global `uga_sock_conf` for socket configuration and can be updated like this:
```c
uga_sock_conf config = uga_sock_get_config() ;

config.family = UGA_SFAM_IPv4   ;
config.type   = UGA_SOCK_STREAM ;
config.flags  = UGA_CANONNAME   ;

uga_sock_set_config( config ) ;
```

You can also pass in a config on each call to avoid having to update the global config all the time:
```c
uga_sock_conf config = { UGA_SFAM_UNSPEC, UGA_SOCK_DGRAM, 0 } ;

uga_socket sock = uga_sock_create_configured( hostname, servname, &config ) ;
```

### <a name = "talk"></a> talk<br/>

Once you have your sockets connected, sending and receiving is similar to the sockets API, but you can additinally use `uga_string`s instead of raw buffers:
```c
// server.c

int main ( void )
{
    uga_socket socket = uga_sock_create_and_listen( "8372", 16 ) ;
    uga_socket client = uga_sock_accept( socket ) ;

    uga_string message = uga_recv_str( &client, 0 ) ;

    printf( "received message: " STR_FMT, STR_ARG( message ) ) ;

    return 0 ;
}
```
```c
// client.c

int main ( void )
{
    uga_socket sock = uga_sock_create_and_connect( "127.0.0.1", "8372" ) ;

    uga_send( &sock, "hello", 5, 0 ) ;

    return 0 ;
}
```

### <a name = "multiplexing"></a> multiplexing<br/>

WiP
