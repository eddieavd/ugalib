//
//
//      uganet
//      uga_async.c
//

#include <net/uga_async.h>
#include <core/uga_err.h>

#define UGA_BUFFLEN 4096


void uga_async_read ( uga_callback * callback )
{
        uga_clr_errs() ;

        _aiocb_t aio = { 0 } ;

        aio.aio_fildes = callback->sock.sockfd   ;
        aio.aio_buf    = callback->data.data     ;
        aio.aio_nbytes = callback->data.capacity ;

        if( aio_read( &aio ) == -1 )
        {
                _uga_check_std_errno() ;
        }
}


#undef UGA_BUFFLEN
