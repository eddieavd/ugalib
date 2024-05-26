//
//
//      uganet
//      uga_ftp.c
//

#include <net/uga_ftp.h>
#include <core/uga_log.h>
#include <core/uga_err.h>
#include <core/uga_fs.h>
#include <core/uga_strview.h>
#include <net/uga_talk.h>
#include <net/uga_async.h>

#include <string.h>


void _uga_tftp_print_packet_bytes ( uga_string * rawpacket ) ;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// uga_tftp_packet parse
///

void _uga_tftp_parse_rq_packet   ( uga_string_view packet_data, uga_tftp_rq_packet   * packet ) ;
void _uga_tftp_parse_ack_packet  ( uga_string_view packet_data, uga_tftp_ack_packet  * packet ) ;
void _uga_tftp_parse_data_packet ( uga_string_view packet_data, uga_tftp_data_packet * packet ) ;
void _uga_tftp_parse_err_packet  ( uga_string_view packet_data, uga_tftp_err_packet  * packet ) ;

uga_tftp_packet uga_tftp_parse_packet ( uga_string_view packet_data )
{
        uga_tftp_packet packet = { 0 } ;

        u16_t opcode_n = *( u16_t * ) packet_data.data ;
        u16_t opcode = uga_ntohs( opcode_n ) ;
        uga_sv_chop_left( &packet_data, 2 ) ;

        packet.opcode = opcode ;

        switch( packet.opcode )
        {
                case UGA_TFTP_OP_RRQ:
                        _uga_tftp_parse_rq_packet( packet_data, ( uga_tftp_rq_packet * ) &packet ) ;
                        break ;
                case UGA_TFTP_OP_WRQ:
                        _uga_tftp_parse_rq_packet( packet_data, ( uga_tftp_rq_packet * ) &packet ) ;
                        break ;
                case UGA_TFTP_OP_ACK:
                        _uga_tftp_parse_ack_packet( packet_data, ( uga_tftp_ack_packet * ) &packet ) ;
                        break ;
                case UGA_TFTP_OP_DATA:
                        _uga_tftp_parse_data_packet( packet_data, ( uga_tftp_data_packet * ) &packet ) ;
                        break ;
                case UGA_TFTP_OP_ERR:
                        _uga_tftp_parse_err_packet( packet_data, ( uga_tftp_err_packet * ) &packet ) ;
                        break ;
                case UGA_TFTP_OP_BAD:
                        packet.opcode = UGA_TFTP_OP_BAD ;
                        break ;
                default:
                        packet.opcode = UGA_TFTP_OP_BAD ;
                        break ;
        }
        return packet ;
}

void _uga_tftp_parse_rq_packet ( uga_string_view packet_data, uga_tftp_rq_packet * packet )
{
        static uga_string_view const netascii_str = { "netascii", 8 } ;
        static uga_string_view const    octet_str = {    "octet", 5 } ;
        static uga_string_view const     mail_str = {     "mail", 4 } ;

        uga_string_view filename = uga_sv_chop_to_delimiter( &packet_data, '\0', true ) ;
        uga_string_view     mode = uga_sv_chop_to_delimiter( &packet_data, '\0', true ) ;

        packet->filename = uga_str_create_cstr( filename ) ;

        if      ( uga_sv_equal_insensitive( mode, netascii_str ) ) packet->mode = UGA_TFTP_MODE_NETASCII ;
        else if ( uga_sv_equal_insensitive( mode,    octet_str ) ) packet->mode = UGA_TFTP_MODE_OCTET    ;
        else if ( uga_sv_equal_insensitive( mode,     mail_str ) ) packet->mode = UGA_TFTP_MODE_MAIL     ;
        else                                                       packet->mode = UGA_TFTP_MODE_BAD      ;
}

void _uga_tftp_parse_ack_packet ( uga_string_view packet_data, uga_tftp_ack_packet * packet )
{
        packet->blockno = uga_ntohs( *( u16_t * ) packet_data.data ) ;
}

void _uga_tftp_parse_data_packet ( uga_string_view packet_data, uga_tftp_data_packet * packet )
{
        packet->blockno = uga_ntohs( *( u16_t * ) packet_data.data ) ;
        uga_sv_chop_left( &packet_data, 2 ) ;

        uga_string_view data = uga_sv_chop_to_delimiter( &packet_data, '\0', true ) ;

        packet->data = uga_str_create_cstr( data ) ;
}

void _uga_tftp_parse_err_packet ( uga_string_view packet_data, uga_tftp_err_packet * packet )
{
        packet->errcode = uga_ntohs( *( u16_t * ) packet_data.data ) ;
        uga_sv_chop_left( &packet_data, 2 ) ;

        uga_string_view errmsg = uga_sv_chop_to_delimiter( &packet_data, '\0', true ) ;

        packet->errmsg = uga_str_create_cstr( errmsg ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// uga_tftp_packet craft
///

uga_tftp_packet_raw uga_tftp_craft_rq_packet ( uga_tftp_opcode const opcode, uga_string_view filename, uga_tftp_mode const mode )
{
        uga_tftp_packet_raw rawpacket = { 0 } ;

        uga_string rawdata = { ( char * ) &rawpacket, 0, sizeof( rawpacket ) } ;

        u16_t opcode_n = uga_htons( opcode ) ;

        uga_str_push_back( &rawdata, ( u8_t ) ( opcode_n      ) ) ;
        uga_str_push_back( &rawdata, ( u8_t ) ( opcode_n >> 8 ) ) ;
        uga_str_append( &rawdata, filename.data, filename.size ) ;
        uga_str_push_back( &rawdata, '\0' ) ;

        switch( mode )
        {
                case UGA_TFTP_MODE_NETASCII:
                        uga_str_append( &rawdata, "netascii\0", 9 ) ;
                        break ;
                case UGA_TFTP_MODE_OCTET:
                        uga_str_append( &rawdata, "octet\0", 6 ) ;
                        break ;
                case UGA_TFTP_MODE_MAIL:
                        uga_str_append( &rawdata, "mail\0", 5 ) ;
                        break ;
                case UGA_TFTP_MODE_BAD:
                        uga_str_append( &rawdata, "error\0", 6 ) ;
                        break ;
                default:
                        uga_str_append( &rawdata, "error\0", 6 ) ;
                        break ;
        }
        return rawpacket ;
}

uga_tftp_packet_raw uga_tftp_craft_ack_packet ( i32_t const blockno )
{
        uga_tftp_packet_raw rawpacket = { 0 } ;

        uga_string rawdata = { ( char * ) &rawpacket, 0, sizeof( rawpacket ) } ;

        u16_t  opcode_n = uga_htons( UGA_TFTP_OP_ACK ) ;
        u16_t blockno_n = uga_htons(         blockno ) ;

        uga_str_push_back( &rawdata, ( u8_t ) (  opcode_n      ) ) ;
        uga_str_push_back( &rawdata, ( u8_t ) (  opcode_n >> 8 ) ) ;
        uga_str_push_back( &rawdata, ( u8_t ) ( blockno_n      ) ) ;
        uga_str_push_back( &rawdata, ( u8_t ) ( blockno_n >> 8 ) ) ;

        return rawpacket ;
}

uga_tftp_packet_raw uga_tftp_craft_err_packet ( uga_tftp_err const errcode )
{
        uga_tftp_packet_raw rawpacket = { 0 } ;

        uga_string rawdata = { ( char * ) &rawpacket, 0, sizeof( rawpacket ) } ;

        u16_t  opcode_n = uga_htons( UGA_TFTP_OP_ERR ) ;
        u16_t errcode_n = uga_htons(         errcode ) ;

        uga_str_push_back( &rawdata, ( u8_t ) (  opcode_n      ) ) ;
        uga_str_push_back( &rawdata, ( u8_t ) (  opcode_n >> 8 ) ) ;
        uga_str_push_back( &rawdata, ( u8_t ) ( errcode_n      ) ) ;
        uga_str_push_back( &rawdata, ( u8_t ) ( errcode_n >> 8 ) ) ;
        uga_str_append( &rawdata, "error\0", 6 ) ;

        return rawpacket ;
}

uga_tftp_packet_raw uga_tftp_craft_data_packet ( i32_t const blockno, uga_string_view data )
{
        uga_tftp_packet_raw rawpacket = { 0 } ;

        uga_string rawdata = { ( char * ) &rawpacket, 0, sizeof( rawpacket ) } ;

        u16_t  opcode_n = uga_htons( UGA_TFTP_OP_DATA ) ;
        u16_t blockno_n = uga_htons(          blockno ) ;

        i32_t datalen = data.size < 512 ? data.size : 512 ;

        uga_str_push_back( &rawdata, ( u8_t ) (  opcode_n      ) ) ;
        uga_str_push_back( &rawdata, ( u8_t ) (  opcode_n >> 8 ) ) ;
        uga_str_push_back( &rawdata, ( u8_t ) ( blockno_n      ) ) ;
        uga_str_push_back( &rawdata, ( u8_t ) ( blockno_n >> 8 ) ) ;
        uga_str_append( &rawdata, data.data, datalen ) ;

        return rawpacket ;
}

////////////////////////////////////////////////////////////////////////////////
/// uga_tftp_packet destroy
///

void _uga_tftp_destroy_rq_packet   ( uga_tftp_packet * packet ) ;
void _uga_tftp_destroy_data_packet ( uga_tftp_packet * packet ) ;
void _uga_tftp_destroy_err_packet  ( uga_tftp_packet * packet ) ;

void uga_tftp_packet_destroy ( uga_tftp_packet * packet )
{
        switch( packet->opcode )
        {
                case UGA_TFTP_OP_RRQ:
                        _uga_tftp_destroy_rq_packet( packet ) ;
                        break ;
                case UGA_TFTP_OP_WRQ:
                        _uga_tftp_destroy_rq_packet( packet ) ;
                        break ;
                case UGA_TFTP_OP_DATA:
                        _uga_tftp_destroy_data_packet( packet ) ;
                        break ;
                case UGA_TFTP_OP_ERR:
                        _uga_tftp_destroy_err_packet( packet ) ;
                        break ;
                default:
                        break ;
        }
}

void _uga_tftp_destroy_rq_packet ( uga_tftp_packet * packet )
{
        uga_tftp_rq_packet * rqpacket = ( uga_tftp_rq_packet * ) packet ;

        uga_str_destroy( &rqpacket->filename ) ;
}
void _uga_tftp_destroy_data_packet ( uga_tftp_packet * packet )
{
        uga_tftp_data_packet * datapacket = ( uga_tftp_data_packet * ) packet ;

        uga_str_destroy( &datapacket->data ) ;
}
void _uga_tftp_destroy_err_packet ( uga_tftp_packet * packet )
{
        uga_tftp_err_packet * errpacket = ( uga_tftp_err_packet * ) packet ;

        uga_str_destroy( &errpacket->errmsg ) ;
}

void uga_tftp_packet_destroy_void ( void * packet )
{
        uga_tftp_packet_destroy( packet ) ;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// uga_tftp_server
///

static uga_sock_conf _uga_tftp_sock_config = { UGA_SFAM_IPv4, UGA_SOCK_DGRAM, UGA_PASSIVE } ;

void uga_tftp_inflight_destroy ( uga_tftp_inflight * request ) ;

uga_tftp_server uga_tftp_server_create ( uga_string_view port )
{
        uga_tftp_server server = { 0 } ;

        server.sock = uga_sock_create_and_bind_configured( uga_sv_create_1( "localhost" ), port, &_uga_tftp_sock_config ) ;
        UGA_RETURN_ON_ERR( server ) ;
        UGA_INFO_S( "uga::tftp::server::create", "bound socket on port "SV_FMT, SV_ARG( port ) ) ;
        server.pool = uga_pool_create( 8 ) ;
        UGA_RETURN_ON_ERR( server ) ;
        UGA_INFO_S( "uga::tftp::server::create", "created thread pool with 8 workers" ) ;
        server.reqs = uga_dl_list_create( uga_tftp_inflight ) ;
        UGA_RETURN_ON_ERR( server ) ;
        server.rmtx = uga_mtx_init( UGA_MTX_PLAIN ) ;
        UGA_INFO_S( "uga::tftp::server::create", "initialized request queue and access mutex" ) ;
        UGA_INFO_S( "uga::tftp::server::create", "finished." ) ;

        return server ;
}

i32_t _uga_tftp_handle_new_client      ( void * arg ) ;
i32_t _uga_tftp_handle_existing_client ( void * arg ) ;

bool _uga_match_sockets ( void * lhs, void * rhs )
{
        uga_tftp_inflight * lhs_s = lhs ;
        return memcmp( &lhs_s->client, rhs, sizeof( uga_socket ) ) == 0 ;
}

i32_t uga_tftp_server_run ( uga_tftp_server * server )
{
        UGA_INFO_S( "uga::tftp::server", "started." ) ;

        uga_vector sockets = uga_vec_create_1( uga_socket, 2 ) ;
        uga_socket stdsock = { 0, fileno( stdin ), { 0 } } ;

        uga_vec_push_back( &sockets, &server->sock ) ;
        uga_vec_push_back( &sockets, &     stdsock ) ;

        while( true )
        {
                UGA_INFO_S( "uga::tftp::server", "waiting for messages..." ) ;

                uga_socket client = uga_async_next( sockets ) ;

                if( client.sockfd == stdsock.sockfd )
                {
                        uga_string input = uga_str_create_1( 256 ) ;
                        uga_fs_read_fd( client.sockfd, &input ) ;
                        if( uga_sv_starts_with_insensitive( uga_sv_create_from( &input ), uga_sv_create_1( "quit" ) ) )
                        {
                                uga_str_destroy( &input ) ;
                                uga_vec_destroy( &sockets ) ;
                                return 0 ;
                        }
                        uga_str_destroy( &input ) ;
                        continue ;
                }
                uga_string message = uga_recv_str_from( &client, 0 ) ;

                uga_mtx_acquire( &server->rmtx ) ;
                UGA_INFO_S( "uga::tftp::server", "checking if incoming message is from existing client..." ) ;
                uga_dl_node * clientdata = uga_dl_list_find_if( &server->reqs, _uga_match_sockets, &client ) ;
                uga_mtx_release( &server->rmtx ) ;

                if( clientdata == NULL )
                {
                        UGA_INFO_S( "uga::tftp::server", "received message from new client." ) ;

                        uga_tftp_inflight new_request = { server, client, uga_str_create_0(),
                                uga_tftp_parse_packet( uga_sv_create_from( &message ) ), 0 } ;

                        uga_mtx_acquire( &server->rmtx ) ;
                        uga_dl_list_push_front( &server->reqs, &new_request ) ;
                        uga_tftp_inflight * reqptr = ( uga_tftp_inflight * ) server->reqs.head->data ;
                        uga_mtx_release( &server->rmtx ) ;

                        UGA_INFO_S( "uga::tftp::server", "request added to queue." ) ;

                        uga_vector taskdata = uga_vec_create_1( uga_tftp_inflight *, 1 ) ;
                        uga_vec_push_back( &taskdata, &reqptr ) ;

                        uga_task handle_new_client = { taskdata, _uga_tftp_handle_new_client } ;
                        uga_pool_add_task( server->pool, handle_new_client ) ;
                        UGA_INFO_S( "uga::tftp::server", "dispatched worker to handle new client" ) ;
                }
                else
                {
                        UGA_INFO_S( "uga::tftp::server", "received message from existing client." ) ;

                        uga_mtx_acquire( &server->rmtx ) ;
                        uga_tftp_inflight * request = ( uga_tftp_inflight * ) clientdata->data ;

                        uga_tftp_packet_destroy( &request->last_req ) ;
                        request->last_req = uga_tftp_parse_packet( uga_sv_create_from( &message ) ) ;
                        uga_mtx_release( &server->rmtx ) ;

                        UGA_INFO_S( "uga::tftp::server", "updated message on inflight." ) ;

                        uga_vector taskdata = uga_vec_create_1( uga_tftp_inflight *, 1 ) ;
                        uga_vec_push_back( &taskdata, &clientdata->data ) ;

                        uga_task handle_existing_client = { taskdata, _uga_tftp_handle_existing_client } ;
                        uga_pool_add_task( server->pool, handle_existing_client ) ;
                        UGA_INFO_S( "uga::tftp::server", "dispatched worker to handle existing client" ) ;
                }
                uga_str_destroy( &message ) ;
        }
        uga_vec_destroy( &sockets ) ;
        UGA_INFO_S( "uga::tftp::server", "finished." ) ;

        return 0 ;
}

i32_t _uga_tftp_server_run_task ( void * server_vec )
{
        uga_vector * vec = ( uga_vector * ) server_vec ;
        uga_tftp_server * server = *( uga_tftp_server ** ) uga_vec_at( vec, 0 ) ;

        return uga_tftp_server_run( server ) ;
}

void _uga_tftp_handle_new_rrq ( uga_tftp_inflight * request, uga_tftp_rq_packet * parsed_packet ) ;
void _uga_tftp_handle_bad_new ( uga_tftp_inflight * request                                     ) ;

i32_t _uga_tftp_handle_new_client ( void * arg )
{
        UGA_INFO_S( "uga::tftp::worker::new", "started." ) ;

        uga_vector * taskdata = ( uga_vector * ) arg ;
        uga_tftp_inflight * request = *( uga_tftp_inflight ** ) uga_vec_at( taskdata, 0 ) ;

        uga_tftp_packet * packet = &request->last_req ;

        UGA_INFO_S( "uga::tftp::worker::new", "opcode is %d", packet->opcode ) ;

        switch( packet->opcode )
        {
                case UGA_TFTP_OP_RRQ:
                        _uga_tftp_handle_new_rrq( request, ( uga_tftp_rq_packet * ) packet ) ;
                        break ;
                case UGA_TFTP_OP_WRQ:
                        _uga_tftp_handle_bad_new( request ) ;
                        break ;
                case UGA_TFTP_OP_ACK:
                        _uga_tftp_handle_bad_new( request ) ;
                        break ;
                case UGA_TFTP_OP_DATA:
                        _uga_tftp_handle_bad_new( request ) ;
                        break ;
                case UGA_TFTP_OP_ERR:
                        _uga_tftp_handle_bad_new( request ) ;
                        break ;
                case UGA_TFTP_OP_BAD:
                        _uga_tftp_handle_bad_new( request ) ;
                        break ;
                default:
                        _uga_tftp_handle_bad_new( request ) ;
                        break ;
        }
        uga_tftp_packet_destroy( packet ) ;
        uga_vec_destroy( taskdata ) ;
        UGA_INFO_S( "uga::tftp::worker::new", "finished." ) ;
        return 0 ;
}

void _uga_tftp_handle_existing_ack ( uga_tftp_inflight * request, uga_tftp_ack_packet * parsed_packet ) ;
void _uga_tftp_handle_bad_existing ( uga_tftp_inflight * request                                      ) ;

i32_t _uga_tftp_handle_existing_client ( void * arg )
{
        UGA_INFO_S( "uga::tftp::worker::existing", "started." ) ;
        uga_vector * taskdata = ( uga_vector * ) arg ;
        uga_tftp_inflight * request = *( uga_tftp_inflight ** ) uga_vec_at( taskdata, 0 ) ;

        uga_tftp_packet * packet = &request->last_req ;

        UGA_INFO_S( "uga::tftp::worker::existing", "parsed incoming message." ) ;

        switch( packet->opcode )
        {
                case UGA_TFTP_OP_RRQ:
                        _uga_tftp_handle_bad_existing( request ) ;
                        break ;
                case UGA_TFTP_OP_WRQ:
                        _uga_tftp_handle_bad_existing( request ) ;
                        break ;
                case UGA_TFTP_OP_ACK:
                        _uga_tftp_handle_existing_ack( request, ( uga_tftp_ack_packet * ) packet ) ;
                        break ;
                case UGA_TFTP_OP_DATA:
                        _uga_tftp_handle_bad_existing( request ) ;
                        break ;
                case UGA_TFTP_OP_ERR:
                        _uga_tftp_handle_bad_existing( request ) ;
                        break ;
                case UGA_TFTP_OP_BAD:
                        _uga_tftp_handle_bad_existing( request ) ;
                        break ;
                default:
                        _uga_tftp_handle_bad_existing( request ) ;
                        break ;
        }
        uga_vec_destroy( taskdata ) ;
        UGA_INFO_S( "uga::tftp::worker::existing", "finished." ) ;

        return 0 ;
}

void _uga_tftp_handle_new_rrq ( uga_tftp_inflight * request, uga_tftp_rq_packet * packet )
{
        UGA_INFO_S( "uga::tftp::worker::new", "got RRQ request for file { "STR_FMT" }", STR_ARG( packet->filename ) ) ;

        uga_string_view filename = uga_sv_create_from( &packet->filename ) ;

        if( !uga_fs_file_exists( filename ) )
        {
                UGA_ERR_S( "uga::tftp::worker::new", "requested file not found" ) ;
                uga_tftp_send_err_packet( &request->client, UGA_TFTP_ERR_FILE_NOT_FOUND ) ;
                return ;
        }
        if( !uga_fs_file_readable( filename ) )
        {
                UGA_ERR_S( "uga::tftp::worker::new", "requested file not readable" ) ;
                uga_tftp_send_err_packet( &request->client, UGA_TFTP_ERR_ACCESS_VIOLATION ) ;
                return ;
        }
        if( !uga_str_empty( request->filedata ) )
        {
                UGA_WARN_S( "uga::tftp::worker::new", "inflight already contains filedata, overwriting..." ) ;
                uga_str_destroy( &request->filedata ) ;
        }
        request->filedata = uga_fs_read_file( filename ) ;
        if( uga_had_errs() )
        {
                uga_print_error() ;
                uga_tftp_send_err_packet( &request->client, UGA_TFTP_ERR_UNDEF ) ;
                return ;
        }
        UGA_INFO_S( "uga::tftp::worker::new", "file read, filesize is %lld", request->filedata.size ) ;
        uga_string_view data = uga_sv_create_from( &request->filedata ) ;
        uga_string_view to_send = uga_sv_chop_left( &data, 512 ) ;

        uga_tftp_packet_raw datapacket = uga_tftp_craft_data_packet( 0, to_send ) ;

        UGA_INFO_S( "uga::tftp::worker::new", "sending data packet..." ) ;
        uga_tftp_send_packet( &request->client, &datapacket ) ;
        UGA_INFO_S( "uga::tftp::worker::new", "data packet sent." ) ;
}
void _uga_tftp_handle_bad_new ( uga_tftp_inflight * request )
{
        uga_tftp_send_err_packet( &request->client, UGA_TFTP_ERR_ILLEGAL_OP ) ;
}
void _uga_tftp_handle_existing_ack ( uga_tftp_inflight * request, uga_tftp_ack_packet * packet )
{
        if( request->last_ack != packet->blockno )
        {
                UGA_ERR_S( "uga::tftp::worker::existing", "bad block acknowledged, expected %d, got %d",
                           request->last_ack + 1, packet->blockno ) ;
                uga_tftp_send_err_packet( &request->client, UGA_TFTP_ERR_ILLEGAL_OP ) ;
                return ;
        }
        UGA_INFO_S( "uga::tftp::worker::existing", "ack %d is good", packet->blockno ) ;
        ++request->last_ack ;

        i64_t offset = request->last_ack * 512 ;

        UGA_DBG_S( "uga::tftp::worker::existing", "filedata size is %lld, offset is %lld", request->filedata.size, offset ) ;
        uga_string_view data = uga_sv_create_from( &request->filedata ) ;
        uga_sv_chop_left( &data, offset ) ;
        uga_string_view to_send = uga_sv_chop_left( &data, 512 ) ;

        if( to_send.size == 0 )
        {
                UGA_INFO_S( "uga::tftp::worker::existing", "last packet acknowledged, transmission finished." ) ;
                uga_mtx_t * mtx = &request->server->rmtx ;
                UGA_DBG_S( "uga::tftp::worker::existing", "acquiring request mutex..." ) ;
                uga_mtx_acquire( mtx ) ;
                UGA_DBG_S( "uga::tftp::worker::existing", "destroying filedata..." ) ;
                uga_str_destroy( &request->filedata ) ;
                UGA_DBG_S( "uga::tftp::worker::existing", "removing request from queue..." ) ;
                uga_dl_list_remove_elem( &request->server->reqs, request ) ;
                UGA_DBG_S( "uga::tftp::worker::existing", "releasing mutex..." ) ;
                uga_mtx_release( mtx ) ;
                UGA_INFO_S( "uga::tftp::worker::existing", "client removed from queue." ) ;
                return ;
        }
        uga_tftp_packet_raw datapacket = uga_tftp_craft_data_packet( request->last_ack, to_send ) ;

        UGA_INFO_S( "uga::tftp::worker::existing", "sending data packet %d...", request->last_ack + 1 ) ;
        uga_tftp_send_packet( &request->client, &datapacket ) ;
        UGA_INFO_S( "uga::tftp::worker::existing", "data packet sent." ) ;
}
void _uga_tftp_handle_bad_existing ( uga_tftp_inflight * request )
{
        uga_tftp_send_err_packet( &request->client, UGA_TFTP_ERR_ILLEGAL_OP ) ;
}

void uga_tftp_send_packet ( uga_socket * client, uga_tftp_packet_raw * packet )
{
        uga_send_to( client, packet, sizeof( *packet ), 0 ) ;
}

void uga_tftp_send_err_packet ( uga_socket * client, uga_tftp_err const errcode )
{
        uga_tftp_packet_raw errpacket = uga_tftp_craft_err_packet( errcode ) ;
        uga_tftp_send_packet( client, &errpacket ) ;
}

void uga_tftp_server_destroy ( uga_tftp_server * server )
{
        UGA_INFO_S( "uga::tftp::server::destroy", "releasing pool..." ) ;
        uga_pool_destroy   (  server->pool ) ;
        UGA_INFO_S( "uga::tftp::server::destroy", "clearing pending requests..." ) ;
        uga_dl_list_destroy( &server->reqs ) ;
        uga_mtx_destroy    ( &server->rmtx ) ;
        UGA_INFO_S( "uga::tftp::server::destroy", "closing socket..." ) ;
        uga_sock_close     ( &server->sock ) ;
        UGA_INFO_S( "uga::tftp::server::destroy", "finished." ) ;
}

void uga_tftp_server_destroy_void ( void * server )
{
        uga_tftp_server_destroy( server ) ;
}

void uga_tftp_inflight_destroy ( uga_tftp_inflight * request )
{
        uga_str_destroy( &request->filedata ) ;
        uga_tftp_packet_destroy( &request->last_req ) ;
}

void _uga_tftp_print_packet_bytes ( uga_string * rawpacket )
{
        u8_t const * str = ( u8_t const * ) rawpacket->data ;
        i32_t pos = rawpacket->size ;

        while( --pos )
        {
                printf( "%hhx", *str++ ) ;
        }
        printf( "\n" ) ;
}
