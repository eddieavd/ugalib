//
//
//      uganet
//      uga_sl_list.c
//

#include <core/uga_list.h>
#include <core/uga_err.h>
#include <core/uga_alloc.h>
#include <core/uga_vector.h>

#include <string.h>


uga_sl_node * _uga_sl_node_create      ( void ( *dtor )( void * ), i64_t const  elem_size              ) ;
uga_sl_node * _uga_sl_node_create_with ( void ( *dtor )( void * ), i64_t const  elem_size, void * data ) ;

void _uga_sl_node_destroy ( uga_sl_node * node ) ;

uga_sl_list _uga_sl_list_create ( i64_t const elem_size )
{
        uga_sl_list list =
        {
                .head      =      NULL ,
                .size      =         0 ,
                .elem_size = elem_size ,
        } ;
        return list ;
}

uga_sl_list _uga_sl_list_create_d ( void ( *dtor )( void * ), i64_t const elem_size )
{
        uga_sl_list list = _uga_sl_list_create( elem_size ) ;
        list.elem_dtor = dtor ;

        return list ;
}

uga_sl_list uga_sl_list_from_vector ( uga_vector const * vector )
{
        uga_sl_list list = _uga_sl_list_create_d( vector->elem_dtor, vector->elem_size ) ;

        for( i64_t i = vector->size - 1; i >= 0; --i )
        {
                uga_sl_list_push_front( &list, uga_vec_at( vector, i ) ) ;
        }
        return list ;
}

void uga_sl_list_push_back ( uga_sl_list * this, void * data )
{
        if( this->size == 0 )
        {
                this->head = _uga_sl_node_create_with( this->elem_dtor, this->elem_size, data ) ;
        }
        else
        {
                uga_sl_node * last = this->head ;

                while( last->next != NULL )
                {
                        last = last->next ;
                }
                last->next = _uga_sl_node_create_with( this->elem_dtor, this->elem_size, data ) ;
        }
        ++this->size ;
}

void uga_sl_list_push_front ( uga_sl_list * this, void * data )
{
        if( this->size == 0 )
        {
                this->head = _uga_sl_node_create_with( this->elem_dtor, this->elem_size, data ) ;
        }
        else
        {
                uga_sl_node * new_front = _uga_sl_node_create_with( this->elem_dtor, this->elem_size, data ) ;

                new_front->next = this->head ;
                this->head      =  new_front ;
        }
        ++this->size ;
}

void * uga_sl_list_peek_back ( uga_sl_list * this )
{
        uga_clr_errs() ;

        if( this->size == 0 )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, 0, 0 ) ;
                return NULL ;
        }
        uga_sl_node * node = this->head ;

        while( node->next != NULL )
        {
                node = node->next ;
        }
        return node->data ;
}

void * uga_sl_list_peek_front ( uga_sl_list * this )
{
        if( this->size == 0 )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, 0, 0 ) ;
                return NULL ;
        }
        return this->head->data ;
}

void uga_sl_list_pop_back ( uga_sl_list * this, void * dest )
{
        uga_clr_errs() ;

        if( this->size == 0 )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, 0, 0 ) ;
                return ;
        }
        uga_sl_node * node = this->head ;

        if( this->size == 1 )
        {
                if( dest )
                {
                        memcpy( dest, node->data, this->elem_size ) ;
                }
                _uga_sl_node_destroy( node ) ;

                this->size =    0 ;
                this->head = NULL ;

                return ;
        }
        while( node->next->next != NULL )
        {
                node = node->next ;
        }
        uga_sl_node * last = node->next ;

        if( dest )
        {
                memcpy( dest, last->data, this->elem_size ) ;
        }
        _uga_sl_node_destroy( last ) ;

        node->next = NULL ;
        --this->size ;
}

void uga_sl_list_pop_front ( uga_sl_list * this, void * dest )
{
        uga_clr_errs() ;

        if( this->size == 0 )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, 0, 0 ) ;
                return ;
        }
        uga_sl_node * node = this->head ;

        if( dest )
        {
                memcpy( dest, node->data, this->elem_size ) ;
        }
        this->head = node->next ;
        --this->size ;
        _uga_sl_node_destroy( node ) ;
}

void * uga_sl_list_node_at ( uga_sl_list * this, i64_t const index )
{
        uga_clr_errs() ;

        if( index < 0 || index >= this->size )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, this->size, index ) ;
                return NULL ;
        }
        i64_t          pos =          0 ;
        uga_sl_node * node = this->head ;

        while( pos != index )
        {
                node = node->next ;
                ++pos ;
        }
        return node ;
}

void * uga_sl_list_at ( uga_sl_list * this, i64_t const index )
{
        uga_sl_node * node = uga_sl_list_node_at( this, index ) ;
        UGA_RETURN_ON_ERR( NULL ) ;

        return node->data ;
}

void uga_sl_list_remove_at ( uga_sl_list * this, i64_t const index, void * dest )
{
        uga_clr_errs() ;

        if( index < 0 || index >= this->size )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, this->size, index ) ;
                return ;
        }
        if( index == 0 )
        {
                uga_sl_list_pop_front( this, dest ) ;
                return ;
        }
        if( index == this->size - 1 )
        {
                uga_sl_list_pop_back( this, dest ) ;
                return ;
        }
        i64_t          pos =          0 ;
        uga_sl_node * node = this->head ;

        while( pos + 1 != index )
        {
                node = node->next ;
                ++pos ;
        }
        uga_sl_node * to_remove = node->next ;

        if( dest )
        {
                memcpy( dest, to_remove->data, this->elem_size ) ;
        }
        node->next = to_remove->next ;

        _uga_sl_node_destroy( to_remove ) ;
        --this->size ;
}

void uga_sl_list_insert_at ( uga_sl_list * this, i64_t const index, void * data )
{
        if( index <= 0 )
        {
                uga_sl_list_push_front( this, data ) ;
        }
        else if( index >= this->size )
        {
                uga_sl_list_push_back( this, data ) ;
        }
        else
        {
                uga_sl_node * to_insert = _uga_sl_node_create_with( this->elem_dtor, this->elem_size, data ) ;
                uga_sl_node * node      =  uga_sl_list_node_at( this, index - 1 ) ;

                to_insert->next = node->next ;
                node->next = to_insert ;

                ++this->size ;
        }
}

bool uga_sl_list_empty ( uga_sl_list const * list )
{
        return list->size == 0 ;
}

void uga_sl_list_clear ( uga_sl_list * this )
{
        while( this->size )
        {
                uga_sl_list_pop_front( this, NULL ) ;
        }
}

void uga_sl_list_destroy ( uga_sl_list * this )
{
        uga_sl_list_clear( this ) ;
}

void uga_sl_list_destroy_void ( void * this )
{
        uga_sl_list_destroy( this ) ;
}

uga_sl_node * _uga_sl_node_create ( void ( *dtor )( void * ), i64_t const elem_size )
{
        uga_sl_node * node = uga_allocate( sizeof( uga_sl_node ) ) ;

        node->data = uga_allocate( elem_size ) ;
        node->next = NULL ;
        node->dtor = dtor ;

        return node ;
}

uga_sl_node * _uga_sl_node_create_with ( void ( *dtor )( void * ), i64_t const elem_size, void * data )
{
        uga_sl_node * node = _uga_sl_node_create( dtor, elem_size ) ;
        memcpy( node->data, data, elem_size ) ;

        return node ;
}

void _uga_sl_node_destroy ( uga_sl_node * node )
{
        if( node->dtor )
        {
                node->dtor( node->data ) ;
        }
        uga_deallocate( node->data ) ;
        uga_deallocate( node       ) ;
}


uga_dl_node * _uga_dl_node_create       ( void ( *dtor )( void * ), i64_t const  elem_size              ) ;
uga_dl_node * _uga_dl_node_create_with  ( void ( *dtor )( void * ), i64_t const  elem_size, void * data ) ;

void _uga_dl_node_destroy ( uga_dl_node * node ) ;

uga_dl_list _uga_dl_list_create ( i64_t const elem_size )
{
        uga_dl_list list =
        {
                .head      =      NULL ,
                .size      =         0 ,
                .elem_size = elem_size ,
        } ;
        return list ;
}

uga_dl_list _uga_dl_list_create_d ( void ( *dtor )( void * ), i64_t const elem_size )
{
        uga_dl_list list = _uga_dl_list_create( elem_size ) ;
        list.elem_dtor = dtor ;

        return list ;
}

uga_dl_list uga_dl_list_from_vector ( uga_vector const * vector )
{
        uga_dl_list list = _uga_dl_list_create_d( vector->elem_dtor, vector->elem_size ) ;

        for( i64_t i = vector->size - 1; i >= 0; --i )
        {
                uga_dl_list_push_front( &list, uga_vec_at( vector, i ) ) ;
        }
        return list ;
}

void uga_dl_list_push_back ( uga_dl_list * this, void * data )
{
        if( this->size == 0 )
        {
                this->head = _uga_dl_node_create_with( this->elem_dtor, this->elem_size, data ) ;
        }
        else
        {
                uga_dl_node * last = this->head ;

                while( last->next != NULL )
                {
                        last = last->next ;
                }
                last->next = _uga_dl_node_create_with( this->elem_dtor, this->elem_size, data ) ;
                last->next->prev = last ;
        }
        ++this->size ;
}

void uga_dl_list_push_front ( uga_dl_list * this, void * data )
{
        if( this->size == 0 )
        {
                this->head = _uga_dl_node_create_with( this->elem_dtor, this->elem_size, data ) ;
        }
        else
        {
                uga_dl_node * new_front = _uga_dl_node_create_with( this->elem_dtor, this->elem_size, data ) ;

                new_front->next = this->head ;
                this->head->prev = new_front ;
                this->head = new_front ;
        }
        ++this->size ;
}

void * uga_dl_list_peek_back ( uga_dl_list * this )
{
        uga_clr_errs() ;

        if( this->size == 0 )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, 0, 0 ) ;
                return NULL ;
        }
        uga_dl_node * node = this->head ;

        while( node->next != NULL )
        {
                node = node->next ;
        }
        return node->data ;
}

void * uga_dl_list_peek_front ( uga_dl_list * this )
{
        if( this->size == 0 )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, 0, 0 ) ;
                return NULL ;
        }
        return this->head->data ;
}

void uga_dl_list_pop_back ( uga_dl_list * this, void * dest )
{
        uga_clr_errs() ;

        if( this->size == 0 )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, 0, 0 ) ;
                return ;
        }
        uga_dl_node * node = this->head ;

        if( this->size == 1 )
        {
                if( dest )
                {
                        memcpy( dest, node->data, this->elem_size ) ;
                }
                _uga_dl_node_destroy( node ) ;

                this->size =    0 ;
                this->head = NULL ;

                return ;
        }
        while( node->next != NULL )
        {
                node = node->next ;
        }
        if( dest )
        {
                memcpy( dest, node->data, this->elem_size ) ;
        }
        node->prev->next = NULL ;
        _uga_dl_node_destroy( node ) ;

        --this->size ;
}

void uga_dl_list_pop_front ( uga_dl_list * this, void * dest )
{
        uga_clr_errs() ;

        if( this->size == 0 )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, 0, 0 ) ;
                return ;
        }
        uga_dl_node * node = this->head ;
        if( dest )
        {
                memcpy( dest, node->data, this->elem_size ) ;
        }
        if( this->size == 1 )
        {
                this->head = NULL ;
        }
        else
        {
                this->head = node->next ;
                this->head->prev = NULL ;
        }
        _uga_dl_node_destroy( node ) ;
        --this->size ;
}

void * uga_dl_list_node_at ( uga_dl_list * this, i64_t const index )
{
        uga_clr_errs() ;

        if( index < 0 || index >= this->size )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, this->size, index ) ;
                return NULL ;
        }
        i64_t          pos =          0 ;
        uga_dl_node * node = this->head ;

        while( pos != index )
        {
                node = node->next ;
                ++pos ;
        }
        return node ;
}

void * uga_dl_list_at ( uga_dl_list * this, i64_t const index )
{
        uga_dl_node * node = uga_dl_list_node_at( this, index ) ;
        UGA_RETURN_ON_ERR( NULL ) ;

        return node->data ;
}

void uga_dl_list_remove_at ( uga_dl_list * this, i64_t const index, void * dest )
{
        uga_clr_errs() ;

        if( index < 0 || index >= this->size )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, this->size, index ) ;
                return ;
        }
        if( index == 0 )
        {
                uga_dl_list_pop_front( this, dest ) ;
                return ;
        }
        if( index == this->size - 1 )
        {
                uga_dl_list_pop_back( this, dest ) ;
                return ;
        }
        i64_t          pos =          0 ;
        uga_dl_node * node = this->head ;

        while( pos != index )
        {
                node = node->next ;
                ++pos ;
        }
        node->prev->next = node->next ;
        node->next->prev = node->prev ;

        if( dest )
        {
                memcpy( dest, node->data, this->elem_size ) ;
        }
        _uga_dl_node_destroy( node ) ;
        --this->size ;
}

void uga_dl_list_insert_at ( uga_dl_list * this, i64_t const index, void * data )
{
        if( index <= 0 )
        {
                uga_dl_list_push_front( this, data ) ;
        }
        else if( index >= this->size )
        {
                uga_dl_list_push_back( this, data ) ;
        }
        else
        {
                uga_dl_node * to_insert = _uga_dl_node_create_with( this->elem_dtor, this->elem_size, data ) ;
                uga_dl_node * node      =  uga_dl_list_node_at( this, index ) ;

                node->prev->next = to_insert ;
                to_insert->prev = node->prev ;

                node->prev = to_insert ;
                to_insert->next = node ;

                ++this->size ;
        }
}

uga_dl_node * uga_dl_list_find ( uga_dl_list * this, void * data )
{
        uga_dl_node * node = this->head ;

        while( node != NULL )
        {
                if( memcmp( node->data, data, this->elem_size ) == 0 )
                {
                        return node ;
                }
                node = node->next ;
        }
        return NULL ;
}

uga_dl_node * uga_dl_list_find_if ( uga_dl_list * this, bool ( *predicate )( void * data, void * extra ), void * extra )
{
        uga_dl_node * node = this->head ;

        while( node != NULL )
        {
                if( predicate( node->data, extra ) )
                {
                        return node ;
                }
                node = node->next ;
        }
        return NULL ;
}

void uga_dl_list_remove ( uga_dl_list * this, uga_dl_node * node )
{
        if( this->head == node ) this->head = node->next ;
        if( node->prev ) node->prev->next = node->next ;
        if( node->next ) node->next->prev = node->prev ;

        _uga_dl_node_destroy( node ) ;
        --this->size ;
}

void uga_dl_list_remove_elem ( uga_dl_list * this, void * data )
{
        uga_dl_node * node = this->head ;

        while( node != NULL )
        {
                if( node->data && memcmp( node->data, data, this->elem_size ) == 0 )
                {
                        if( this->head == node ) this->head = node->next ;
                        if( node->prev ) node->prev->next = node->next ;
                        if( node->next ) node->next->prev = node->prev ;

                        _uga_dl_node_destroy( node ) ;
                        --this->size ;

                        return ;
                }
                node = node->next ;
        }
}

bool uga_dl_list_empty ( uga_dl_list const * this )
{
        return this->size == 0 ;
}

void uga_dl_list_clear ( uga_dl_list * this )
{
        while( this->head )
        {
                uga_dl_node * to_destroy = this->head ;
                this->head = this->head->next ;

                _uga_dl_node_destroy( to_destroy ) ;
        }
}

void uga_dl_list_destroy ( uga_dl_list * this )
{
        uga_dl_list_clear( this ) ;
}

void uga_dl_list_destroy_void ( void * this )
{
        uga_dl_list_destroy( this ) ;
}

uga_dl_node * _uga_dl_node_create ( void ( *dtor )( void * ), i64_t const elem_size )
{
        uga_dl_node * node = uga_allocate( sizeof( uga_dl_node ) ) ;

        node->data = uga_allocate( elem_size ) ;
        node->next = NULL ;
        node->prev = NULL ;
        node->dtor = dtor ;

        return node ;
}

uga_dl_node * _uga_dl_node_create_with ( void ( *dtor )( void * ), i64_t const elem_size, void * data )
{
        uga_dl_node * node = _uga_dl_node_create( dtor, elem_size ) ;
        memcpy( node->data, data, elem_size ) ;

        return node ;
}

void _uga_dl_node_destroy ( uga_dl_node * node )
{
        if( node->dtor )
        {
                node->dtor( node->data ) ;
        }
        uga_deallocate( node->data ) ;
        uga_deallocate( node       ) ;
}
