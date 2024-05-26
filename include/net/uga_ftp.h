//
//
//      uganet
//      uga_ftp.h
//

#ifndef UGA_FTP_H_
#define UGA_FTP_H_

#include <core/uga_types.h>
#include <core/uga_util.h>
#include <core/uga_str.h>
#include <core/uga_list.h>
#include <core/uga_thread.h>
#include <core/uga_pool.h>
#include <net/uga_sock.h>


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


typedef struct
{
        u16_t opcode ;
        u8_t data[ 514 ] ;
} UGA_ALIGN( 8 ) uga_tftp_packet_raw ;

typedef struct
{
        uga_tftp_opcode opcode ;
        u8_t data[ 32 - sizeof( uga_tftp_opcode ) ] ;
} UGA_ALIGN( 8 ) uga_tftp_packet ;

typedef struct
{
        uga_tftp_opcode   opcode ;
        uga_tftp_mode       mode ;
        uga_string      filename ;
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
        u8_t pad[ 32 - 8 ] ;
} UGA_ALIGN( 8 ) uga_tftp_ack_packet ;

typedef struct
{
        uga_tftp_opcode opcode ;
        uga_tftp_err   errcode ;
        uga_string      errmsg ;
} uga_tftp_err_packet ;


uga_tftp_packet uga_tftp_parse_packet ( uga_string_view packet_data ) ;

uga_tftp_packet_raw uga_tftp_craft_rq_packet ( uga_tftp_opcode const opcode, uga_string_view filename, uga_tftp_mode const mode ) ;
uga_tftp_packet_raw uga_tftp_craft_ack_packet ( i32_t const blockno ) ;
uga_tftp_packet_raw uga_tftp_craft_data_packet ( i32_t const blockno, uga_string_view data ) ;
uga_tftp_packet_raw uga_tftp_craft_err_packet ( uga_tftp_err const errcode ) ;

void uga_tftp_send_err_packet ( uga_socket * client, uga_tftp_err const   errcode ) ;
void uga_tftp_send_packet     ( uga_socket * client, uga_tftp_packet_raw * packet ) ;

void uga_tftp_packet_destroy      ( uga_tftp_packet * packet ) ;
void uga_tftp_packet_destroy_void ( void            * packet ) ;


typedef struct
{
        uga_socket        sock ;
        uga_thread_pool * pool ;
        uga_dl_list       reqs ;
        uga_mtx_t         rmtx ;
} uga_tftp_server ;

typedef struct
{
        uga_tftp_server * server ;
        uga_socket        client ;
        uga_string      filedata ;
        uga_tftp_packet last_req ;
        i32_t           last_ack ;
} uga_tftp_inflight ;

uga_tftp_server uga_tftp_server_create ( uga_string_view port ) ;

i32_t uga_tftp_server_run ( uga_tftp_server * server ) ;

i32_t _uga_tftp_server_run_task ( void * server_vec ) ;

void uga_tftp_server_destroy      ( uga_tftp_server * server ) ;
void uga_tftp_server_destroy_void ( void            * server ) ;


#endif // UGA_FTP_H_
