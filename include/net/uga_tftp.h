//
//
//      uganet
//      uga_tftp.h
//

#ifndef UGA_TFTP_H_
#define UGA_TFTP_H_

#include <core/uga_util.h>
#include <core/uga_str.h>
#include <core/uga_thread.h>
#include <core/uga_vector.h>
#include <core/uga_list.h>
#include <net/uga_async.h>


typedef enum
{
        UGA_TFTP_OP_ZERO ,
        UGA_TFTP_OP_RRQ  ,
        UGA_TFTP_OP_WRQ  ,
        UGA_TFTP_OP_DATA ,
        UGA_TFTP_OP_ACK  ,
        UGA_TFTP_OP_ERR  ,
        UGA_TFTP_OP_BAD  ,
} uga_tftp_opcode ;

typedef enum
{
        UGA_TFTP_MODE_NETASCII ,
        UGA_TFTP_MODE_OCTET    ,
        UGA_TFTP_MODE_MAIL     ,
        UGA_TFTP_MODE_BAD      ,
} uga_tftp_mode ;

typedef enum
{
        UGA_TFTP_ERR_UNDEF            ,
        UGA_TFTP_ERR_FILE_NOT_FOUND   ,
        UGA_TFTP_ERR_ACCESS_VIOLATION ,
        UGA_TFTP_ERR_DISK_FULL        ,
        UGA_TFTP_ERR_ILLEGAL_OP       ,
        UGA_TFTP_ERR_UNKNOWN_TID      ,
        UGA_TFTP_ERR_FILE_EXISTS      ,
        UGA_TFTP_ERR_NO_SUCH_USER     ,
} uga_tftp_err ;


/*
typedef struct
{
        u16_t opcode ;
        u8_t data[ 514 ] ;
} UGA_ALIGN( 8 ) uga_tftp_packet_raw ;
*/

typedef struct
{
        uga_tftp_opcode opcode ;
        u8_t data [ 32 - sizeof( uga_tftp_opcode ) ] ;
} UGA_ALIGN( 8 ) uga_tftp_packet ;

typedef struct
{
        uga_tftp_opcode opcode ;
        uga_tftp_mode     mode ;
        uga_string    filename ;
} uga_tftp_rq_packet ;

typedef struct
{
        uga_tftp_opcode opcode ;
        u16_t          blockno ;
        uga_string        data ;
} uga_tftp_data_packet ;

typedef struct
{
        uga_tftp_opcode opcode ;
        u16_t          blockno ;
        u8_t pad [ 32 - sizeof( uga_tftp_opcode ) - sizeof( u16_t ) ] ;
} UGA_ALIGN( 8 ) uga_tftp_ack_packet ;

typedef struct
{
        uga_tftp_opcode opcode ;
        uga_tftp_err   errcode ;
        uga_string      errmsg ;
} uga_tftp_err_packet ;


uga_tftp_packet uga_tftp_parse_packet ( uga_string_view raw_packet ) ;

uga_string uga_tftp_craft_rq_packet ( uga_tftp_opcode const opcode, uga_string_view filename, uga_tftp_mode const mode ) ;
uga_string uga_tftp_craft_ack_packet ( u16_t const blockno ) ;
uga_string uga_tftp_craft_err_packet ( uga_tftp_err const errcode ) ;
uga_string uga_tftp_craft_data_packet ( i64_t blockno, uga_string_view data ) ;


typedef enum
{
        UGA_TFTP_SRV_MAIN   ,
        UGA_TFTP_SRV_DAEMON ,
} uga_tftp_server_mode ;


typedef struct
{
        uga_tftp_server_mode mode ;
        uga_string_view      port ;
} uga_tftp_server_config ;

typedef struct
{
        i32_t                  id ;
        uga_tftp_server_mode mode ;
        uga_socket           sock ;
        uga_dl_list          reqs ; // uga_dl_list< uga_tftp_inflight_request >
} uga_tftp_server ;

typedef struct
{
        uga_tftp_server * server ;

        uga_socket   client ;
        uga_string filedata ;
        uga_string last_req ;
        i32_t     last_ackd ;
} uga_tftp_inflight_request ;


uga_tftp_server uga_tftp_server_create ( uga_tftp_server_config config ) ;
i32_t           uga_tftp_server_run    ( uga_tftp_server      * server ) ;


#endif // UGA_TFTP_H_
































