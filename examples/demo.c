//
//
//      uganet
//      demo.c
//

#include <uganet.h>
#include <core/uga_vector.h>

#include <stdio.h>


int main ( void )
{
        uga_log_init () ;

        uga_vector vec = uga_vec_create_2( uga_string, 1, uga_str_destroy_void ) ;
        uga_print_abort_if_err() ;

        uga_string str = uga_str_create_from_1( "string" ) ;
        uga_print_abort_if_err() ;

        uga_vec_reserve( &vec, 10 ) ;

        for( i32_t i = 0; i < 10; ++i )
        {
                *( ( uga_string * ) _uga_vec_at( &vec, i ) ) = uga_str_copy( str ) ;
                ++vec.size ;
                uga_print_abort_if_err() ;
        }
        for( i32_t i = 0; i < vec.size; ++i )
        {
                uga_string * str = _uga_vec_at( &vec, i ) ;

                printf( STR_FMT"\n", STR_ARG( *str ) ) ;
        }
        uga_vec_destroy( &vec ) ;
        uga_str_destroy( &str ) ;

        vec = uga_vec_create_1( i32_t, 10 ) ;

        for( i32_t i = 0; i < 10; ++i )
        {
                uga_vec_push_back( &vec, &i ) ;
                uga_print_if_err() ;
        }
        for( i32_t i = 0; i < 10; ++i )
        {
                i32_t val ;
                uga_vec_at( &vec, i, &val ) ;
                printf( "%d\n", val ) ;
        }
        uga_vec_destroy( &vec ) ;


        return 0;
}
