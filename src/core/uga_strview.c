//
//
//      uganet
//      uga_strview.c
//

#include <core/uga_strview.h>
#include <core/uga_str.h>
#include <core/uga_log.h>
#include <core/uga_err.h>

#include <string.h>


uga_string_view uga_sv_create_0 ( void )
{
        uga_string_view view = { NULL, 0 } ;
        return view ;
}

uga_string_view uga_sv_create_1 ( char const * cstr )
{
        uga_string_view view = { cstr, strlen( cstr ) } ;
        return view ;
}

uga_string_view uga_sv_create_2 ( char const * data, i64_t const size )
{
        uga_string_view view = { data, size } ;
        return view ;
}

uga_string_view uga_sv_create_from ( uga_string const * str )
{
        uga_string_view view = { str->data, str->size } ;
        return view ;
}

i32_t uga_sv_equal ( uga_string_view lhs, uga_string_view rhs )
{
        if( lhs.size != rhs.size ) return 0 ;

        if( lhs.data == rhs.data ) return 1 ;

        for( i64_t i = 0; i < lhs.size; ++i )
        {
                if( lhs.data[ i ] != rhs.data[ i ] ) return 0 ;
        }
        return 1 ;
}

i32_t uga_sv_starts_with ( uga_string_view this, uga_string_view prefix )
{
        if( this.size < prefix.size ) return 0 ;

        if( this.data == prefix.data ) return 1 ;

        for( i64_t i = 0; i < prefix.size; ++i )
        {
                if( this.data[ i ] != prefix.data[ i ] ) return 0 ;
        }
        return 1 ;
}

i32_t uga_sv_ends_with ( uga_string_view this, uga_string_view suffix )
{
        if( this.size < suffix.size ) return 0 ;

        i64_t start = this.size - suffix.size ;

        if( this.data + start == suffix.data ) return 1 ;

        for( i64_t i = 0; i < suffix.size; ++i )
        {
                if( this.data[ start + i ] != suffix.data[ i ] ) return 0 ;
        }
        return 1 ;
}

void uga_sv_trim ( uga_string_view * this )
{
        uga_sv_trim_left ( this ) ;
        uga_sv_trim_right( this ) ;
}

void uga_sv_trim_left ( uga_string_view * this )
{
        while( !uga_sv_empty( this ) && ( uga_sv_front( this ) == ' ' || uga_sv_front( this ) == '\t' ) )
        {
                ++this->data ;
                --this->size ;
        }
}

void uga_sv_trim_right ( uga_string_view * this )
{
        while( !uga_sv_empty( this ) && ( uga_sv_back( this ) == ' ' || uga_sv_back( this ) == '\t' ) )
        {
                --this->size ;
        }
}

uga_string_view uga_sv_trimmed ( uga_string_view const * this )
{
        uga_string_view view = *this ;
        uga_sv_trim( &view ) ;
        return view ;
}

uga_string_view uga_sv_trimmed_left ( uga_string_view const * this )
{
        uga_string_view view = *this ;
        uga_sv_trim_left( &view ) ;
        return view ;
}

uga_string_view uga_sv_trimmed_right ( uga_string_view const * this )
{
        uga_string_view view = *this ;
        uga_sv_trim_right( &view ) ;
        return view ;
}

uga_string_view uga_sv_substr ( uga_string_view const * this, i64_t const start, i64_t const len )
{
        if( start >= this->size || start + len >= this->size ) return uga_sv_create_0() ;

        return uga_sv_create_2( this->data + start, len ) ;
}

uga_string_view uga_sv_chop_left ( uga_string_view * this, i64_t count )
{
        if( count > this->size ) count = this->size ;

        uga_string_view chop = { this->data, count } ;

        this->data += count ;
        this->size -= count ;

        return chop ;
}

uga_string_view uga_sv_chop_right ( uga_string_view * this, i64_t count )
{
        if( count > this->size ) count = this->size ;

        uga_string_view chop = { this->data + this->size - count, count } ;

        this->size -= count ;

        return chop ;
}

void uga_sv_unchop_left ( uga_string_view * this, i64_t count )
{
        if( uga_sv_empty( this ) ) return ;

        this->data -= count ;
        this->size += count ;
}

void uga_sv_unchop_right ( uga_string_view * this, i64_t count )
{
        if( uga_sv_empty( this ) ) return ;

        this->size += count ;
}

i32_t uga_sv_contains ( uga_string_view const * this, char const val )
{
        for( i64_t i = 0; i < this->size; ++i )
        {
                if( uga_sv_at( this, i ) == val ) return 1 ;
        }
        return 0 ;
}

i64_t uga_sv_index_of ( uga_string_view const * this, char const val )
{
        for( i64_t i = 0; i < this->size; ++i )
        {
                if( uga_sv_at( this, i ) == val ) return i ;
        }
        return this->size ;
}

i64_t uga_sv_count ( uga_string_view const * this, char const val )
{
        i64_t count = 0 ;
        for( i64_t i = 0; i < this->size; ++i )
        {
                if( uga_sv_at( this, i ) == val ) ++count ;
        }
        return count ;
}

uga_string_view uga_sv_chop_to_delimiter ( uga_string_view * this, char const delim, bool discard_delimiter )
{
        uga_string_view chop = { this->data, 0 } ;

        while( !uga_sv_empty( this ) && uga_sv_front( this ) != delim )
        {
                ++chop .size ;
                ++this->data ;
                --this->size ;
        }
        if( discard_delimiter && !uga_sv_empty( this ) )
        {
                ++this->data ;
                --this->size ;
        }
        return chop ;
}

i64_t uga_sv_parse_int ( uga_string_view const * this )
{
        uga_string_view digits = uga_sv_trimmed( this ) ;

        if( uga_sv_empty( &digits ) ) return 0 ;

        i32_t sign = 1 ;
        i64_t pos  = 0 ;

        if( uga_sv_front( &digits ) == '+' || uga_sv_front( &digits ) == '-' )
        {
                sign = ( uga_sv_front( &digits ) == '-' ) ? -1 : 1 ;
                ++pos ;
        }
        i64_t val = 0 ;
        for( ; pos < digits.size; ++pos )
        {
                char chr = uga_sv_at( &digits, pos ) ;
                if( chr < '0' || chr > '9' ) break ;

                val = val * 10 + ( chr - '0' ) ;
        }
        return sign * val ;
}

char uga_sv_at ( uga_string_view const * this, i64_t const index )
{
        uga_clr_errs() ;
        if( index >= this->size )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, this->size, index ) ;
                return '\0' ;
        }
        return this->data[ index ] ;
}

i32_t uga_sv_empty ( uga_string_view const * this )
{
        return ( !this->data || !this->size ) ;
}

char uga_sv_front ( uga_string_view const * this )
{
        uga_clr_errs() ;
        if( uga_sv_empty( this ) )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, 0, 1 ) ;
                return '\0' ;
        }
        return this->data[ 0 ] ;
}

char uga_sv_back ( uga_string_view const * this )
{
        uga_clr_errs() ;
        if( uga_sv_empty( this ) )
        {
                uga_set_mem_error( UGA_ERR_BAD_ACCESS, 0, 1 ) ;
        }
        return this->data[ this->size - 1 ] ;
}
