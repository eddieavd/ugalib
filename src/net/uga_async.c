//
//
//      uganet
//      uga_async.c
//

#include <net/uga_async.h>
#include <core/uga_err.h>

#include <time.h>

#define UGA_BUFFLEN 4096


void uga_async_read ( uga_callback * callback )
{
        uga_clr_errs() ;

        _aiocb_t aio = { 0 } ;

        aio.aio_fildes = callback->sock->sockfd   ;
        aio.aio_buf    = callback->data->data     ;
        aio.aio_nbytes = callback->data->capacity ;

        if( aio_read( &aio ) == -1 )
        {
                _uga_check_std_errno() ;
        }
}

void uga_async_add_handler ( uga_handler_list * handler_list, uga_callback const * callback )
{
        uga_clr_errs() ;

        if( handler_list->num_handlers >= UGA_MAX_HANDLERS )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, UGA_MAX_HANDLERS, UGA_MAX_HANDLERS ) ;
                return ;
        }
        handler_list->handlers[ handler_list->num_handlers++ ] = *callback ;
}

void uga_async_remove_handler ( uga_handler_list * handler_list, uga_socket const * socket )
{
        for( i32_t i = 0; i < handler_list->num_handlers; ++i )
        {
                if( handler_list->handlers[ i ].sock->sockfd == socket->sockfd )
                {
                        handler_list->handlers[ i ] = handler_list->handlers[ handler_list->num_handlers - 1 ] ;
                        --handler_list->num_handlers ;
                        return ;
                }
        }
}

uga_socket uga_async_has_ready ( uga_handler_list * handler_list )
{
        fd_set fds, fdr ;
        FD_ZERO( &fds ) ;

        i32_t max_fd = 0 ;

        for( i32_t i = 0; i < handler_list->num_handlers; ++i )
        {
                i32_t fd = handler_list->handlers[ i ].sock->sockfd ;

                FD_SET( fd, &fds ) ;
                if( fd > max_fd ) max_fd = fd ;
        }
        struct timeval time = { 0, 0 } ;
        if( fdr = fds, select( max_fd + 1, &fdr, NULL, NULL, &time ) > 0 )
        {
                for( i32_t i = 0; i < handler_list->num_handlers; ++i )
                {
                        if( FD_ISSET( handler_list->handlers[ i ].sock->sockfd, &fdr ) )
                        {
                                UGA_WARN_S( "uga::async::has_ready", "socket %d ready", handler_list->handlers[ i ].sock->sockfd ) ;
                                return *handler_list->handlers[ i ].sock ;
                        }
                }
        }
        uga_socket empty = { 0 } ;
        empty.sockfd = -1 ;

        return empty ;
}

i32_t uga_async_run ( uga_handler_list * handler_list )
{
        fd_set fds, fdr ;
        FD_ZERO( &fds ) ;

        i32_t max_fd = 0 ;

        for( i32_t i = 0; i < handler_list->num_handlers; ++i )
        {
                i32_t fd = handler_list->handlers[ i ].sock->sockfd ;

                FD_SET( fd, &fds ) ;
                if( fd > max_fd ) max_fd = fd ;
        }
        while( fdr = fds, select( max_fd + 1, &fdr, NULL, NULL, NULL ) > 0 )
        {
                for( i32_t i = 0; i < handler_list->num_handlers; ++i )
                {
                        if( FD_ISSET( handler_list->handlers[ i ].sock->sockfd, &fdr ) )
                        {
                                if( !handler_list->handlers[ i ].callback( &handler_list->handlers[ i ] ) )
                                {
                                        return 1 ;
                                }
                        }
                }
        }
        return 0 ;
}

i32_t uga_async_run_on_pool ( uga_handler_list * handler_list, uga_thread_pool * pool )
{
        fd_set fds, fdr ;
        FD_ZERO( &fds ) ;

        i32_t max_fd = 0 ;

        for( i32_t i = 0; i < handler_list->num_handlers; ++i )
        {
                i32_t fd = handler_list->handlers[ i ].sock->sockfd ;

                FD_SET( fd, &fds ) ;
                if( fd > max_fd ) max_fd = fd ;
        }
        while( fdr = fds, select( max_fd + 1, &fdr, NULL, NULL, NULL ) > 0 )
        {
                for( i32_t i = 0; i < handler_list->num_handlers; ++i )
                {
                        if( FD_ISSET( handler_list->handlers[ i ].sock->sockfd, &fdr ) )
                        {
                                uga_vector task_data = uga_vec_create_1( uga_callback, 1 ) ;
                                uga_vec_push_back( &task_data, &handler_list->handlers[ i ] ) ;

                                uga_task task = { task_data, handler_list->handlers[ i ].callback } ;
                                uga_pool_add_task( pool, task ) ;

                                if( !handler_list->handlers[ i ].callback( &handler_list->handlers[ i ] ) )
                                {
                                        return 1 ;
                                }
                        }
                }
        }
        return 0 ;
}


#undef UGA_BUFFLEN
