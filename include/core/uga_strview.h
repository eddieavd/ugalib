//
//
//      uganet
//      uga_strview.h
//

#ifndef UGA_STRVIEW_H_
#define UGA_STRVIEW_H_

#include <core/uga_types.h>

#define SV_FMT "%.*s"
#define SV_ARG(sv) (i32_t) (sv).size, (sv).data


struct uga_string_t ;

struct uga_string_view_t
{
        char const * data ;
        i64_t        size ;
} ;
typedef struct uga_string_view_t uga_string_view ;


uga_string_view uga_sv_create_0 ( void                                ) ;
uga_string_view uga_sv_create_1 ( char const * cstr                   ) ;
uga_string_view uga_sv_create_2 ( char const * data, i64_t const size ) ;

uga_string_view uga_sv_create_from ( struct uga_string_t const * str ) ;

i32_t uga_sv_equal       ( uga_string_view  lhs, uga_string_view    rhs ) ;
i32_t uga_sv_starts_with ( uga_string_view this, uga_string_view prefix ) ;
i32_t uga_sv_ends_with   ( uga_string_view this, uga_string_view suffix ) ;

i32_t uga_sv_equal_insensitive       ( uga_string_view  lhs, uga_string_view    rhs ) ;
i32_t uga_sv_starts_with_insensitive ( uga_string_view this, uga_string_view prefix ) ;
i32_t uga_sv_ends_with_insensitive   ( uga_string_view this, uga_string_view suffix ) ;

void uga_sv_trim       ( uga_string_view * this ) ;
void uga_sv_trim_left  ( uga_string_view * this ) ;
void uga_sv_trim_right ( uga_string_view * this ) ;

uga_string_view uga_sv_trimmed       ( uga_string_view this ) ;
uga_string_view uga_sv_trimmed_left  ( uga_string_view this ) ;
uga_string_view uga_sv_trimmed_right ( uga_string_view this ) ;

uga_string_view uga_sv_substr ( uga_string_view this, i64_t const start, i64_t const len ) ;

uga_string_view uga_sv_chop_left  ( uga_string_view * this, i64_t count ) ;
uga_string_view uga_sv_chop_right ( uga_string_view * this, i64_t count ) ;

void uga_sv_unchop_left  ( uga_string_view * this, i64_t count ) ;
void uga_sv_unchop_right ( uga_string_view * this, i64_t count ) ;

i32_t uga_sv_contains ( uga_string_view this, char const val ) ;
i64_t uga_sv_index_of ( uga_string_view this, char const val ) ;
i64_t uga_sv_count    ( uga_string_view this, char const val ) ;

uga_string_view uga_sv_chop_to_delimiter ( uga_string_view * this, char const delim, bool discard_delimiter ) ;

i64_t uga_sv_parse_int ( uga_string_view this ) ;

char uga_sv_at ( uga_string_view this, i64_t const index ) ;

i32_t uga_sv_empty ( uga_string_view this ) ;

char uga_sv_front ( uga_string_view this ) ;
char uga_sv_back  ( uga_string_view this ) ;


#endif // UGA_STRVIEW_H_
