//
//
//      uganet
//      mtex.c
//

#include <uganet.h>
#include <core/uga_vector.h>
#include <core/uga_thread.h>
#include <net/uga_tftp.h>

#include <string.h>

#define VALUE_COUNT_MAX 5


i32_t vals[ VALUE_COUNT_MAX ] ;
i32_t vals_count = 0 ;

uga_mtx_t vals_mtx ;
uga_cnd_t vals_cnd ;

i32_t work ( void * arg )
{
        ( void ) arg ;

        while( true )
        {
                uga_mtx_acquire( &vals_mtx ) ;
                uga_print_if_err() ;

                while( vals_count < VALUE_COUNT_MAX )
                {
                        if( vals_count == -1 )
                        {
                                UGA_DBG_S( "thrd", "quitting..." ) ;
                                goto done ;
                        }
                        else
                        {
                                UGA_DBG_S( "thrd", "waiting..." ) ;
                                uga_cnd_wait( &vals_cnd, &vals_mtx ) ;
                                uga_print_if_err() ;
                        }
                }
                UGA_DBG_S( "thrd", "woken up." ) ;

                i32_t sum = 0 ;

                for( i32_t i = 0; i < VALUE_COUNT_MAX; ++i )
                {
                        sum += vals[ i ] ;
                }
                UGA_DBG_S( "thrd", "sum : %d", sum ) ;

                vals_count = 0 ;

                uga_mtx_release( &vals_mtx ) ;
                uga_print_if_err() ;
        }
done:
        return 0 ;
}

int main ( void )
{
        UGA_DBG_S( "main", "creating thread..." ) ;

        uga_task   task   = { NULL, work } ;
        uga_thread thread = uga_thread_do_task( task ) ;
        uga_print_if_err() ;
        uga_thread_detach( thread ) ;
        uga_print_if_err() ;

        UGA_DBG_S( "main", "thread created." ) ;

        vals_mtx = uga_mtx_init( UGA_MTX_PLAIN ) ;
        uga_print_if_err() ;
        vals_cnd = uga_cnd_init() ;
        uga_print_if_err() ;

        UGA_DBG_S( "main", "initialized mtx and cnd" ) ;

        while( true )
        {
                UGA_DBG_S( "main", "provide value:" ) ;

                i32_t n ;
                scanf( "%d", &n ) ;

                uga_mtx_acquire( &vals_mtx ) ;
                uga_print_if_err() ;

                if( n == 0 )
                {
                        UGA_DBG_S( "main", "quitting..." ) ;
                        vals_count = -1 ;
                        uga_cnd_ping( &vals_cnd ) ;
                        uga_print_if_err() ;
                        goto done ;
                }
                vals[ vals_count++ ] = n ;

                if( vals_count == VALUE_COUNT_MAX )
                {
                        UGA_DBG_S( "main", "signaling threads..." ) ;
                        uga_cnd_ping( &vals_cnd ) ;
                        uga_print_if_err() ;
                }
                uga_mtx_release( &vals_mtx ) ;
                uga_print_if_err() ;
        }
done:
        uga_mtx_destroy( &vals_mtx ) ;
        uga_cnd_destroy( &vals_cnd ) ;

        return 0;
}
