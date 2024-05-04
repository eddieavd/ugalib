//
//
//      uganet
//      uga_callback.h
//

#ifndef UGA_CALLBACK_H_
#define UGA_CALLBACK_H_

#include <core/uga_types.h>

#include <aio.h>


struct aiocb cb ;

struct fd_callback_t
{
        i32_t filedesc ;
        void ( *callback )( struct fd_callback_t * cb ) ;
} ;
typedef struct fd_callback_t fd_callback ;

typedef struct
{
        fd_callback callbacks[ 32 ] ;
} fdcb_list ;

struct mplex
{
        fdcb_list cb_list ;
} ;


#endif // UGA_CALLBACK_H_
