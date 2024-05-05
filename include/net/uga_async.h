//
//
//      uganet
//      uga_async.h
//

#ifndef UGA_ASYNC_H_
#define UGA_ASYNC_H_

#include <core/uga_str.h>
#include <net/uga_sock.h>

#include <aio.h>

#define UGA_MAX_HANDLERS 32


typedef struct aiocb _aiocb_t ;

struct uga_callback_t
{
        uga_socket * sock ;
        uga_string * data ;
        bool ( *callback )( struct uga_callback_t * ) ;
} ;
typedef struct uga_callback_t uga_callback ;

typedef struct
{
        i32_t    num_handlers                     ;
        uga_callback handlers[ UGA_MAX_HANDLERS ] ;
} uga_handler_list ;


void uga_async_read ( uga_callback * callback ) ;


#endif // UGA_ASYNC_H_
