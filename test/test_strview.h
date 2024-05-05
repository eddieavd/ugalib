//
//
//      uganet
//      test_strview.h
//

#ifndef UGA_TEST_STRVIEW_H_
#define UGA_TEST_STRVIEW_H_

#include <test.h>
#include <core/uga_strview.h>

#include <stdio.h>
#include <string.h>


static inline void test_strview ( void )
{
        // test create
        {
                uga_string_view view_0 = uga_sv_create_0() ;

                test_log( "test::strview::create_0", view_0.data == NULL && view_0.size == 0 ) ;

                uga_string_view view_1 = uga_sv_create_1( "cstring1" ) ;

                test_log( "test::strview::create_1", view_1.data != NULL && view_1.size == strlen( "cstring1" ) ) ;

                char const * cstr = "cstring2" ;

                uga_string_view view_2 = uga_sv_create_2( cstr, strlen( cstr ) ) ;

                test_log( "test::strview::create_2", view_2.data == cstr && ( u64_t ) view_2.size == strlen( cstr ) ) ;
        }
        // test comparisons
        {
                uga_string_view view_0 = uga_sv_create_1( "abcdefg" ) ;
                uga_string_view view_1 = uga_sv_create_1( "abcdefg" ) ;
                uga_string_view view_2 = uga_sv_create_1( "bcdefga" ) ;

                uga_string_view prefix_0 = uga_sv_create_1( "abc" ) ;
                uga_string_view prefix_1 = uga_sv_create_1( "bcd" ) ;

                uga_string_view suffix_0 = uga_sv_create_1( "efg" ) ;
                uga_string_view suffix_1 = uga_sv_create_1( "def" ) ;

                test_log( "test::strview::equal",  uga_sv_equal( view_0, view_1 ) ) ;
                test_log( "test::strview::equal", !uga_sv_equal( view_0, view_2 ) ) ;

                test_log( "test::strview::starts_with",  uga_sv_starts_with( view_0, prefix_0 ) ) ;
                test_log( "test::strview::starts_with", !uga_sv_starts_with( view_0, prefix_1 ) ) ;

                test_log( "test::strview::ends_with",  uga_sv_ends_with( view_0, suffix_0 ) ) ;
                test_log( "test::strview::ends_with", !uga_sv_ends_with( view_0, suffix_1 ) ) ;
        }
        // test trim
        {
                uga_string_view view_0 = uga_sv_create_1( "  abc " ) ;
                uga_string_view view_1 = uga_sv_create_1(   "abc"  ) ;

                uga_string_view trimmed = uga_sv_trimmed( view_0 ) ;

                test_log( "test::strview::trimmed", uga_sv_equal( trimmed, view_1 ) ) ;

                uga_sv_trim( &view_0 ) ;

                test_log( "test::strview::trim", uga_sv_equal( view_0, view_1 ) ) ;
        }
        // test substr
        {
                uga_string_view view_0 = uga_sv_create_1( "somestringdata" ) ;
                uga_string_view view_1 = uga_sv_create_1(     "string"     ) ;

                uga_string_view sub = uga_sv_substr( view_0, 4, 6 ) ;

                test_log( "test::strview::substr", uga_sv_equal( view_1, sub ) ) ;
        }
        // test chop and unchop
        {
                uga_string_view view_0 = uga_sv_create_1( "abcdefg" ) ;
                uga_string_view view_1 = uga_sv_create_1( "abcdefg" ) ;
                uga_string_view view_2 = uga_sv_create_1( "abc"     ) ;
                uga_string_view view_3 = uga_sv_create_1(    "defg" ) ;

                uga_string_view chop = uga_sv_chop_left( &view_0, 3 ) ;

                test_log( "test::strview::chop", uga_sv_equal( chop  , view_2 ) ) ;
                test_log( "test::strview::chop", uga_sv_equal( view_0, view_3 ) ) ;

                uga_sv_unchop_left( &view_0, 3 ) ;

                test_log( "test::strview::unchop", uga_sv_equal( view_0, view_1 ) ) ;

                uga_string_view view_4 = uga_sv_create_1( "int a; char b;" ) ;
                uga_string_view view_5 = uga_sv_create_1( "int a"          ) ;

                uga_string_view expr = uga_sv_chop_to_delimiter( &view_4, ';', true ) ;

                test_log( "test::strview::chop_to_delimiter", uga_sv_equal( expr, view_5 ) ) ;

                uga_string_view view_6 = uga_sv_create_1( "abcdefg" ) ;

                uga_string_view chop_1 = uga_sv_chop_to_delimiter( &view_6, ';', true ) ;

                test_log( "test::strview::chop_to_delimiter", uga_sv_equal( chop_1, view_1 ) ) ;
        }
        // test contains, index_of, count
        {
                uga_string_view view_0 = uga_sv_create_1( "abcdabcdab " ) ;

                test_log( "test::strview::contains",  uga_sv_contains( view_0,  'a' ) ) ;
                test_log( "test::strview::contains",  uga_sv_contains( view_0,  'd' ) ) ;
                test_log( "test::strview::contains",  uga_sv_contains( view_0,  ' ' ) ) ;
                test_log( "test::strview::contains", !uga_sv_contains( view_0,  'e' ) ) ;
                test_log( "test::strview::contains", !uga_sv_contains( view_0, '\t' ) ) ;

                test_log( "test::strview::index_of", uga_sv_index_of( view_0, 'c' ) == 2 ) ;
                test_log( "test::strview::index_of", uga_sv_index_of( view_0, ' ' ) == view_0.size - 1 ) ;
                test_log( "test::strview::index_of", uga_sv_index_of( view_0, 'z' ) == view_0.size ) ;

                test_log( "test::strview::count", uga_sv_count( view_0, 'a' ) == 3 ) ;
                test_log( "test::strview::count", uga_sv_count( view_0, 'd' ) == 2 ) ;
                test_log( "test::strview::count", uga_sv_count( view_0, 'z' ) == 0 ) ;
        }
        // test parse
        {
                uga_string_view view_0 = uga_sv_create_1(  "1234" ) ;
                uga_string_view view_1 = uga_sv_create_1( "+1234" ) ;
                uga_string_view view_2 = uga_sv_create_1( "-1234" ) ;

                i64_t val_pos =  1234 ;
                i64_t val_neg = -1234 ;

                test_log( "test::strview::parse_int", uga_sv_parse_int( view_0 ) == val_pos ) ;
                test_log( "test::strview::parse_int", uga_sv_parse_int( view_1 ) == val_pos ) ;
                test_log( "test::strview::parse_int", uga_sv_parse_int( view_2 ) == val_neg ) ;
        }
        // test access
        {
                uga_string_view view_e = uga_sv_create_0() ;
                uga_string_view view_0 = uga_sv_create_1( "abcdefgabcdefg" ) ;

                test_log( "test::strview::empty",  uga_sv_empty( view_e ) ) ;
                test_log( "test::strview::empty", !uga_sv_empty( view_0 ) ) ;

                test_log( "test::strview::at", uga_sv_at( view_0, 0 ) == 'a' ) ;
                test_log( "test::strview::at", uga_sv_at( view_0, 4 ) == 'e' ) ;
                test_log( "test::strview::at", uga_sv_at( view_0, view_0.size - 1 ) == 'g' ) ;

                test_log( "test::strview::front", uga_sv_front( view_0 ) == 'a' ) ;

                test_log( "test::strview::back", uga_sv_back( view_0 ) == 'g' ) ;
        }
}

#endif // UGA_TEST_STRVIEW_H_
