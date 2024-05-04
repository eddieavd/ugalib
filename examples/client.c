//
//
//      uganet
//      client.c
//

#define UGA_NUM_CLI_ARGS 2

#include <uganet.h>

#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define CLI_TOTAL_ARGS UGA_NUM_CLI_ARGS * 2 + 1

#define CLI_PORT 'p'
#define CLI_MSG  'm'


uga_cli_args prepare_args ( void ) ;
void configure_uga ( void ) ;

int main ( int argc, char ** argv )
{
        configure_uga() ;

        if( argc < CLI_TOTAL_ARGS )
        {
                UGA_ERR_S( "uga::demo::client", "usage: ./client [ -p port ] [ -m message ]\n" ) ;
                return 1 ;
        }
        uga_cli_args args = prepare_args() ;
        uga_cli_parse_args( argc, argv, &args ) ;

        char const * port = uga_cli_get_val( CLI_PORT, &args ) ;
        char const * msg  = uga_cli_get_val( CLI_MSG , &args ) ;

        UGA_INFO_S( "uga::demo::client", "creating and connecting socket..." ) ;

        uga_socket sock = uga_sock_create_and_connect( NULL, port ) ;
        uga_print_abort_if_err() ;

        UGA_INFO_S( "uga::demo::client", "connection established." );
        uga_sock_log_s( "uga::demo::client", sock ) ;

        i64_t const datalen = strlen( msg );

        UGA_INFO_S( "uga::demo::client", "sending message..." );

        uga_send( &sock, msg, datalen, 0 );
        uga_print_abort_if_err();

        UGA_INFO_S( "uga::demo::client", "message sent." );


        return 0;
}

uga_cli_args prepare_args ( void )
{
        uga_cli_args args = { 0 } ;

        args.arg_count = UGA_NUM_CLI_ARGS;

        args.args[ 0 ].     name = CLI_PORT ;
        args.args[ 0 ].has_value =     true ;

        args.args[ 1 ].     name = CLI_MSG ;
        args.args[ 1 ].has_value =    true ;

        return args;
}

void configure_uga ( void )
{
        uga_log_init() ;

        uga_sock_conf config = uga_sock_get_config();

        config. flags = 0 ;
        config.family = UGA_SFAM_IPv6 ;
        config.  type = UGA_SOCK_STREAM ;

        uga_sock_set_config( config );
}
