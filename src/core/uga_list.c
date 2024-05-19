//
//
//      uganet
//      uga_sl_list.c
//

#include <core/uga_list.h>
#include <core/uga_err.h>
#include <core/uga_alloc.h>

#include <string.h>


uga_sl_node * _uga_sl_node_create  ( i64_t const  elem_size ) ;
void          _uga_sl_node_destroy ( uga_sl_node *     node ) ;

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

void uga_sl_list_push_back ( uga_sl_list * this, void * data )
{
        if( this->size == 0 )
        {
                this->head = _uga_sl_node_create( this->elem_size ) ;

                memcpy( this->head->data, data, this->elem_size ) ;
        }
        else
        {
                uga_sl_node * last = this->head ;

                while( last->next != NULL )
                {
                        last = last->next ;
                }
                last->next = _uga_sl_node_create( this->elem_size ) ;

                memcpy( last->next->data, data, this->elem_size ) ;
        }
        ++this->size ;
}

void uga_sl_list_push_front ( uga_sl_list * this, void * data )
{
        if( this->size == 0 )
        {
                this->head = _uga_sl_node_create( this->elem_size ) ;

                memcpy( this->head->data, data, this->elem_size ) ;
        }
        else
        {
                uga_sl_node * new_front = _uga_sl_node_create( this->elem_size ) ;

                new_front->next = this->head ;
                this->head = new_front ;

                memcpy( new_front->data, data, this->elem_size ) ;
        }
        ++this->size ;
}

void * uga_sl_list_peek_back ( uga_sl_list * this )
{
        if( this->size == 0 ) return NULL ;

        uga_sl_node * node = this->head ;

        while( node->next != NULL )
        {
                node = node->next ;
        }
        return node->data ;
}

void * uga_sl_list_peek_front ( uga_sl_list * this )
{
        if( this->size == 0 ) return NULL ;

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
        if( this->size == 1 )
        {
                this->size =    0 ;
                this->head = NULL ;
        }
        else
        {
                --this->size ;
                this->head = node->next ;
        }
        _uga_sl_node_destroy( node ) ;
}

void * uga_sl_list_at ( uga_sl_list * this, i64_t const index )
{
        uga_clr_errs() ;

        if( index < 0 || index >= this->size )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, this->size, index ) ;
                return NULL ;
        }
        i64_t pos = 0 ;
        uga_sl_node * node = this->head ;

        while( pos != index )
        {
                node = node->next ;
                ++pos ;
        }
        return node->data ;
}

void uga_sl_list_remove_at ( uga_sl_list * this, i64_t const index )
{
        uga_clr_errs() ;

        if( index < 0 || index >= this->size )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, this->size, index ) ;
                return ;
        }
        i64_t pos = 0 ;
        uga_sl_node * node = this->head ;

        while( pos + 1 != index )
        {
                node = node->next ;
                ++pos ;
        }
        uga_sl_node * to_remove = node->next ;

        node->next = to_remove->next ;

        _uga_sl_node_destroy( to_remove ) ;
}

void uga_sl_list_pop_at ( uga_sl_list * this, i64_t const index, void * dest )
{
        uga_clr_errs() ;

        if( index < 0 || index >= this->size )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, this->size, index ) ;
                return ;
        }
        i64_t pos = 0 ;
        uga_sl_node * node = this->head ;

        while( pos + 1 != index )
        {
                node = node->next ;
                ++pos ;
        }
        uga_sl_node * to_pop = node->next ;

        if( dest )
        {
                memcpy( dest, to_pop->data, this->elem_size ) ;
        }
        node->next = to_pop->next ;

        _uga_sl_node_destroy( to_pop ) ;
}

bool uga_sl_list_empty ( uga_sl_list const * list )
{
        return list->size == 0 ;
}

void uga_sl_list_destroy ( uga_sl_list * this )
{
        while( this->size )
        {
                uga_sl_list_pop_front( this, NULL ) ;
        }
}

uga_sl_node * _uga_sl_node_create ( i64_t const elem_size )
{
        uga_sl_node * node = uga_allocate( sizeof( uga_sl_node ) ) ;

        node->data = uga_allocate( elem_size ) ;
        node->next = NULL ;

        return node ;
}

void _uga_sl_node_destroy ( uga_sl_node * node )
{
        uga_deallocate( node->data ) ;
        uga_deallocate( node       ) ;
}


uga_dl_node * _uga_dl_node_create  ( i64_t const  elem_size ) ;
void          _uga_dl_node_destroy ( uga_dl_node *     node ) ;


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

void uga_dl_list_push_back ( uga_dl_list * this, void * data )
{
        if( this->size == 0 )
        {
                this->head = _uga_dl_node_create( this->elem_size ) ;

                memcpy( this->head->data, data, this->elem_size ) ;
        }
        else
        {
                uga_dl_node * last = this->head ;

                while( last->next != NULL )
                {
                        last = last->next ;
                }
                last->next = _uga_dl_node_create( this->elem_size ) ;
                last->next->prev = last ;

                memcpy( last->next->data, data, this->elem_size ) ;
        }
        ++this->size ;
}

void uga_dl_list_push_front ( uga_dl_list * this, void * data )
{
        if( this->size == 0 )
        {
                this->head = _uga_dl_node_create( this->elem_size ) ;

                memcpy( this->head->data, data, this->elem_size ) ;
        }
        else
        {
                uga_dl_node * new_front = _uga_dl_node_create( this->elem_size ) ;

                new_front->next = this->head ;
                this->head->prev = new_front ;
                this->head = new_front ;

                memcpy( new_front->data, data, this->elem_size ) ;
        }
        ++this->size ;
}

void * uga_dl_list_peek_back ( uga_dl_list * this )
{
        if( this->size == 0 ) return NULL ;

        uga_dl_node * node = this->head ;

        while( node->next != NULL )
        {
                node = node->next ;
        }
        return node->data ;
}

void * uga_dl_list_peek_front ( uga_dl_list * this )
{
        if( this->size == 0 ) return NULL ;

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
                this->size =    0 ;
                this->head = NULL ;
        }
        else
        {
                --this->size ;
                this->head = node->next ;
                this->head->prev = NULL ;
        }
        _uga_dl_node_destroy( node ) ;
}

void * uga_dl_list_at ( uga_dl_list * this, i64_t const index )
{
        uga_clr_errs() ;

        if( index < 0 || index >= this->size )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, this->size, index ) ;
                return NULL ;
        }
        i64_t pos = 0 ;
        uga_dl_node * node = this->head ;

        while( pos != index )
        {
                node = node->next ;
                ++pos ;
        }
        return node->data ;
}

void uga_dl_list_remove_at ( uga_dl_list * this, i64_t const index )
{
        uga_clr_errs() ;

        if( index < 0 || index >= this->size )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, this->size, index ) ;
                return ;
        }
        i64_t pos = 0 ;
        uga_dl_node * node = this->head ;

        while( pos != index )
        {
                node = node->next ;
                ++pos ;
        }
        node->prev->next = node->next ;
        node->next->next = node->prev ;

        _uga_dl_node_destroy( node ) ;
}

void uga_dl_list_pop_at ( uga_dl_list * this, i64_t const index, void * dest )
{
        uga_clr_errs() ;

        if( index < 0 || index >= this->size )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, this->size, index ) ;
                return ;
        }
        i64_t pos = 0 ;
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
}

bool uga_dl_list_empty ( uga_dl_list const * this )
{
        return this->size == 0 ;
}

void uga_dl_list_destroy ( uga_dl_list * this )
{
        while( this->size )
        {
                uga_dl_list_pop_front( this, NULL ) ;
        }
}

uga_dl_node * _uga_dl_node_create ( i64_t const elem_size )
{
        uga_dl_node * node = uga_allocate( sizeof( uga_dl_node ) ) ;

        node->data = uga_allocate( elem_size ) ;
        node->next = NULL ;
        node->prev = NULL ;

        return node ;
}

void _uga_dl_node_destroy ( uga_dl_node * node )
{
        uga_deallocate( node->data ) ;
        uga_deallocate( node       ) ;
}
