//
//
//      uganet
//      test_talk.h
//

#ifndef UGA_TEST_TALK_H_
#define UGA_TEST_TALK_H_

#include <uganet.h>
#include <test.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


static inline void test_client_1 ( void )
{
        uga_socket sock = uga_sock_create_and_listen( "8372", 10 ) ;
        uga_print_abort_if_err() ;
        UGA_INFO_S( "test::talk::client_1", "socket created and listening..." ) ;

        uga_sock_reuse( sock ) ;

        uga_socket client = uga_sock_accept( sock ) ;
        uga_print_abort_if_err() ;
        UGA_INFO_S( "test::talk::client_1", "accepted connection from client." ) ;

        char data[ 256 ] = { 0 } ;

        uga_recv( &client, data, 256, 0 ) ;
        UGA_INFO_S( "test::talk::client_1", "received message from client: %s", data ) ;

        test_log( "test::talk", strncmp( data, "hello", 5 ) == 0 ) ;
}

static inline void test_client_2 ( void )
{
        sleep( 1 ) ;

        uga_socket sock = uga_sock_create_and_connect( "127.0.0.1", "8372" ) ;
        uga_print_abort_if_err() ;
        UGA_INFO_S( "test::talk::client_2", "socket created and connected." ) ;

        uga_send( &sock, "hello", 5, 0 ) ;
        uga_print_abort_if_err() ;
        UGA_INFO_S( "test::talk::client_2", "message sent." ) ;
}

static inline void test_talk ( void )
{
        if( fork() == 0 )
        {
                test_client_1() ;
                exit( 0 ) ;
        }
        if( fork() == 0 )
        {
                test_client_2() ;
                exit( 0 ) ;
        }
        wait( NULL ) ;
        wait( NULL ) ;
}


#endif // UGA_TEST_TALK_H_
