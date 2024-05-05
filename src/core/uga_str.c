//
//
//      uganet
//      uga_str.c
//

#include <core/uga_str.h>
#include <core/uga_log.h>
#include <core/uga_err.h>
#include <core/uga_alloc.h>

#include <stddef.h>
#include <string.h>


uga_string uga_str_create_0 ( void )
{
        uga_string str = { NULL, 0, 0 } ;
        return str ;
}

uga_string uga_str_create_1 ( i64_t const capacity )
{
        char * data = ( char * ) uga_allocate( capacity ) ;

        if( uga_had_errs() ) return uga_str_create_0() ;

        memset( data, 0, capacity ) ;
        uga_string str = { data, 0, capacity } ;

        return str ;
}

uga_string uga_str_create_from_1 ( char const * cstr )
{
        i64_t size = strlen( cstr ) ;

        uga_string str = uga_str_create_1( size ) ;

        if( uga_had_errs() ) return uga_str_create_0() ;

        memcpy( str.data, cstr, size ) ;
        str.size = size ;

        return str ;
}

uga_string uga_str_create_from_2 ( char const * data, i64_t const datalen )
{
        uga_string str = uga_str_create_1( datalen ) ;

        if( uga_had_errs() ) return uga_str_create_0() ;

        memcpy( str.data, data, datalen ) ;
        str.size = datalen ;

        return str ;
}

uga_string uga_str_copy ( uga_string const * other )
{
        uga_string str = uga_str_create_1( other->size ) ;

        if( uga_had_errs() ) return uga_str_create_0() ;

        memcpy( str.data, other->data, other->size ) ;
        str.capacity = str.size = other->size ;

        return str ;
}

uga_string uga_str_move ( uga_string * other )
{
        uga_string str ;

        str.data     = other->data ;
        str.size     = other->size ;
        str.capacity = other->capacity ;

        other->data     = NULL ;
        other->size     =    0 ;
        other->capacity =    0 ;

        return str ;
}

char       * uga_str_data  ( uga_string       * str ) { return str->data ; }
char const * uga_str_cdata ( uga_string const * str ) { return str->data ; }

i64_t uga_str_size     ( uga_string const * str ) { return str->      size ; }
i64_t uga_str_capacity ( uga_string const * str ) { return str->  capacity ; }
bool  uga_str_empty    ( uga_string const * str ) { return str->size == 0  ; }

i64_t uga_str_space_left ( uga_string const * str ) { return str->capacity - str->size ; }

char uga_str_at ( uga_string const * str, i64_t const pos )
{
        if( pos >= str->size )
        {
                UGA_ERR_S( "uga::str::at", "index out of bounds" ) ;
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, str->size, pos ) ;
                return -1 ;
        }
        return str->data[ pos ] ;
}

void uga_str_reserve ( uga_string * str, i64_t const capacity )
{
        if( capacity <= str->capacity )
        {
                return ;
        }
        uga_reallocate( ( void ** ) &str->data, capacity ) ;

        if( uga_had_errs() ) return ;

        str->capacity = capacity ;
}

void uga_str_shrink_to_fit ( uga_string * str )
{
        if( str->size >= str->capacity ) return ;

        char * new_block = ( char * ) uga_allocate( str->size ) ;
        if( uga_had_errs() ) return ;

        memcpy( new_block, str->data, str->size ) ;

        uga_deallocate( str->data ) ;

        str->data     = new_block ;
        str->capacity = str->size ;
}

void uga_str_append_str ( uga_string * str, uga_string const * other )
{
        uga_str_append( str, other->data, other->size ) ;
}

void uga_str_append_cstr ( uga_string * str, char const * cstr )
{
        uga_str_append( str, cstr, strlen( cstr ) ) ;
}

void uga_str_push_back ( uga_string * str, char const val )
{
        if( uga_str_empty( str ) )
        {
                *str = uga_str_create_1( 1 ) ;
                str->size = 1 ;
                str->data[ 0 ] = val ;
                return ;
        }
        if( uga_str_space_left( str ) == 0 )
        {
                uga_str_reserve( str, str->size + 1 ) ;
        }
        str->data[ str->size++ ] = val ;
}

void uga_str_append ( uga_string * str, char const * data, i64_t const datalen )
{
        if( uga_str_empty( str ) )
        {
                *str = uga_str_create_from_2( data, datalen ) ;
                return ;
        }
        else if( uga_str_space_left( str ) >= datalen )
        {
                memcpy( str->data + str->size, data, datalen ) ;
                str->size += datalen ;
        }
        else
        {
                uga_str_reserve( str, str->size + datalen ) ;

                if( uga_had_errs() )
                {
                        return ;
                }

                memcpy( str->data + str->size, data, datalen ) ;
                str->size += datalen ;
        }
}

void uga_str_clear ( uga_string * str )
{
        memset( str->data, '\0', str->capacity ) ;
        str->size = 0 ;
}

void uga_str_destroy ( uga_string * str )
{
        if( str->data )
        {
                uga_deallocate( str->data ) ;
        }
        str->data     = NULL ;
        str->size     =    0 ;
        str->capacity =    0 ;
}

void uga_str_destroy_void ( void * str )
{
        uga_str_destroy( str ) ;
}
