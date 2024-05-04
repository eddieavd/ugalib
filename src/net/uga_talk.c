//
//
//      uganet
//      uga_talk.c
//

#include <net/uga_talk.h>

#include <core/uga_log.h>
#include <core/uga_err.h>
#include <core/uga_fs.h>
#include <core/uga_str.h>

#define UGA_RECV_BUFF_SIZE 4096


void uga_send ( uga_socket const * sock, void const * data, i64_t const datalen, i32_t const flags )
{
        uga_clr_errs() ;

        i64_t bytes_sent = send( sock->sockfd, data, datalen, flags ) ;
        if( bytes_sent == -1 )
        {
                _uga_check_std_errno() ;
        }
        else if( bytes_sent < datalen )
        {
                UGA_WARN_S( "uga::talk::send", "partial write, wanted %db, sent %db", datalen, bytes_sent ) ;
                uga_set_io_error( UGA_ERR_PARTIAL_WRITE, datalen, bytes_sent ) ;
        }
}

void uga_send_to ( uga_socket const * sock, void const * data, i64_t const datalen, i32_t const flags )
{
        uga_clr_errs() ;

        i64_t bytes_sent = sendto( sock->sockfd, data, datalen, flags, ( _sockaddr_t * ) &sock->addr.addr, sock->addr.addrlen ) ;
        if( bytes_sent == -1 )
        {
                _uga_check_std_errno() ;
        }
        else if( bytes_sent < datalen )
        {
                UGA_WARN_S( "uga::talk::send_to", "partial write, wanted %db, sent %db", datalen, bytes_sent ) ;
                uga_set_io_error( UGA_ERR_PARTIAL_WRITE, datalen, bytes_sent ) ;
        }
}

void uga_send_str ( uga_socket const * sock, uga_string const * message, i32_t const flags )
{
        uga_send( sock, message->data, message->size, flags ) ;
}

void uga_send_str_to ( uga_socket const * sock, uga_string const * message, i32_t const flags )
{
        uga_send_to( sock, message->data, message->size, flags ) ;
}

i64_t uga_recv ( uga_socket const * sock, void * dest, i64_t const destlen, i32_t const flags )
{
        uga_clr_errs() ;

        i64_t bytes_recvd = recv( sock->sockfd, dest, destlen, flags ) ;
        if( bytes_recvd == -1 )
        {
                _uga_check_std_errno() ;
        }
        else if( bytes_recvd < destlen )
        {
                UGA_WARN_S( "uga::talk::recv", "partial read, wanted %db, got %db", destlen, bytes_recvd ) ;
                uga_set_io_error( UGA_ERR_PARTIAL_READ, destlen, bytes_recvd ) ;
        }
        return bytes_recvd ;
}

i64_t uga_recv_from ( uga_socket * sock, void * dest, i64_t const destlen, i32_t const flags )
{
        uga_clr_errs() ;

        i64_t bytes_recvd = recvfrom( sock->sockfd, dest, destlen, flags, ( _sockaddr_t * ) &sock->addr.addr, &sock->addr.addrlen ) ;
        if( bytes_recvd == -1 )
        {
                _uga_check_std_errno() ;
        }
        else if( bytes_recvd < destlen )
        {
                UGA_WARN_S( "uga::talk::recv_from", "partial read, wanted %db, got %db", destlen, bytes_recvd ) ;
                uga_set_io_error( UGA_ERR_PARTIAL_READ, destlen, bytes_recvd ) ;
        }
        return bytes_recvd ;
}

uga_string uga_recv_str ( uga_socket const * sock, i32_t const flags )
{
        uga_string buff = uga_str_create_1( UGA_RECV_BUFF_SIZE ) ;

        i64_t size = uga_recv( sock, buff.data, buff.capacity, flags ) ;
        buff.size = size ;

        if( uga_current_errtype() == UGA_ERR_PARTIAL_READ )
        {
                uga_str_shrink_to_fit( &buff ) ;
        }
        return buff ;
}

uga_string uga_recv_str_from ( uga_socket * sock, i32_t const flags )
{
        uga_string buff = uga_str_create_1( UGA_RECV_BUFF_SIZE ) ;

        i64_t size = uga_recv_from( sock, buff.data, buff.capacity, flags ) ;
        buff.size = size ;

        if( uga_current_errtype() == UGA_ERR_PARTIAL_READ )
        {
                uga_str_shrink_to_fit( &buff ) ;
        }
        return buff ;
}

uga_string uga_recv_strn ( uga_socket const * sock, i64_t const maxlen, i32_t const flags )
{
        uga_string buff = uga_str_create_1( maxlen ) ;

        i64_t size = uga_recv( sock, buff.data, buff.capacity, flags ) ;
        buff.size = size ;

        return buff ;
}

uga_string uga_recv_strn_from ( uga_socket * sock, i64_t const maxlen, i32_t const flags )
{
        uga_string buff = uga_str_create_1( maxlen ) ;

        i64_t size = uga_recv_from( sock, buff.data, buff.capacity, flags ) ;
        buff.size = size ;

        return buff ;
}
