//
//
//      uganet
//      uga_cli.h
//

#ifndef UGA_ARG_H_
#define UGA_ARG_H_

#ifndef UGA_CLI_ARG_LEN
#define UGA_CLI_ARG_LEN 32
#endif

#ifndef UGA_NUM_CLI_ARGS
#define UGA_NUM_CLI_ARGS 32
#endif

#include <core/uga_types.h>


typedef struct
{
        char      name ;
        bool has_value ;
        char     value [ UGA_CLI_ARG_LEN ] ;
} uga_arg ;

typedef struct
{
        i32_t arg_count ;
        uga_arg args[ UGA_NUM_CLI_ARGS ] ;
} uga_cli_args ;


i32_t uga_cli_parse_args ( i32_t argc, char ** argv, uga_cli_args * args ) ;

bool         uga_cli_is_set  ( char const arg, uga_cli_args const * args ) ;
char const * uga_cli_get_val ( char const arg, uga_cli_args const * args ) ;


#endif // UGA_ARG_H_
