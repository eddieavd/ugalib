//
//
//      uganet
//      string.c
//

#include <core/uga_log.h>
#include <core/uga_str.h>

#include <stdio.h>


int main ( void )
{
        uga_log_init();

        uga_string str = uga_str_create_1( 6 );

        uga_str_append( &str, "hello!", 6 );

        printf( STR_FMT"\n", STR_ARG( str ) );

        uga_str_append( &str, "!!", 2 );

        printf( STR_FMT"\n", STR_ARG( str ) );

        uga_str_destroy( &str );




        return 0;
}
