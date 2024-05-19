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
  * [core](#core)
    * [types](#types)
    * [err](#err)
    * [log](#log)
    * [string](#string)
    * [string view](#string_view)
    * [vector](#vector)
    * [list](#list)
    * [fs](#fs)
    * [thread](#thread)
    * [thread pool](#thread_pool)
  * [net](#net)
    * [socket](#socket)
    * [talk](#talk)
    * [async](#async)

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

`ugalib` aims to provide higher level utilities similar to ones found in the c++ stl. Just don't forget to call the destructors (I really miss RAII).  
ugalib networking closely follows [Beej's Guide to Network Programming](https://beej.us/guide/bgnet) and additionally provides centralized error handling and other luxuries like configurable logging and multithreading.  

## <a name = "core"></a> core<br/>

### <a name = "types"></a> types<br/>

the library provides some basic types (eg. sized integers) and wrappers for low level sockets API structs (eg. `uga_addrinfo`). 

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
In the above example, calling `uga_print_error()` after `uga_str_at()` would print:
```bash
:: 20:21:31:861 : 281473276694560 : UGA::ERR  : mem error: block size is 6, bad access at 10
```
### <a name = "log"></a> log<br/>

Before using `ugalib`'s logging facilities, it's recommended to run `uga_log_init()` which checks the underlying terminal and enables colors if available.  
Several log levels are provided by the library, but can also be extended by the user. Keep in mind that any custom log level has to have a lower priority than the provided ones (`log level > UGA_LVL_DBG`).  
Compile with `-DUGA_LOG_LVL=level` to set a custom log level for the library. Otherwise the default is used (`UGA_LOG_LVL_DBG`).

```c
i32_t some_val = 0 ;

UGA_DBG_S (     "worker", "value is %d", ++some_val ) ;
UGA_INFO_S(     "worker", "value is %d", ++some_val ) ;
UGA_WARN_S(     "worker", "value is %d", ++some_val ) ;
UGA_ERR_S (     "worker", "value is %d", ++some_val ) ;
UGA_LOG_S ( 10, "worker", "value is %d", ++some_val ) ;
```
produces:
```bash
:: 19:16:47:939 : 281473251115040 : UGA::DBG  : worker : value is 1
:: 19:16:47:940 : 281473251115040 : UGA::INFO : worker : value is 2
:: 19:16:47:940 : 281473251115040 : UGA::WARN : worker : value is 3
:: 19:16:47:940 : 281473251115040 : UGA::ERR  : worker : value is 4
:: 19:16:47:940 : 281473251115040 : UGA::0010 : worker : value is 5
   ^-|current     ^-|thread         ^-|log      ^-|scope
     |time          |id               |level
```

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
for( i32_t i = 0; i < vec.size; ++i )
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

### <a name = "fs"></a> fs<br/>

`uga_fs.h` provides some basic filesystem utilities for checking filesizes, reading from and writing to files.

```c
uga_string_view filename = uga_sv_create_1( "somefile.txt" ) ;

uga_string data = uga_fs_read_file( filename ) ;

// do stuff to contents

uga_fs_write_file( filename, uga_sv_create_from( &data ) ) ;
```

### <a name = "thread"></a> thread<br/>

`uga_thread.h` provides low level wrappers for `thread.h` utilities.  
Use `uga_task`s to define the job and arguments passed to the thread.  
During the call to `uga_thread_do_task()`, the newly created thread will make a deep copy of the task's data string
so each thread gets their own copy of the data.  
This copied string will be referenced in the returned `uga_thread`'s task field. This can be used as as in-out parameter to the thread
forcing the caller to clean up after the threads or as an in parameter in which case the thread destroys its copy once it's done using it.

```c
i32_t work ( void * arg )
{
    uga_string * data = ( uga_string * ) arg ;
    ...do some work...
    uga_str_destroy( data ) ; // if we don't need to pass any results back to the caller thread
    return 0 ;
}

int main ( void )
{
    ...
    uga_string data = get_data() ;
    uga_task   task = { data, work } ;

    uga_thread worker = uga_thread_do_task( task ) ;
    uga_str_destroy( &data ) ;
    ...
    uga_thread_join( &worker ) ;
    ...
    uga_str_destroy( &worker.task.data ) ; // if the thread needed to return data to the caller
    ...
}
```

For mutual exclusion you can either use `thread.h`'s `mtx` and `cnd` through `ugalib`'s wrappers, or `ugalib`'s counting semaphore.  
The following example has the main thread waiting for data from a third party and pushing all of it into a vector (global here for simplicity, could also be passed to the thread via the data string).  
Once new data is available, the main thread locks the `data_mtx` mutex, pushes all the new data into the vector and calls `uga_sem_release_n( sem, n )`. This call increases the semaphore's internal counter by `n`
allowing up to `n` threads to be woken up and try using the data.

```c
uga_vector global_data ;
uga_mtx_t     data_mtx ;
uga_semaphore data_sem ;

i32_t work ( void * arg )
{
    while( true )
    {
        uga_sem_acquire( &data_sem ) ;
        uga_mtx_acquire( &data_mtx ) ;
        ...do stuff with the data vector...
        uga_mtx_release( &data_mtx ) ;
        ...do stuff with the data...
    }
}

int main ( void )
{
    uga_mtx_init( &data_mtx ) ;
    uga_sem_init( &data_sem ) ;

    uga_task   task   = { uga_str_create_0(), work } ;
    uga_thread worker = uga_thread_do_task( task ) ;

    while( true )
    {
        if( new_data_available() )
        {
            uga_mtx_acquire( &data_mtx ) ;
            push_new_data( &global_data ) ;
            uga_sem_release_n( &data_sem, global_data.size ) ;
            uga_mtx_release( &data_mtx ) ;
        }
    }
    uga_sem_destroy( &data_sem ) ;
    uga_mtx_destroy( &data_mtx ) ;
}
```

### <a name = "thread_pool"></a> thread pool<br/>

`ugalib` also provides a simple thread pool implementation with FIFO task scheduling.

```c

uga_task task_1 = { data_1, work_1 } ;
uga_task task_2 = { data_2, work_2 } ;
uga_task task_3 = { data_3, work_3 } ;
uga_task task_4 = { data_4, work_4 } ;

uga_thread_pool * pool = uga_pool_create_new( NUM_THREADS ) ;

uga_pool_add_task( pool, task_1 ) ;
uga_pool_add_task( pool, task_2 ) ;
uga_pool_add_task( pool, task_3 ) ;
uga_pool_add_task( pool, task_4 ) ;

uga_pool_destroy( pool ) ;
```

## <a name = "net"></a> net<br/>

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

### <a name = "async"></a> async<br/>

(this will be overhauled soon)

In situations where you need to monitor multiple sockets for incoming data, `uga_async.h` provides some basic solutions.  
The following example creates two sockets, one TCP and the other UDP, both waiting for incoming connections on their own ports.  
We also "handcraft" a socket for `stdin`.  


```c
bool tcp_callback ( uga_callback * callback_data ) ;
bool udp_callback ( uga_callback * callback_data ) ;
bool std_callback ( uga_callback * callback_data ) ;

int main ( void )
{
    ...
    uga_string shared_data ;

    uga_socket tcp_sock = uga_sock_create_and_listen( tcp_port, backlog ) ;
    uga_socket udp_sock = uga_sock_create_and_bind( localhost, udp_port ) ;
    uga_socket std_sock = { 0 } ;
    std_sock.sockfd = fileno( stdin ) ;

    uga_callback tcp_cb = { &tcp_sock, &shared_data, tcp_callback } ;
    uga_callback udp_cb = { &udp_sock, &shared_data, udp_callback } ;
    uga_callback std_cb = { &std_sock, &shared_data, std_callback } ;

    uga_handler_list handlers = { 0 } ;

    uga_async_add_handler( &handlers, &tcp_cb ) ;
    uga_async_add_handler( &handlers, &udp_cb ) ;
    uga_async_add_handler( &handlers, &std_cb ) ;

    return uga_async_run( &handlers ) ;
}
```
After calling `uga_asnyc_run()`, the calling thread will block until a socket has data ready to be read. Once a socket becomes ready, the corresponding callback function is called and the wait continues.  
`uga_async_run()` only returns if a callback returns `false`.  

You can also periodically check if there is any data available by calling `uga_async_has_ready( &handlers )` which will return the ready socket or an empty socket (with `sockfd == -1`) if no sockets are ready.
