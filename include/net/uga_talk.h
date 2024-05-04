//
//
//      uganet
//      uga_talk.h
//

#ifndef UGA_TALK_H_
#define UGA_TALK_H_

#include <core/uga_types.h>
#include <net/uga_sock.h>


struct uga_string_t ;

void uga_send    ( uga_socket const * sock, void const * data, i64_t const datalen, i32_t const flags ) ;
void uga_send_to ( uga_socket const * sock, void const * data, i64_t const datalen, i32_t const flags ) ;

void uga_send_str    ( uga_socket const * sock, struct uga_string_t const * message, i32_t const flags ) ;
void uga_send_str_to ( uga_socket const * sock, struct uga_string_t const * message, i32_t const flags ) ;

i64_t uga_recv      ( uga_socket const * sock, void       * dest, i64_t const destlen, i32_t const flags ) ;
i64_t uga_recv_from ( uga_socket       * sock, void       * dest, i64_t const destlen, i32_t const flags ) ;

struct uga_string_t uga_recv_str      ( uga_socket const * sock, i32_t const flags ) ;
struct uga_string_t uga_recv_str_from ( uga_socket       * sock, i32_t const flags ) ;

struct uga_string_t uga_recv_strn      ( uga_socket const * sock, i64_t const maxlen, i32_t const flags ) ;
struct uga_string_t uga_recv_strn_from ( uga_socket       * sock, i64_t const maxlen, i32_t const flags ) ;


#endif // UGA_TALK_H_
