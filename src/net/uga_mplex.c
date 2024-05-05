//
//
//      uganet
//      uga_mplex.c
//

#include <net/uga_mplex.h>
#include <net/uga_talk.h>
#include <core/uga_err.h>

#include <unistd.h>
#include <fcntl.h>
#include <time.h> // for NULL


void uga_mplex_add_handler ( uga_handler_list * handler_list, uga_callback const * callback )
{
        if( handler_list->num_handlers >= UGA_MAX_HANDLERS )
        {
                // set error
                return ;
        }
        handler_list->handlers[ handler_list->num_handlers++ ] = *callback ;
}

void uga_mplex_remove_handler ( uga_handler_list * handler_list, uga_socket const * socket )
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

i32_t uga_mplex_run ( uga_handler_list * handler_list )
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
