//
//
//      uganet
//      tftp_client.c
//

#include <core/uga_log.h>
#include <core/uga_err.h>
#include <core/uga_strview.h>

#include <net/uga_sock.h>
#include <net/uga_talk.h>
#include <net/uga_tftp.h>

#include <strings.h>
#include <arpa/inet.h>


// static uga_sock_conf const config = { UGA_SFAM_IPv4, UGA_SOCK_DGRAM, 0 } ;

void configure_uga ( void )
{
        uga_log_init() ;
}

int main ( int argc, char ** argv )
{
        configure_uga() ;

        if( argc < 2 )
        {
                UGA_ERR_S( "tftp::client", "usage: ./tftp_client filename" ) ;
                return 1 ;
        }
        uga_string_view filename = uga_sv_create_1( argv[ 1 ] ) ;

        int sockfd ;
        struct sockaddr_in servaddr ;

        bzero( &servaddr, sizeof( servaddr ) ) ;
        servaddr.sin_addr.s_addr = inet_addr( "127.0.0.1" ) ;
        servaddr.sin_port = htons( 8372 ) ;
        servaddr.sin_family = AF_INET ;

        sockfd = socket( AF_INET, SOCK_DGRAM, 0 ) ;

        if( connect( sockfd, ( struct sockaddr * ) &servaddr, sizeof( servaddr ) ) < 0 )
        {
                UGA_ERR_S( "tftp::client", "connect failed" ) ;
                return 1 ;
        }
        uga_string request = uga_tftp_craft_rq_packet( UGA_TFTP_OP_RRQ, filename, UGA_TFTP_MODE_NETASCII ) ;

        sendto( sockfd, request.data, request.size, 0, NULL, sizeof( servaddr ) ) ;
        uga_str_destroy( &request ) ;

        uga_string response = uga_str_create_1( 1024 ) ;

        while( true )
        {
                response.size = recvfrom( sockfd, response.data, response.capacity, 0, NULL, NULL ) ;

                uga_tftp_packet packet = uga_tftp_parse_packet( uga_sv_create_from( &response ) ) ;
                uga_tftp_data_packet * datapacket = ( uga_tftp_data_packet * ) &packet ;

                printf( STR_FMT, STR_ARG( datapacket->data ) ) ;
                uga_str_destroy( &datapacket->data ) ;

                uga_string ack = uga_tftp_craft_ack_packet( datapacket->blockno ) ;

                sendto( sockfd, ack.data, ack.size, 0, NULL, sizeof( servaddr ) ) ;
                uga_str_destroy( &ack ) ;

                if( response.size < 514 ) break ;
        }
        uga_str_destroy( &response ) ;

        /*
        uga_string_view filename = uga_sv_create_1( argv[ 1 ] ) ;

        UGA_INFO_S( "tftp::client", "started." ) ;

        uga_socket tftp_server = uga_sock_create_and_connect_configured( ( uga_string_view ){ "localhost", 9 },
                                                                         ( uga_string_view ){      "8372", 4 },
                                                                         &config ) ;
        UGA_INFO_S( "tftp::client", "created socket to server." ) ;

        uga_string request = uga_tftp_craft_rq_packet( UGA_TFTP_OP_RRQ, filename, UGA_TFTP_MODE_NETASCII ) ;
        uga_send_str( &tftp_server, uga_sv_create_from( &request ), 0 );
        uga_str_destroy( &request ) ;

        UGA_INFO_S( "tftp::client", "sent read request for file "SV_FMT, SV_ARG( filename ) ) ;

        uga_string ackpacket_str = uga_tftp_craft_ack_packet( 0 ) ;

        while( true )
        {
                UGA_INFO_S( "tftp::client", "waiting for server response..." ) ;
                uga_string response = uga_recv_str( &tftp_server, 0 ) ;
                UGA_INFO_S( "tftp::client", "got response from server." ) ;

                uga_tftp_packet packet = uga_tftp_parse_packet( uga_sv_create_from( &response ) ) ;

                if( packet.opcode != UGA_TFTP_OP_DATA )
                {
                        UGA_ERR_S( "tftp::client", "error from server" ) ;
                        break ;
                }
                UGA_INFO_S( "tftp::client", "response is data packet" ) ;
                uga_tftp_data_packet * datapacket = ( uga_tftp_data_packet * ) &packet ;

                printf( STR_FMT, STR_ARG( datapacket->data ) ) ;

                u16_t blockno = datapacket->blockno ;

                ( ( uga_tftp_ack_packet * ) &ackpacket_str.data )->blockno = blockno ;

                uga_send_str( &tftp_server, uga_sv_create_from( &ackpacket_str ), 0 ) ;
                uga_print_if_err() ;

                UGA_INFO_S( "tftp::client", "sent ack packet for block %u", blockno ) ;

                i64_t datalen = datapacket->data.size ;

                uga_str_destroy( &datapacket->data ) ;
                uga_str_destroy( &response         ) ;

                if( datalen < 512 ) break ;
        }



        return 0 ;
        */
}
