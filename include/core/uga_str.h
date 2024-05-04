//
//
//      uganet
//      uga_str.h
//

#ifndef UGA_STR_H_
#define UGA_STR_H_


#include <core/uga_types.h>

#include <stdbool.h>

#define STR_FMT "%.*s"
#define STR_ARG(str) (i32_t) (str).size, (str).data


struct uga_string_t
{
        char *    data ;
        i64_t     size ;
        i64_t capacity ;
} ;
typedef struct uga_string_t uga_string ;


uga_string uga_str_create_0 ( void                 ) ;
uga_string uga_str_create_1 ( i64_t const capacity ) ;

uga_string uga_str_create_from_1 ( char const * cstr                      ) ;
uga_string uga_str_create_from_2 ( char const * data, i64_t const datalen ) ;

uga_string uga_str_copy ( uga_string const * other ) ;
uga_string uga_str_move ( uga_string       * other ) ;

char       * uga_str_data  ( uga_string       * str ) ;
char const * uga_str_cdata ( uga_string const * str ) ;

i64_t uga_str_size     ( uga_string const * str ) ;
i64_t uga_str_capacity ( uga_string const * str ) ;
bool  uga_str_empty    ( uga_string const * str ) ;

char uga_str_at ( uga_string const * str, i64_t const pos ) ;

i64_t uga_str_space_left ( uga_string const * str ) ;

void uga_str_reserve ( uga_string * str, i64_t const capacity ) ;

void uga_str_shrink_to_fit ( uga_string * str ) ;

void uga_str_append_str ( uga_string * str, uga_string const * other ) ;

void uga_str_append_cstr ( uga_string * str, char const * cstr ) ;

void uga_str_push_back ( uga_string * str, char const val ) ;

void uga_str_append ( uga_string * str, char const * data, i64_t const datalen ) ;

void uga_str_destroy ( uga_string * str ) ;


#endif // UGA_STR_H_
