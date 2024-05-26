//
//
//      uganet
//      tftp::client.c
//

#include <core/uga_log.h>
#include <core/uga_err.h>
#include <core/uga_str.h>
#include <core/uga_strview.h>
#include <core/uga_vector.h>
#include <core/uga_fs.h>

#include <net/uga_sock.h>
#include <net/uga_talk.h>
#include <net/uga_async.h>
#include <net/uga_ftp.h>


void print_help ( i32_t argc, char ** argv ) ;

void configure_uga ( void ) ;

bool handle_cli_command ( uga_string_view command ) ;
void handle_net_message ( uga_socket    *  socket ) ;

uga_vector socks = { 0 } ;

i32_t main ( i32_t argc, char ** argv )
{
        configure_uga() ;
        if( argc > 1 )
        {
                print_help( argc, argv ) ;
                return 1 ;
        }
        UGA_S_INFO_S( "tftp::client", "started.\n" ) ;
        UGA_S_INFO_S( "tftp::client", "type help for usage info.\n" ) ;

        uga_socket stdsock = { false, fileno( stdin ), { 0 } } ;

        socks = uga_vec_create_d_1( uga_socket, 2, uga_sock_close_void ) ;
        uga_vec_push_back( &socks, &stdsock ) ;

        bool should_run = true ;

        while( should_run )
        {
                UGA_S_INFO_S( "tftp::client", "" ) ;

                uga_socket to_read = uga_async_next( socks ) ;

                if( to_read.sockfd == stdsock.sockfd )
                {
                        char command_buffer[ 128 ] = { 0 } ;
                        uga_string command = { command_buffer, 0, 128 } ;

                        uga_fs_read_fd( stdsock.sockfd, &command ) ;

                        should_run = handle_cli_command( uga_sv_create_from( &command ) ) ;
                }
                else
                {
                        handle_net_message( &to_read ) ;
                }
        }
        uga_vec_destroy( &socks ) ;

        UGA_S_INFO_S( "tftp::client", "finished.\n" ) ;

        return 0 ;
}

void handle_incoming_ack  ( uga_tftp_ack_packet  * packet ) ;
void handle_incoming_data ( uga_tftp_data_packet * packet ) ;
void handle_incoming_err  ( uga_tftp_err_packet  * packet ) ;

void handle_net_message ( uga_socket * socket )
{
        uga_string message = uga_recv_str_from( socket, 0 ) ;

        uga_tftp_packet packet = uga_tftp_parse_packet( uga_sv_create_from( &message ) ) ;

        switch( packet.opcode )
        {
                case UGA_TFTP_OP_DATA:
                        handle_incoming_data( ( uga_tftp_data_packet * ) &packet ) ;
                        break ;
                case UGA_TFTP_OP_ACK:
                        handle_incoming_ack( ( uga_tftp_ack_packet * ) &packet ) ;
                        break ;
                case UGA_TFTP_OP_ERR:
                        handle_incoming_err( ( uga_tftp_err_packet * ) &packet ) ;
                        break ;
                default:
                        UGA_S_WARN_S( "tftp::client", "incoming message opcode is %d\n", packet.opcode ) ;
                        break ;
        }
        uga_tftp_packet_destroy( &packet ) ;
        uga_str_destroy( &message ) ;
}

void handle_incoming_ack ( uga_tftp_ack_packet * packet )
{
        UGA_S_INFO_S( "tftp::client", "received ack for packet %d\n", packet->blockno ) ;
}
void handle_incoming_data ( uga_tftp_data_packet * packet )
{
        UGA_S_INFO_S( "tftp::client", "received data packet, data:\n"STR_FMT"\n", STR_ARG( packet->data ) ) ;
}
void handle_incoming_err ( uga_tftp_err_packet * packet )
{
        UGA_S_INFO_S( "tftp::client", "received error packet with message: "STR_FMT"\n", STR_ARG( packet->errmsg ) ) ;
}

void do_connect_command ( uga_string_view args ) ;
void do_send_command    ( uga_string_view args ) ;
void do_get_command     ( uga_string_view args ) ;

bool handle_cli_command ( uga_string_view command )
{
        static uga_string_view const cmd_connect = { "connect", 7 } ;
        static uga_string_view const cmd_send    = {    "send", 4 } ;
        static uga_string_view const cmd_get     = {     "get", 3 } ;
        static uga_string_view const cmd_quit    = {    "quit", 4 } ;

        uga_string_view cmd = uga_sv_chop_to_delimiter( &command, ' ', true ) ;

        if( uga_sv_starts_with_insensitive( cmd, cmd_connect ) )
        {
                do_connect_command( command ) ;
        }
        else if( uga_sv_starts_with_insensitive( cmd, cmd_send ) )
        {
                do_send_command( command ) ;
        }
        else if( uga_sv_starts_with_insensitive( cmd, cmd_get ) )
        {
                do_get_command( command ) ;
        }
        else if( uga_sv_starts_with_insensitive( cmd, cmd_quit ) )
        {
                return false ;
        }
        else
        {
                UGA_S_ERR_S( "tftp::client", "bad command, type help for usage info.\n" ) ;
        }
        return true ;
}

void do_connect_command ( uga_string_view args )
{
        uga_string_view host = uga_sv_chop_to_delimiter( &args,  ' ', true ) ;
        uga_string_view port = uga_sv_chop_to_delimiter( &args, '\n', true ) ;

        uga_string hostcstr = uga_str_create_cstr( host ) ;
        uga_string portcstr = uga_str_create_cstr( port ) ;

        host = uga_sv_create_from( &hostcstr ) ;
        port = uga_sv_create_from( &portcstr ) ;

        if( host.size == 0 || port.size == 0 )
        {
                UGA_S_ERR_S(" tftp::client", "bad args for connect command.\n" ) ;
                UGA_S_ERR_S(" tftp::client", "type help for usage info.\n" ) ;
                return ;
        }
        UGA_S_DBG_S( "tftp::client", "connecting to "SV_FMT":"SV_FMT"\n", SV_ARG( host ), SV_ARG( port ) ) ;
        uga_socket server = uga_sock_create_and_connect( host, port ) ;
        UGA_S_DBG_S( "tftp::client", "connected.\n" ) ;

        uga_vec_push_back( &socks, &server ) ;

        uga_str_destroy( &hostcstr ) ;
        uga_str_destroy( &portcstr ) ;
}

void do_send_ack  ( uga_string_view args ) ;
void do_send_rq   ( uga_string_view args ) ;
void do_send_data ( uga_string_view args ) ;
void do_send_err  ( uga_string_view args ) ;

void do_send_command ( uga_string_view args )
{
        static uga_string_view const packet_ack  = {  "ack", 3 } ;
        static uga_string_view const packet_rrq  = {  "rrq", 3 } ;
        static uga_string_view const packet_wrq  = {  "wrq", 3 } ;
        static uga_string_view const packet_data = { "data", 4 } ;
        static uga_string_view const packet_err  = {  "err", 3 } ;

        uga_socket server = *( uga_socket * ) uga_vec_at( &socks, 1 ) ;

        if( server.sockfd == -1 )
        {
                UGA_S_ERR_S( "tftp::client", "cannot send before connecting to a server\n" ) ;
                return ;
        }
        uga_string_view packet_type = uga_sv_chop_to_delimiter( &args, ' ', true ) ;

        if( packet_type.size == 0 || args.size == 0 )
        {
                UGA_S_ERR_S( "tftp::client", "bad args for send command.\n" ) ;
                UGA_S_ERR_S( "tftp::client", "type help for usage info.\n" ) ;
                return ;
        }
        if( uga_sv_equal_insensitive( packet_type, packet_ack ) )
        {
                do_send_ack( args ) ;
        }
        else if( uga_sv_equal_insensitive( packet_type, packet_rrq ) )
        {
                uga_sv_unchop_left( &args, 4 ) ;
                do_send_rq( args ) ;
        }
        else if( uga_sv_equal_insensitive( packet_type, packet_wrq ) )
        {
                uga_sv_unchop_left( &args, 4 ) ;
                do_send_rq( args ) ;
        }
        else if( uga_sv_equal_insensitive( packet_type, packet_data ) )
        {
                do_send_data( args ) ;
        }
        else if( uga_sv_equal_insensitive( packet_type, packet_err ) )
        {
                do_send_err( args ) ;
        }
        else
        {
                UGA_S_ERR_S( "tftp::client", "bad args for send command.\n" ) ;
                UGA_S_ERR_S( "tftp::client", "type help for usage info.\n" ) ;
        }
}

void do_get_command ( uga_string_view args )
{
        {
                uga_string_view filename = uga_sv_chop_to_delimiter( &args, '\n', true ) ;
                uga_string filenamecstr = uga_str_create_cstr( filename ) ;
                filename = uga_sv_create_from( &filenamecstr ) ;

                uga_tftp_packet_raw rrqpacket = uga_tftp_craft_rq_packet( UGA_TFTP_OP_RRQ, filename, UGA_TFTP_MODE_OCTET ) ;
                uga_tftp_send_packet( uga_vec_at( &socks, 1 ), &rrqpacket ) ;

                uga_str_destroy( &filenamecstr ) ;
        }
        UGA_S_INFO_S( "tftp::client", "request sent, waiting for file...\n" ) ;

        bool done = false ;
        while( !done )
        {
                uga_string message = uga_recv_str( uga_vec_at( &socks, 1 ), 0 ) ;
                uga_tftp_packet response = uga_tftp_parse_packet( uga_sv_create_from( &message ) ) ;

                if( response.opcode != UGA_TFTP_OP_DATA )
                {
                        UGA_S_ERR_S( "tftp::client", "unexpected server response\n" ) ;
                        uga_tftp_packet_destroy( &response ) ;
                        uga_str_destroy( &message ) ;
                }
                uga_tftp_data_packet * datapacket = ( uga_tftp_data_packet * ) &response ;

                if( datapacket->data.size < 512 ) done = true ;
                printf( SV_FMT, SV_ARG( datapacket->data ) ) ;

                uga_tftp_packet_raw ackpacket = uga_tftp_craft_ack_packet( datapacket->blockno ) ;
                uga_tftp_send_packet( uga_vec_at( &socks, 1 ), &ackpacket ) ;

                uga_tftp_packet_destroy( &response ) ;
                uga_str_destroy( &message ) ;
        }
        UGA_S_INFO_S( "tftp::client", "file transfer complete.\n" ) ;
}

void do_send_ack ( uga_string_view args )
{
        uga_string_view blockno = uga_sv_chop_to_delimiter( &args, '\n', true ) ;

        if( blockno.size == 0 )
        {
                UGA_S_ERR_S( "tftp::client", "bad args for send ack command.\n" ) ;
                UGA_S_ERR_S( "tftp::client", "type help for usage info.\n" ) ;
                return ;
        }
        i32_t blocknum = uga_sv_parse_int( blockno ) ;

        uga_tftp_packet_raw packet = uga_tftp_craft_ack_packet( blocknum ) ;

        uga_tftp_send_packet( uga_vec_at( &socks, 1 ), &packet ) ;
}
void do_send_rq  ( uga_string_view args )
{
        static uga_string_view const op_rrq = {      "rrq", 3 } ;
        static uga_string_view const m_net  = { "netascii", 8 } ;
        static uga_string_view const m_oct  = {    "octet", 5 } ;
        static uga_string_view const m_mail = {     "mail", 4 } ;

        uga_string_view   opcode = uga_sv_chop_to_delimiter( &args,  ' ', true ) ;
        uga_string_view filename = uga_sv_chop_to_delimiter( &args,  ' ', true ) ;
        uga_string_view     mode = uga_sv_chop_to_delimiter( &args, '\n', true ) ;

        if( opcode.size == 0 || filename.size == 0 || mode.size == 0 )
        {
                UGA_S_ERR_S( "tftp::client", "bad args for send rq command.\n" ) ;
                UGA_S_ERR_S( "tftp::client", "type help for usage info.\n" ) ;
                return ;
        }
        uga_tftp_opcode op = uga_sv_equal_insensitive( opcode, op_rrq ) ? UGA_TFTP_OP_RRQ : UGA_TFTP_OP_WRQ ;
        uga_tftp_mode m ;

        if     ( uga_sv_equal_insensitive( mode, m_net  ) ) m = UGA_TFTP_MODE_NETASCII ;
        else if( uga_sv_equal_insensitive( mode, m_oct  ) ) m = UGA_TFTP_MODE_OCTET    ;
        else if( uga_sv_equal_insensitive( mode, m_mail ) ) m = UGA_TFTP_MODE_MAIL     ;
        else                                                m = UGA_TFTP_MODE_BAD      ;

        uga_tftp_packet_raw packet = uga_tftp_craft_rq_packet( op, filename, m ) ;

        uga_tftp_send_packet( uga_vec_at( &socks, 1 ), &packet ) ;
}
void do_send_data ( uga_string_view args )
{
        ( void ) args ;
        UGA_S_ERR_S( "tftp::client", "not implemented\n" ) ;
}
void do_send_err ( uga_string_view args )
{
        ( void ) args ;
        UGA_S_ERR_S( "tftp::client", "not implemented\n" ) ;
}

void configure_uga ( void )
{
        uga_log_init() ;

        uga_sock_conf config = uga_sock_get_config() ;

        config.family = UGA_SFAM_IPv4  ;
        config.  type = UGA_SOCK_DGRAM ;
        config. flags =              0 ;

        uga_sock_set_config( config ) ;
}

void print_help ( i32_t argc, char ** argv )
{
        UGA_S_ERR_S( "tftp::client", "%d excess arguments provided!\n", argc - 1 ) ;
        UGA_S_ERR_S( "tftp::client", "usage: %s\n", argv[ 0 ] ) ;
}
