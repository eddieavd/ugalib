//
//
//      uganet
//      server.c
//

#include <uganet.h>

#include <stdlib.h>


void configure_uga ( void ) ;

int main ( int argc, char ** argv )
{
        configure_uga() ;

        if( argc < 2 )
        {
                UGA_ERR_S( "uga::demo::server", "usage: ./server <port>" ) ;
                return 1 ;
        }
        char const * port = argv[ 1 ] ;

        uga_socket sock = uga_sock_create_and_listen( port, 12 ) ;
        uga_print_abort_if_err() ;
        uga_sock_reuse( sock ) ;
        UGA_INFO_S( "uga::demo::server", "socket created, listening..." ) ;

        uga_sock_log_s( "uga::demo::server", sock ) ;

        uga_socket client = uga_sock_accept( sock ) ;
        uga_print_abort_if_err() ;

        UGA_INFO_S( "uga::demo::server", "accepted connection." ) ;

        uga_sock_log_s( "uga::demo::server", client ) ;

        char data [ 64 ] = { 0 } ;
        i64_t len = 64   ;

        UGA_INFO_S( "uga::demo::server", "waiting for message..." );

        uga_recv( &client, data, len, 0 );

        if( uga_had_errs() && uga_current_errtype() != UGA_ERR_PARTIAL_READ )
        {
                uga_print_error() ;
                exit( 1 ) ;
        }
        else if( uga_had_errs() ) uga_log_error( UGA_LVL_WARN ) ;

        UGA_INFO_S( "uga::demo::server", "message received." );
        UGA_INFO_S( "uga::demo::server", "message: %s", data );


        return 0;
}

void configure_uga ( void )
{
        uga_log_init() ;

        uga_sock_conf config = uga_sock_get_config();

        config.flags  = 0 ;
        config.family = UGA_SFAM_IPv6 ;
        config.  type = UGA_SOCK_STREAM ;

        uga_sock_set_config( config );
}
