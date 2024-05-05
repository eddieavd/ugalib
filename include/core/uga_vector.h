//
//
//      uganet
//      uga_vector.h
//

#ifndef UGA_VECTOR_H_
#define UGA_VECTOR_H_

#include <core/uga_types.h>

#define uga_vec_create_0( type            ) _uga_vec_create_0(            sizeof( type ) )
#define uga_vec_create_1( type, cap       ) _uga_vec_create_1( cap,       sizeof( type ) )
#define uga_vec_create_2( type, cap, dtor ) _uga_vec_create_2( cap, dtor, sizeof( type ) )


struct uga_vector_t
{
        i64_t       size ;
        i64_t   capacity ;
        i64_t  elem_size ;
        void *      data ;

        void ( *elem_dtor )( void * elem ) ;
} ;
typedef struct uga_vector_t uga_vector ;

uga_vector _uga_vec_create_0 (                                           i64_t elem_size ) ;
uga_vector _uga_vec_create_1 ( i64_t capacity,                           i64_t elem_size ) ;
uga_vector _uga_vec_create_2 ( i64_t capacity, void ( *dtor )( void * ), i64_t elem_size ) ;

uga_vector uga_vec_copy ( uga_vector const * other ) ;
uga_vector uga_vec_move ( uga_vector       * other ) ;

bool uga_vec_empty ( uga_vector const * this ) ;

i64_t uga_vec_space_left ( uga_vector const * this ) ;

void * _uga_vec_at ( uga_vector * this, i64_t const index ) ;

void uga_vec_at ( uga_vector * this, i64_t const index, void * dest ) ;

void uga_vec_reserve ( uga_vector * this, i64_t const capacity ) ;

void uga_vec_shrink_to_fit ( uga_vector * this ) ;

void uga_vec_push_back ( uga_vector * this, void * value ) ;

void uga_vec_clear   ( uga_vector * this ) ;
void uga_vec_destroy ( uga_vector * this ) ;


#endif // UGA_VECTOR_H_
