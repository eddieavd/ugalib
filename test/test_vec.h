//
//
//      uganet
//      test_vec.h
//

#ifndef UGA_TEST_VEC_H_
#define UGA_TEST_VEC_H_

#include <test.h>
#include <core/uga_vector.h>
#include <core/uga_str.h>


static inline bool test_vectors_equal ( uga_vector const * lhs, uga_vector const * rhs )
{
        if( lhs->size != rhs->size )
        {
                return false ;
        }
        for( i64_t i = 0; i < lhs->size * lhs->elem_size; ++i )
        {
                u8_t const * lhs_data = ( ( u8_t const * ) lhs->data ) + i ;
                u8_t const * rhs_data = ( ( u8_t const * ) rhs->data ) + i ;

                if( *lhs_data != *rhs_data ) return false ;
        }
        return true ;
}

static inline void test_vec ( void )
{
        // test create
        {
                uga_vector vec_0 = uga_vec_create_0( i32_t ) ;

                if( !test_log( "test::vec::create_0", vec_0.data == NULL && vec_0.size == 0 && vec_0.capacity == 0 ) )
                {
                        UGA_ERR_S( "test::vec::create_0",
                                   "vec size is %d, expected 0; vec capacity is %d, expected 0",
                                   vec_0.size, vec_0.capacity
                        ) ;
                }
                uga_vector vec_1 = uga_vec_create_1( i32_t, 8 ) ;

                if( !test_log( "test::vec::create_1", vec_1.data != NULL && vec_1.size == 0 && vec_1.capacity == 8 ) )
                {
                        UGA_ERR_S( "test::vec::create_1",
                                   "vec size is %d, expected 0; vec capacity is %d, expected 8",
                                   vec_1.size, vec_1.capacity
                        ) ;
                }
                uga_vector vec_2 = uga_vec_create_d_1( uga_string, 8, uga_str_destroy_void ) ;

                if( !test_log( "test::vec::create_2", vec_2.data != NULL && vec_2.size == 0 && vec_2.capacity == 8 ) )
                {
                        UGA_ERR_S( "test::vec::create_2",
                                   "vec size is %d, expected 0; vec capacity is %d, expected 8",
                                   vec_2.size, vec_2.capacity
                        ) ;
                }
                uga_vec_destroy( &vec_0 ) ;
                uga_vec_destroy( &vec_1 ) ;
                uga_vec_destroy( &vec_2 ) ;
        }
        // test reserve
        {
                uga_vector vec = uga_vec_create_1( i32_t, 8 ) ;

                test_log( "test::vec::reserve", vec.capacity == 8 ) ;

                uga_vec_reserve( &vec, 16 ) ;

                if( !test_log( "test::vec::reserve", vec.capacity == 16 ) )
                {
                        UGA_ERR_S( "test::vec::reserve",
                                   "vec capacity is %d, expected 16",
                                   vec.capacity
                        ) ;
                }
                uga_vec_destroy( &vec ) ;
        }
        // test push back
        {
                uga_vector vec = uga_vec_create_1( i32_t, 8 ) ;

                for( i32_t i = 0; i < 8; ++i )
                {
                        uga_vec_push_back( &vec, &i ) ;
                }
                if( !test_log( "test::vec::push_back", vec.capacity >= 8 && vec.size == 8 ) )
                {
                        UGA_ERR_S( "test::vec::push_back",
                                   "vec capacity is %d, expected 8; vec size is %d, expected 8",
                                   vec.capacity, vec.size
                        ) ;
                }
                for( i32_t i = 0; i < 8; ++i )
                {
                        uga_vec_push_back( &vec, &i ) ;
                }
                if( !test_log( "test::vec::push_back", vec.capacity > 8 && vec.size == 16 ) )
                {
                        UGA_ERR_S( "test::vec::push_back",
                                   "vec capacity is %d, expected >8; vec size is %d, expected 16",
                                   vec.capacity, vec.size
                        ) ;
                }
        }
        // test copy and move
        {
                uga_vector vec = uga_vec_create_1( i32_t, 8 ) ;

                for( i32_t i = 0; i < vec.size; ++i )
                {
                        uga_vec_push_back( &vec, &i ) ;
                }
                uga_vector copy = uga_vec_copy( &vec ) ;

                if( !test_log( "test::vec::copy", test_vectors_equal( &vec, &copy ) ) )
                {
                        UGA_ERR_S( "test::vec::copy", "copied vector not equal to source vector" ) ;
                }
                uga_vector move = uga_vec_move( &vec ) ;

                if( !test_log( "test::vec::move", uga_vec_empty( &vec ) ) )
                {
                        UGA_ERR_S( "test::vec::move", "moved from vector is not empty" ) ;
                }
                if( !test_log( "test::vec::move", test_vectors_equal( &copy, &move ) ) )
                {
                        UGA_ERR_S( "test::vec::move", "moved to vector not equal to source" ) ;
                }
                uga_vec_destroy( & vec ) ;
                uga_vec_destroy( &copy ) ;
                uga_vec_destroy( &move ) ;
        }
}


#endif // UGA_TEST_VEC_H_
