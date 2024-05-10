//
//
//      uganet
//      test_str.h
//

#ifndef UGA_TEST_STR_H_
#define UGA_TEST_STR_H_

#include <test.h>
#include <core/uga_str.h>
#include <core/uga_strview.h>

#include <string.h>


static inline void test_str ( void )
{
        // test create
        {
                uga_string str_0 = uga_str_create_0() ;

                test_log( "test::str::create_0", str_0.data == NULL && str_0.size == 0 && str_0.capacity == 0 ) ;

                uga_string str_1 = uga_str_create_1( 8 ) ;

                test_log( "test::str::create_1", str_1.data != NULL && str_1.size == 0 && str_1.capacity == 8 ) ;

                uga_string str_2 = uga_str_create_from_1( "cstring" ) ;

                test_log( "test::str::create_from_1", str_2.size == strlen( "cstring" ) ) ;

                uga_string str_3 = uga_str_create_from_2( "string", 6 ) ;

                test_log( "test::str::create_from_2", str_3.size == 6 ) ;

                uga_str_destroy( &str_0 ) ;
                uga_str_destroy( &str_1 ) ;
                uga_str_destroy( &str_2 ) ;
                uga_str_destroy( &str_3 ) ;
        }
        // test copy and move
        {
                uga_string src = uga_str_create_from_1( "string" ) ;

                uga_string copy = uga_str_copy( src ) ;

                test_log( "test::str::copy", uga_sv_equal( uga_sv_create_from( &src ),
                                                           uga_sv_create_from( &copy ) ) ) ;
                uga_string move = uga_str_move( &src ) ;

                test_log( "test::str::move", uga_str_empty( src ) ) ;
                test_log( "test::str::move", uga_sv_equal( uga_sv_create_from( &copy ),
                                                           uga_sv_create_from( &move ) ) ) ;
                uga_str_destroy( & src ) ;
                uga_str_destroy( &copy ) ;
                uga_str_destroy( &move ) ;
        }
        // test reserve
        {
                uga_string str = uga_str_create_from_1( "string" ) ;

                test_log( "test::str::reserve", str.capacity < 16 ) ;

                uga_str_reserve( &str, 16 ) ;

                test_log( "test::str::reserve", str.capacity == 16 ) ;

                uga_str_destroy( &str ) ;
        }
        // test append
        {
                uga_string str_0 = uga_str_create_0() ;
                uga_string str_1 = uga_str_create_from_1( "abcdefg" ) ;

                uga_str_append_cstr( &str_0, "abcdefg" ) ;

                test_log( "test::str::append_cstr", uga_sv_equal( uga_sv_create_from( &str_0 ),
                                                                  uga_sv_create_from( &str_1 ) ) ) ;
                uga_str_append( &str_1, "abc", 3 ) ;

                test_log( "test::str::append", str_1.size == 10 ) ;

                uga_str_destroy( &str_0 ) ;
                uga_str_destroy( &str_1 ) ;
        }
}


#endif // UGA_TEST_STR_H_
