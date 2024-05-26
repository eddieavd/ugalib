//
//
//      uganet
//      uga_list.h
//

#ifndef UGA_LIST_H_
#define UGA_LIST_H_

#include <core/uga_types.h>

#define uga_sl_list_create(   type       ) _uga_sl_list_create(         sizeof( type ) )
#define uga_dl_list_create(   type       ) _uga_dl_list_create(         sizeof( type ) )
#define uga_sl_list_create_d( type, dtor ) _uga_sl_list_create_d( dtor, sizeof( type ) )
#define uga_dl_list_create_d( type, dtor ) _uga_dl_list_create_d( dtor, sizeof( type ) )


struct uga_vector_t ;

struct uga_sl_node_t
{
        void                 * data ;
        struct uga_sl_node_t * next ;
        void    ( *dtor )( void * ) ;
} ;
typedef struct uga_sl_node_t uga_sl_node ;

struct uga_sl_list_t
{
        uga_sl_node * head ;
        i64_t         size ;
        i64_t    elem_size ;

        void ( *elem_dtor )( void * ) ;
} ;
typedef struct uga_sl_list_t uga_sl_list ;


uga_sl_list _uga_sl_list_create   (                           i64_t const elem_size ) ;
uga_sl_list _uga_sl_list_create_d ( void ( *dtor )( void * ), i64_t const elem_size ) ;

uga_sl_list uga_sl_list_from_vector ( struct uga_vector_t const * vector ) ;

void uga_sl_list_push_back  ( uga_sl_list * this, void * data ) ;
void uga_sl_list_push_front ( uga_sl_list * this, void * data ) ;

void * uga_sl_list_peek_back  ( uga_sl_list * this ) ;
void * uga_sl_list_peek_front ( uga_sl_list * this ) ;

void uga_sl_list_pop_back  ( uga_sl_list * this, void * dest ) ;
void uga_sl_list_pop_front ( uga_sl_list * this, void * dest ) ;

void * uga_sl_list_node_at   ( uga_sl_list * this, i64_t const index              ) ;
void * uga_sl_list_at        ( uga_sl_list * this, i64_t const index              ) ;
void   uga_sl_list_remove_at ( uga_sl_list * this, i64_t const index, void * dest ) ;
void   uga_sl_list_insert_at ( uga_sl_list * this, i64_t const index, void * data ) ;

bool uga_sl_list_empty ( uga_sl_list const * this ) ;

void uga_sl_list_clear ( uga_sl_list * this ) ;

void uga_sl_list_destroy      ( uga_sl_list * this ) ;
void uga_sl_list_destroy_void ( void        * this ) ;


struct uga_dl_node_t
{
        void                 * data ;
        struct uga_dl_node_t * prev ;
        struct uga_dl_node_t * next ;
        void    ( *dtor )( void * ) ;
} ;
typedef struct uga_dl_node_t uga_dl_node ;

struct uga_dl_list_t
{
        uga_dl_node * head ;
        i64_t         size ;
        i64_t    elem_size ;

        void ( *elem_dtor )( void * ) ;
} ;
typedef struct uga_dl_list_t uga_dl_list ;


uga_dl_list _uga_dl_list_create   (                           i64_t const elem_size ) ;
uga_dl_list _uga_dl_list_create_d ( void ( *dtor )( void * ), i64_t const elem_size ) ;

uga_dl_list uga_dl_list_from_vector ( struct uga_vector_t const * vector ) ;

void uga_dl_list_push_back  ( uga_dl_list * this, void * data ) ;
void uga_dl_list_push_front ( uga_dl_list * this, void * data ) ;

void * uga_dl_list_peek_back  ( uga_dl_list * this ) ;
void * uga_dl_list_peek_front ( uga_dl_list * this ) ;

void uga_dl_list_pop_back  ( uga_dl_list * this, void * dest ) ;
void uga_dl_list_pop_front ( uga_dl_list * this, void * dest ) ;

void * uga_dl_list_node_at   ( uga_dl_list * this, i64_t const index              ) ;
void * uga_dl_list_at        ( uga_dl_list * this, i64_t const index              ) ;
void   uga_dl_list_remove_at ( uga_dl_list * this, i64_t const index, void * dest ) ;
void   uga_dl_list_insert_at ( uga_dl_list * this, i64_t const index, void * data ) ;

uga_dl_node * uga_dl_list_find    ( uga_dl_list * this, void * data ) ;
uga_dl_node * uga_dl_list_find_if ( uga_dl_list * this, bool ( *predicate )( void * data, void * extra ), void * extra ) ;

void uga_dl_list_remove      ( uga_dl_list * this, uga_dl_node * node ) ;
void uga_dl_list_remove_elem ( uga_dl_list * this, void        * data ) ;

bool uga_dl_list_empty ( uga_dl_list const * this ) ;

void uga_dl_list_clear ( uga_dl_list * this ) ;

void uga_dl_list_destroy      ( uga_dl_list * this ) ;
void uga_dl_list_destroy_void ( void        * this ) ;


#endif // UGA_LIST_H_
