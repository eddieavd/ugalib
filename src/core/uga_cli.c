//
//
//      uganet
//      uga_cli.c
//

#include <core/uga_cli.h>
#include <core/uga_err.h>
#include <core/uga_str.h>

#include <string.h>
#include <unistd.h>


i32_t uga_cli_parse_args ( i32_t argc, char ** argv, uga_cli_args * args )
{
        i32_t parsed_args = 0 ;

        uga_string opts_str = uga_str_create_1( args->arg_count * 2 + 1 ) ;

        for( i32_t i = 0; i < args->arg_count; ++i )
        {
                uga_str_append( &opts_str, &args->args[ i ].name, 1 ) ;
                if( args->args[ i ].has_value )
                {
                        uga_str_append( &opts_str, ":", 1 ) ;
                }
        }
        uga_str_append( &opts_str, "\0", 1 ) ;

        i32_t arg = 0 ;

        while( ( arg = getopt( argc, argv, opts_str.data ) ) != -1 )
        {
                if( arg == '?' )
                {
                        uga_set_cli_error( UGA_ERR_CLI ) ;
                        uga_str_destroy( &opts_str ) ;
                        return -1 ;
                }
                for( i32_t i = 0; i < args->arg_count; ++i )
                {
                        if( args->args[ i ].name == arg && args->args[ i ].has_value )
                        {
                                memcpy( args->args[ i ].value, optarg, strlen( optarg ) ) ;
                                parsed_args += 2 ;
                        }
                        else if( args->args[ i ].name == arg )
                        {
                                memcpy( args->args[ i ].value, "true", 4 ) ;
                                parsed_args += 1 ;
                        }
                        else
                        {
                                // unreachable?
                        }
                }
        }
        uga_str_destroy( &opts_str ) ;
        return parsed_args ;
}

bool uga_cli_is_set ( char const arg, uga_cli_args const * args )
{
        for( i32_t i = 0; i < args->arg_count; ++i )
        {
                if( args->args[ i ].name == arg )
                {
                        return args->args[ i ].value[ 0 ] != '\0' ;
                }
        }
        return false ;
}

char const * uga_cli_get_val ( char const arg, uga_cli_args const * args )
{
        for( i32_t i = 0; i < args->arg_count; ++i )
        {
                if( args->args[ i ].name == arg )
                {
                        return args->args[ i ].value ;
                }
        }
        return "" ;
}
