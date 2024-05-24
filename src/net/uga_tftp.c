//
//
//      uganet
//      uga_tftp.c
//

#include <net/uga_tftp.h>
#include <core/uga_log.h>
#include <core/uga_err.h>
#include <core/uga_fs.h>
#include <core/uga_strview.h>
#include <core/uga_pool.h>
#include <net/uga_sock.h>
#include <net/uga_talk.h>

#include <string.h>
#include <unistd.h>


static uga_sock_conf const tftp_sock_config = { UGA_SFAM_IPv4, UGA_SOCK_DGRAM, UGA_PASSIVE } ;

void _uga_tftp_parse_rq_packet   ( uga_string_view raw_packet, uga_tftp_rq_packet   * packet ) ;
void _uga_tftp_parse_data_packet ( uga_string_view raw_packet, uga_tftp_data_packet * packet ) ;
void _uga_tftp_parse_ack_packet  ( uga_string_view raw_packet, uga_tftp_ack_packet  * packet ) ;
void _uga_tftp_parse_err_packet  ( uga_string_view raw_packet, uga_tftp_err_packet  * packet ) ;

uga_tftp_packet uga_tftp_parse_packet ( uga_string_view raw_packet )
{
        uga_tftp_packet packet = { 0 } ;

        u16_t opcode = uga_ntohs( *( u16_t * ) raw_packet.data ) ;

        switch( opcode )
        {
                case UGA_TFTP_OP_RRQ:
                        _uga_tftp_parse_rq_packet( raw_packet, ( uga_tftp_rq_packet * ) &packet ) ;
                        break ;
                case UGA_TFTP_OP_WRQ:
                        _uga_tftp_parse_rq_packet( raw_packet, ( uga_tftp_rq_packet * ) &packet ) ;
                        break ;
                case UGA_TFTP_OP_DATA:
                        _uga_tftp_parse_data_packet( raw_packet, ( uga_tftp_data_packet * ) &packet ) ;
                        break ;
                case UGA_TFTP_OP_ACK:
                        _uga_tftp_parse_ack_packet( raw_packet, ( uga_tftp_ack_packet * ) &packet ) ;
                        break ;
                case UGA_TFTP_OP_ERR:
                        _uga_tftp_parse_err_packet( raw_packet, ( uga_tftp_err_packet * ) &packet ) ;
                        break ;
                default:
                        packet.opcode = UGA_TFTP_OP_BAD ;
                        break ;
        }
        return packet ;
}

void _uga_tftp_parse_rq_packet ( uga_string_view raw_packet, uga_tftp_rq_packet * packet )
{
        static uga_string_view const netascii_str = { "netascii", 8 } ;
        static uga_string_view const    octet_str = {    "octet", 5 } ;
        static uga_string_view const     mail_str = {     "mail", 4 } ;

//      packet-> opcode = uga_ntohs( *( u16_t * ) raw_packet.data ) ;
        packet->opcode = uga_ntohs( ( raw_packet.data[ 1 ] << 8 ) + raw_packet.data[ 0 ] ) ;
        uga_sv_chop_left( &raw_packet, 2 ) ;

        uga_string_view filename = uga_sv_chop_to_delimiter( &raw_packet, '\0', true ) ;
        uga_string_view     mode = uga_sv_chop_to_delimiter( &raw_packet, '\0', true ) ;

        packet->filename = uga_str_create_cstr( filename ) ;

        if     ( uga_sv_equal_insensitive( mode, netascii_str ) ) packet->mode = UGA_TFTP_MODE_NETASCII ;
        else if( uga_sv_equal_insensitive( mode,    octet_str ) ) packet->mode = UGA_TFTP_MODE_OCTET    ;
        else if( uga_sv_equal_insensitive( mode,     mail_str ) ) packet->mode = UGA_TFTP_MODE_MAIL     ;
        else                                                      packet->mode = UGA_TFTP_MODE_BAD      ;
}

void _uga_tftp_parse_data_packet ( uga_string_view raw_packet, uga_tftp_data_packet * packet )
{
//      packet-> opcode = uga_ntohs( *( u16_t * ) raw_packet.data     ) ;
//      packet->blockno = uga_ntohs( *( u16_t * ) raw_packet.data + 2 ) ;
        packet-> opcode = uga_ntohs( ( raw_packet.data[ 1 ] << 8 ) + raw_packet.data[ 0 ] ) ;
        packet->blockno = uga_ntohs( ( raw_packet.data[ 3 ] << 8 ) + raw_packet.data[ 2 ] ) ;
        uga_sv_chop_left( &raw_packet, 4 ) ;

        uga_string_view data = uga_sv_chop_to_delimiter( &raw_packet, '\0', true ) ;

        packet->data= uga_str_create_cstr( data ) ;
}

void _uga_tftp_parse_ack_packet ( uga_string_view raw_packet, uga_tftp_ack_packet * packet )
{
//      packet-> opcode = uga_ntohs( *( u16_t * ) raw_packet.data     ) ;
//      packet->blockno = uga_ntohs( *( u16_t * ) raw_packet.data + 2 ) ;
        packet-> opcode = uga_ntohs( ( raw_packet.data[ 1 ] << 8 ) + raw_packet.data[ 0 ] ) ;
        packet->blockno = uga_ntohs( ( raw_packet.data[ 3 ] << 8 ) + raw_packet.data[ 2 ] ) ;
}

void _uga_tftp_parse_err_packet ( uga_string_view raw_packet, uga_tftp_err_packet * packet )
{
//      packet-> opcode = uga_ntohs( *( u16_t * ) raw_packet.data     ) ;
//      packet->errcode = uga_ntohs( *( u16_t * ) raw_packet.data + 2 ) ;
        packet-> opcode = uga_ntohs( ( raw_packet.data[ 1 ] << 8 ) + raw_packet.data[ 0 ] ) ;
        packet->errcode = uga_ntohs( ( raw_packet.data[ 3 ] << 8 ) + raw_packet.data[ 2 ] ) ;
        uga_sv_chop_left( &raw_packet, 4 ) ;

        uga_string_view errmsg = uga_sv_chop_to_delimiter( &raw_packet, '\0', true ) ;

        packet->errmsg = uga_str_create_cstr( errmsg ) ;
}

uga_tftp_server uga_tftp_server_create ( uga_tftp_server_config config )
{
        static uga_string_view const localhost = { "127.0.0.1", 9 } ;

        uga_tftp_server server = { 0, config.mode, { 0 }, uga_dl_list_create( uga_tftp_inflight_request ) } ;

        server.sock = uga_sock_create_and_bind_configured( localhost, config.port, &tftp_sock_config ) ;

        return server ;
}

i32_t _uga_tftp_client_handler ( void * arg ) ;
i32_t _uga_tftp_server_run ( uga_tftp_server * server ) ;

i32_t uga_tftp_server_run ( uga_tftp_server * server )
{
        if( server->mode == UGA_TFTP_SRV_MAIN )
        {
                UGA_INFO_S( "uga::tftp::server::run", "starting server..." ) ;
                return _uga_tftp_server_run( server ) ;
        }
        else if( server->mode == UGA_TFTP_SRV_DAEMON )
        {
                UGA_INFO_S( "uga::tftp::server::run", "starting server as daemon..." ) ;
                if( !fork() ) return _uga_tftp_server_run( server ) ;
        }
        else
        {
                UGA_ERR_S( "uga::tftp::server:run", "tried starting server with bad mode" ) ;
                return 1 ;
        }
        UGA_INFO_S( "uga::tftp::server::run", "finished." ) ;
        return 0 ;
}

i32_t _uga_tftp_handle_rrq ( uga_tftp_packet * packet, uga_tftp_inflight_request * request )
{
        UGA_INFO_S( "uga::tftp::worker::rrq", "started." ) ;

        u16_t block_count = 0 ;

        if( packet->opcode == UGA_TFTP_OP_RRQ && uga_str_empty( request->filedata ) && request->last_ackd == -1 )
        {
                UGA_INFO_S( "uga::tftp::worker::rrq", "received new request." ) ;

                uga_tftp_rq_packet * rqpacket = ( uga_tftp_rq_packet * ) packet ;

                uga_string_view filename = uga_sv_create_from( &rqpacket->filename ) ;

                if( !uga_fs_file_readable( filename ) )
                {
                        UGA_ERR_S( "uga::tftp::worker::rrq", "requested file { "SV_FMT" } not readable!", SV_ARG( filename ) ) ;

                        uga_string errpacket = uga_tftp_craft_err_packet( UGA_TFTP_ERR_ACCESS_VIOLATION ) ;
                        uga_send_str_to( &request->client, uga_sv_create_from( &errpacket ), 0 ) ;
                        uga_str_destroy( &errpacket ) ;

                        return 1 ;
                }
                UGA_INFO_S( "uga::tftp::worker::rrq", "reading requested file { "SV_FMT" }...", SV_ARG( filename ) ) ;
                request->filedata = uga_fs_read_file( filename ) ;

                uga_dl_list_push_front( &request->server->reqs, request ) ;
                uga_str_destroy( &rqpacket->filename ) ;
        }
        else if( packet->opcode == UGA_TFTP_OP_ACK && !uga_str_empty( request->filedata ) )
        {
                UGA_INFO_S( "uga::tftp::worker::rrq", "received ack from existing client." ) ;

                uga_tftp_ack_packet * ackpacket = ( uga_tftp_ack_packet * ) packet ;
                if( request->last_ackd + 1 != ackpacket->blockno )
                {
                        UGA_ERR_S( "uga::tftp::worker::rrq", "wrong packet acknowledged, expected %d, got %d", request->last_ackd + 1, ackpacket->blockno ) ;
                        uga_string errpacket = uga_tftp_craft_err_packet( UGA_TFTP_ERR_ILLEGAL_OP ) ;
                        uga_send_str_to( &request->client, uga_sv_create_from( &errpacket ), 0 ) ;
                        uga_str_destroy( &errpacket ) ;

                        return 1 ;
                }
                UGA_INFO_S( "uga::tftp::worker::rrq", "ack fine, blockno is %d", ackpacket->blockno ) ;
                request->last_ackd = ackpacket->blockno ;
                block_count = request->last_ackd + 1 ;
        }
        else
        {
                UGA_ERR_S( "uga::tftp::worker::rrq", "internal server error, opcode is %d, filedata size is %d, last ackd block is %d", packet->opcode, request->filedata.size, request->last_ackd ) ;
                return 1 ;
        }
        i64_t offset = block_count * 512 ;

        uga_string_view contents = uga_sv_create_from( &request->filedata ) ;

        if( offset > contents.size )
        {
                UGA_INFO_S( "uga::tftp::worker::rrq", "file transfer finished." ) ;
                uga_dl_list_remove( &request->server->reqs, request ) ;
                return 0 ;
        }
        uga_sv_chop_left( &contents, offset ) ;

        UGA_INFO_S( "uga::tftp::worker::rrq", "sending block %d...", block_count ) ;
        uga_string datapacket = uga_tftp_craft_data_packet( block_count, contents ) ;
        uga_send_str_to( &request->client, uga_sv_create_from( &datapacket ), 0 ) ;
        uga_str_destroy( &datapacket ) ;
        UGA_INFO_S( "uga::tftp::worker::rrq", "block %d sent.", block_count ) ;

        return 0 ;
}

i32_t _uga_tftp_client_handler ( void * arg )
{
        UGA_INFO_S( "uga::tftp::worker", "started." ) ;

        uga_vector * data =  ( uga_vector * ) arg ;
        uga_tftp_inflight_request * request = *( uga_tftp_inflight_request ** ) uga_vec_at( data, 0 ) ;

        uga_tftp_packet packet = uga_tftp_parse_packet( uga_sv_create_from( &request->last_req ) ) ;
        UGA_INFO_S( "uga::tftp::worker", "parsed last request." ) ;

        if( packet.opcode == UGA_TFTP_OP_RRQ || packet.opcode == UGA_TFTP_OP_ACK )
        {
                _uga_tftp_handle_rrq( &packet, request ) ;
        }
        else
        {
                UGA_ERR_S( "uga::tftp::worker", "bad opcode: %d", packet.opcode ) ;
                uga_string errpacket = uga_tftp_craft_err_packet( UGA_TFTP_ERR_ILLEGAL_OP ) ;
                uga_send_str_to( &request->client, uga_sv_create_from( &errpacket ), 0 ) ;
                uga_str_destroy( &errpacket ) ;
        }
        UGA_INFO_S( "uga::tftp::worker", "cleaning up..." ) ;
        uga_vec_destroy( data ) ;
        UGA_INFO_S( "uga::tftp::worker", "finished." ) ;

        return 0 ;
}

i32_t _uga_tftp_server_run ( uga_tftp_server * server )
{
        UGA_INFO_S( "uga::tftp", "starting server..." ) ;
        UGA_INFO_S( "uga::tftp", "creating thread pool..." ) ;

        uga_thread_pool * pool = uga_pool_create_new( 8 ) ;

        UGA_INFO_S( "uga::tftp", "thread pool created." ) ;
        UGA_INFO_S( "uga::tftp", "starting main loop..." ) ;
        while( true )
        {
                uga_socket client = server->sock ;

                UGA_INFO_S( "uga::tftp", "waiting for a message..." ) ;
                uga_string request = uga_recv_str_from( &client, 0 ) ;
//                uga_sock_connect( client ) ;
                UGA_INFO_S( "uga::tftp", "received a message." ) ;

                uga_vector task_vec = uga_vec_create_1( uga_tftp_inflight_request *, 1 ) ;

                uga_dl_node * req_node = server->reqs.head ;

                while( req_node != NULL )
                {
                        uga_tftp_inflight_request * req = req_node->data ;
                        if( memcmp( &req->client, &client, sizeof( uga_socket ) ) == 0 )
                        {
                                UGA_INFO_S( "uga::tftp", "message was from known client." ) ;
                                uga_str_destroy( &req->last_req ) ;
                                req->last_req = request ;
                                uga_vec_push_back( &task_vec, &req ) ;
                                break ;
                        }
                        req_node = req_node->next ;
                }
                if( !req_node )
                {
                        UGA_INFO_S( "uga::tftp", "message was from new client." ) ;
                        uga_tftp_inflight_request inflight = { server, client, uga_str_create_0(), request, -1 } ;
                        uga_dl_list_push_front( &server->reqs, &inflight ) ;
                        uga_vec_push_back( &task_vec, &server->reqs.head->data ) ;
                }
                UGA_INFO_S( "uga::tftp", "dispatching worker to handle client..." ) ;
                uga_task handle_client = { task_vec, _uga_tftp_client_handler } ;
                uga_pool_add_task( pool, handle_client ) ;
                UGA_INFO_S( "uga::tftp", "worker dispatched." ) ;
        }
        return 0 ;
}

uga_string uga_tftp_craft_rq_packet ( uga_tftp_opcode const opcode, uga_string_view filename, uga_tftp_mode const mode )
{
        uga_string packet = uga_str_create_1( 2 + filename.size + 10 ) ;

        u16_t opcode_n = uga_htons( opcode ) ;

        uga_str_push_back( &packet, ( u8_t ) opcode_n ) ;
        uga_str_push_back( &packet, opcode_n >> 8 ) ;

        uga_str_append( &packet, filename.data, filename.size ) ;
        uga_str_push_back( &packet, '\0' ) ;

        switch( mode )
        {
                case UGA_TFTP_MODE_NETASCII:
                        uga_str_append( &packet, "netascii\0", 9 ) ;
                        break ;
                case UGA_TFTP_MODE_OCTET:
                        uga_str_append( &packet, "octet\0", 9 ) ;
                        break ;
                case UGA_TFTP_MODE_MAIL:
                        uga_str_append( &packet, "mail\0", 9 ) ;
                        break ;
                case UGA_TFTP_MODE_BAD:
                        break ;
                default:
                        break ;
        }
        return packet ;
}

uga_string uga_tftp_craft_ack_packet ( u16_t const blockno )
{
        uga_string packet = uga_str_create_1( 4 ) ;

        u16_t opcode_n = uga_htons( UGA_TFTP_OP_ACK ) ;
        u16_t  block_n = uga_htons(         blockno ) ;

        uga_str_push_back( &packet, ( u8_t ) opcode_n ) ;
        uga_str_push_back( &packet, opcode_n >> 8 ) ;
        uga_str_push_back( &packet, ( u8_t ) block_n ) ;
        uga_str_push_back( &packet, block_n >> 8 ) ;

        return packet ;
}

uga_string uga_tftp_craft_err_packet ( uga_tftp_err const errcode )
{
        uga_string packet = uga_str_create_1( 2 + 2 + 5 + 1 ) ;

        u16_t  opcode_n = uga_htons( UGA_TFTP_OP_ERR ) ;
        u16_t errcode_n = uga_htons( errcode ) ;

        uga_str_push_back( &packet, ( u8_t ) opcode_n ) ;
        uga_str_push_back( &packet, opcode_n >> 8 ) ;
        uga_str_push_back( &packet, ( u8_t ) errcode_n ) ;
        uga_str_push_back( &packet, errcode_n >> 8 ) ;
        uga_str_append( &packet, "error\0", 6 ) ; // make this more detailed depending on errcode

        return packet ;
}

uga_string uga_tftp_craft_data_packet ( i64_t blockno, uga_string_view data )
{
        uga_string packet = uga_str_create_1( 2 + 2 + data.size + 1 ) ;

        i64_t datalen = data.size > 512 ? 512 : data.size ;

        u16_t  opcode_n = uga_htons( UGA_TFTP_OP_DATA ) ;
        u16_t blockno_n = uga_htons( blockno ) ;

        uga_str_push_back( &packet, ( u8_t ) opcode_n ) ;
        uga_str_push_back( &packet, opcode_n >> 8 ) ;
        uga_str_push_back( &packet, ( u8_t ) blockno_n ) ;
        uga_str_push_back( &packet, blockno_n >> 8 ) ;
        uga_str_append( &packet, data.data, datalen ) ;
        uga_str_push_back( &packet, '\0' ) ;

        return packet ;
}
