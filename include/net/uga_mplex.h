//
//
//      uganet
//      uga_mplex.h
//

#ifndef UGA_MPLEX_H_
#define UGA_MPLEX_H_

#include <core/uga_types.h>
#include <net/uga_sock.h>
#include <net/uga_async.h>

#define UGA_MAX_HANDLERS 32


void uga_mplex_add_handler    ( uga_handler_list * handler_list, uga_callback const * callback ) ;
void uga_mplex_remove_handler ( uga_handler_list * handler_list, uga_socket   const *   socket ) ;

i32_t uga_mplex_run ( uga_handler_list * handler_list ) ;


#endif // UGA_MPLEX_H_
