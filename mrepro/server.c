//
//
//      mrepro
//      server.c
//

#include <uganet.h>
#include <net/uga_async.h>
#include <net/uga_mplex.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CLI_TCP 't'
#define CLI_UDP 'u'
#define CLI_PLD 'p'

#define CLI_TCP_DEFAULT "1234"
#define CLI_UDP_DEFAULT "1234"
#define CLI_PLD_DEFAULT     ""

#define CMD_PRINT "PRINT"
#define CMD_SET     "SET"
#define CMD_QUIT   "QUIT"


static uga_sock_conf const tcp_config = { UGA_SFAM_UNSPEC, UGA_SOCK_STREAM, UGA_PASSIVE } ;
static uga_sock_conf const udp_config = { UGA_SFAM_UNSPEC, UGA_SOCK_DGRAM , UGA_PASSIVE } ;

static uga_string_view const hello_msg = { "HELLO\n", strlen( "HELLO\n" ) } ;

void print_help ( void ) ;

uga_cli_args prepare_args ( void ) ;

bool tcp_callback ( uga_callback * callback_data ) ;
bool udp_callback ( uga_callback * callback_data ) ;
bool std_callback ( uga_callback * callback_data ) ;

int main ( int argc, char ** argv )
{
        uga_log_init() ;

        uga_cli_args args = prepare_args() ;
        uga_cli_parse_args( argc, argv, &args ) ;
        if( uga_had_errs() )
        {
                print_help() ;
                return 1 ;
        }
        char const * tcp_port = uga_cli_get_val( CLI_TCP, &args ) ;
        char const * udp_port = uga_cli_get_val( CLI_UDP, &args ) ;

        uga_string payloads = uga_str_create_from_1( uga_cli_get_val( CLI_PLD, &args ) ) ;
        uga_print_abort_if_err() ;

        UGA_INFO_S( "lab2::server", "config: tcp port : %s, udp port : %s, payloads : "STR_FMT, tcp_port, udp_port, STR_ARG( payloads ) ) ;

        uga_socket tcp_sock = uga_sock_create_and_listen_configured( tcp_port, 16, &tcp_config ) ;
        uga_print_abort_if_err() ;
        uga_socket udp_sock = uga_sock_create_and_bind_configured( "127.0.0.1", udp_port, &udp_config ) ;
        uga_print_abort_if_err() ;
        uga_socket std_sock = { 0 } ;
        std_sock.sockfd = fileno( stdin ) ;

        uga_sock_reuse( tcp_sock ) ;
        uga_sock_reuse( udp_sock ) ;

        UGA_INFO_S( "lab2::server", "created sockets" ) ;

        uga_callback tcp_cb_data = { &tcp_sock, &payloads, tcp_callback } ;
        uga_callback udp_cb_data = { &udp_sock, &payloads, udp_callback } ;
        uga_callback std_cb_data = { &std_sock, &payloads, std_callback } ;

        uga_handler_list handlers = { 0 } ;

        uga_mplex_add_handler( &handlers, &tcp_cb_data ) ;
        uga_mplex_add_handler( &handlers, &udp_cb_data ) ;
        uga_mplex_add_handler( &handlers, &std_cb_data ) ;

        UGA_INFO_S( "lab2::server", "added callbacks to multiplexer" ) ;
        UGA_INFO_S( "lab2::server", "starting multiplexer..." ) ;

        i32_t ret = uga_mplex_run( &handlers ) ;


        return ret ;
}

bool tcp_callback ( uga_callback * callback_data )
{
        UGA_INFO_S( "lab2::server::tcp_callback", "handling tcp connection" ) ;

        uga_socket *     sock = callback_data->sock ;
        uga_string * payloads = callback_data->data ;

        uga_socket client = uga_sock_accept( *sock ) ;

        uga_string message = uga_recv_str( &client, 0 ) ;
        UGA_RETURN_ON_ERR( true ) ;

        if( uga_sv_equal( hello_msg, uga_sv_create_from( &message ) ) )
        {
                UGA_INFO_S( "lab2::server::tcp_callback", "sent payloads" ) ;
                uga_send_str( &client, payloads, 0 ) ;
        }
        uga_sock_shutdown( &client, SHUT_RDWR ) ;
        uga_sock_close( &client ) ;
        uga_str_destroy( &message ) ;
        return true ;
}

bool udp_callback ( uga_callback * callback_data )
{
        UGA_INFO_S( "lab2::server::udp_callback", "handling udp connection" ) ;

        uga_socket *     sock = callback_data->sock ;
        uga_string * payloads = callback_data->data ;

        uga_string message = uga_recv_str_from( sock, 0 ) ;
        UGA_RETURN_ON_ERR( true ) ;

        if( uga_sv_equal( hello_msg, uga_sv_create_from( &message ) ) )
        {
                UGA_INFO_S( "lab2::server::udp_callback", "sent payloads" ) ;
                uga_send_str_to( sock, payloads, 0 ) ;
        }
        uga_str_destroy( &message ) ;
        return true ;
}

bool std_callback ( uga_callback * callback_data )
{
        UGA_INFO_S( "lab2::server::std_callback", "handling stdin input" ) ;

        uga_socket *     sock = callback_data->sock ;
        uga_string * payloads = callback_data->data ;

        uga_string command = uga_str_create_1( 1024 ) ;

//      uga_fs_read_fd_into( sock->sockfd, ( u8_t * ) command.data, command.capacity ) ;
        i32_t bread = read( sock->sockfd, command.data, command.capacity ) ;
        command.size = bread ;

        uga_string_view command_view = uga_sv_create_from( &command ) ;

        if( uga_sv_starts_with( command_view, uga_sv_create_1( CMD_PRINT ) ) )
        {
                UGA_INFO_S( "lab2::server::std_callback", "current payload: "STR_FMT, STR_ARG( *payloads ) ) ;
        }
        else if( uga_sv_starts_with( command_view, uga_sv_create_1( CMD_SET ) ) )
        {
                uga_sv_chop_to_delimiter( &command_view, ' ', true ) ;

                uga_str_clear( payloads ) ;
                uga_str_reserve( payloads, command_view.size ) ;

                uga_str_append( payloads, command_view.data, command_view.size ) ;
                UGA_INFO_S( "lab2::server::std_callback", "new payloads: "STR_FMT, STR_ARG( *payloads ) ) ;
        }
        else if( uga_sv_starts_with( command_view, uga_sv_create_1( CMD_QUIT ) ) )
        {
                UGA_INFO_S( "lab2::server::std_callback", "quitting..." ) ;
                uga_str_destroy( &command ) ;
                return false ;
        }
        else
        {
                UGA_INFO_S( "lab2::server::std_callback", "unknown command, ignoring" ) ;
        }

        uga_str_destroy( &command ) ;
        return true ;
}

void print_help ( void )
{
        UGA_ERR_S( "lab2::server", "usage: ./server [ -t tcp_port ] [ -u udp_port ] [ -p payloads ]" ) ;
}

uga_cli_args prepare_args ( void )
{
        uga_cli_args args = { 0 } ;

        args.arg_count = 3 ;

        args.args[ 0 ].     name = CLI_TCP ;
        args.args[ 0 ].has_value =    true ;

        args.args[ 1 ].     name = CLI_UDP ;
        args.args[ 1 ].has_value =    true ;

        args.args[ 2 ].     name = CLI_PLD ;
        args.args[ 2 ].has_value =    true ;

        memcpy( args.args[ 0 ].value, CLI_TCP_DEFAULT, strlen( CLI_TCP_DEFAULT ) ) ;
        memcpy( args.args[ 1 ].value, CLI_UDP_DEFAULT, strlen( CLI_UDP_DEFAULT ) ) ;
        memcpy( args.args[ 2 ].value, CLI_PLD_DEFAULT, strlen( CLI_PLD_DEFAULT ) ) ;

        return args ;
}
