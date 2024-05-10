//
//
//      uganet
//      uga_vector.c
//

#include <core/uga_vector.h>
#include <core/uga_alloc.h>
#include <core/uga_err.h>

#include <string.h>


uga_vector _uga_vec_create_0 ( i64_t elem_size )
{
        uga_vector vec = { 0 } ;
        vec.elem_size = elem_size ;
        return vec ;
}

uga_vector _uga_vec_create_1 ( i64_t capacity, i64_t elem_size )
{
        uga_vector vec = _uga_vec_create_0( elem_size ) ;

        vec.data = uga_allocate( capacity * vec.elem_size ) ;
        if( uga_had_errs() ) return vec ;

        vec.capacity = capacity ;
        return vec ;
}

uga_vector _uga_vec_create_2 ( i64_t capacity, void ( *dtor )( void * ), i64_t elem_size )
{
        uga_vector vec = _uga_vec_create_1( capacity, elem_size ) ;

        if( uga_had_errs() ) return vec ;

        vec.elem_dtor = dtor ;

        return vec ;
}

uga_vector uga_vec_copy ( uga_vector const * other )
{
        uga_vector vec = _uga_vec_create_1( other->size, other->elem_size ) ;

        if( uga_had_errs() ) return _uga_vec_create_0( other->elem_size ) ;

        memcpy( vec.data, other->data, other->size * other->elem_size ) ;
        vec.capacity = vec.size = other->size ;

        return vec ;
}

uga_vector uga_vec_move ( uga_vector * other )
{
        uga_vector vec = _uga_vec_create_0( other->elem_size ) ;

        vec.data = other->data ;
        vec.size = other->size ;
        vec.capacity = other->capacity ;

        other->data     = NULL ;
        other->size     =    0 ;
        other->capacity =    0 ;

        return vec ;
}

bool uga_vec_empty ( uga_vector const * this )
{
        return !this->data || this->size == 0 ;
}

i64_t uga_vec_space_left ( uga_vector const * this )
{
        return this->capacity - this->size ;
}

void * uga_vec_at_ptr ( uga_vector * this, i64_t const index )
{
        /*
        if( index < 0 || index >= this->size )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, this->size, index ) ;
                return NULL ;
        }
        */
        return ( char * ) this->data + ( index * this->elem_size ) ;
}

void uga_vec_at ( uga_vector * this, i64_t const index, void * dest )
{
        memcpy( dest, uga_vec_at_ptr( this, index ), this->elem_size ) ;
}

void uga_vec_reserve ( uga_vector * this, i64_t const capacity )
{
        if( capacity <= this->capacity ) return ;

        if( !this->data )
        {
                *this = _uga_vec_create_1( capacity, this->elem_size ) ;
                return ;
        }
        uga_reallocate( &this->data, capacity * this->elem_size ) ;

        if( uga_had_errs() ) return ;

        this->capacity = capacity ;
}

void uga_vec_shrink_to_fit ( uga_vector * this )
{
        if( this->size >= this->capacity ) return ;

        void * new_block = uga_allocate( this->size * this->elem_size ) ;
        if( uga_had_errs() ) return ;

        memcpy( new_block, this->data, this->size * this->elem_size ) ;

        uga_deallocate( this->data ) ;

        this->data     =  new_block ;
        this->capacity = this->size ;
}

void uga_vec_push_back ( uga_vector * this, void * value )
{
        if( uga_vec_empty( this ) || uga_vec_space_left( this ) < this->elem_size )
        {
                uga_vec_reserve( this, this->capacity == 0 ? 1 : this->capacity * 2 ) ;
        }
        memcpy( ( u8_t * ) this->data + ( this->size * this->elem_size ), value, this->elem_size ) ;
        ++this->size ;
}

void uga_vec_clear ( uga_vector * this )
{
        memset( this->data, 0, this->capacity * this->elem_size ) ;
        this->size = 0 ;
}

void uga_vec_destroy ( uga_vector * this )
{
        if( this->data )
        {
                if( this->elem_dtor )
                {
                        for( i64_t i = 0; i < this->size; ++i )
                        {
                                this->elem_dtor( ( char * ) this->data + ( i * this->elem_size ) ) ;
                        }
                }
                uga_deallocate( this->data ) ;
        }
        *this = _uga_vec_create_0( this->elem_size ) ;
}
