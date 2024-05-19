//
//
//      uganet
//      uga_tftp.c
//

#include <net/uga_tftp.h>
#include <core/uga_strview.h>


void _uga_tftp_parse_rq_packet   ( uga_tftp_packet_raw const raw_packet, uga_tftp_rq_packet   * packet ) ;
void _uga_tftp_parse_data_packet ( uga_tftp_packet_raw const raw_packet, uga_tftp_data_packet * packet ) ;
void _uga_tftp_parse_ack_packet  ( uga_tftp_packet_raw const raw_packet, uga_tftp_ack_packet  * packet ) ;
void _uga_tftp_parse_err_packet  ( uga_tftp_packet_raw const raw_packet, uga_tftp_err_packet  * packet ) ;

uga_tftp_packet uga_tftp_parse_packet ( uga_tftp_packet_raw const raw_packet )
{
        uga_tftp_packet packet = { 0 } ;

        switch( raw_packet.opcode )
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

void _uga_tftp_parse_rq_packet ( uga_tftp_packet_raw const raw_packet, uga_tftp_rq_packet * packet )
{
        static uga_string_view const netascii_str = { "netascii", 8 } ;
        static uga_string_view const    octet_str = {    "octet", 5 } ;
        static uga_string_view const     mail_str = {     "mail", 4 } ;

        uga_string_view raw_data = uga_sv_create_2( ( char const * ) &raw_packet, sizeof( raw_packet ) ) ;

        packet->opcode = raw_data.data[ 0 ] ;
        uga_sv_chop_left( &raw_data, 2 ) ;

        uga_string_view filename = uga_sv_chop_to_delimiter( &raw_data, '\0', true ) ;
        uga_string_view     mode = uga_sv_chop_to_delimiter( &raw_data, '\0', true ) ;

        packet->filename = uga_str_create_cstr( filename ) ;

        if     ( uga_sv_equal_insensitive( mode, netascii_str ) ) packet->mode = UGA_TFTP_MODE_NETASCII ;
        else if( uga_sv_equal_insensitive( mode,    octet_str ) ) packet->mode = UGA_TFTP_MODE_OCTET    ;
        else if( uga_sv_equal_insensitive( mode,     mail_str ) ) packet->mode = UGA_TFTP_MODE_MAIL     ;
        else                                                      packet->mode = UGA_TFTP_MODE_BAD      ;
}

void _uga_tftp_parse_data_packet ( uga_tftp_packet_raw const raw_packet, uga_tftp_data_packet * packet )
{
        uga_string_view raw_data = uga_sv_create_2( ( char const * ) &raw_packet, sizeof( raw_packet ) ) ;

        packet-> opcode = raw_data.data[ 0 ] ;
        packet->blockno = raw_data.data[ 2 ] ;
        uga_sv_chop_left( &raw_data, 4 ) ;

        uga_string_view data = uga_sv_chop_to_delimiter( &raw_data, '\0', true ) ;

        packet->data= uga_str_create_cstr( data ) ;
}

void _uga_tftp_parse_ack_packet ( uga_tftp_packet_raw const raw_packet, uga_tftp_ack_packet * packet )
{
        packet-> opcode = raw_packet.opcode    ;
        packet->blockno = raw_packet.data[ 0 ] ;
}

void _uga_tftp_parse_err_packet ( uga_tftp_packet_raw const raw_packet, uga_tftp_err_packet * packet )
{
        uga_string_view raw_data = uga_sv_create_2( ( char const * ) &raw_packet, sizeof( raw_packet ) ) ;

        packet-> opcode = raw_data.data[ 0 ] ;
        packet->errcode = raw_data.data[ 2 ] ;
        uga_sv_chop_left( &raw_data, 4 ) ;

        uga_string_view errmsg = uga_sv_chop_to_delimiter( &raw_data, '\0', true ) ;

        packet->errmsg = uga_str_create_cstr( errmsg ) ;
}
