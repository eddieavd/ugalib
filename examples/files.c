//
//
//      uganet
//      files.c
//

#include <uganet.h>

#include <stdio.h>


int main ( void )
{
        uga_log_init() ;

        uga_string_view filepath = uga_sv_create_1( "examples/files.c" ) ;

        i64_t filesize = uga_fs_filesize( filepath ) ;

        uga_string data = uga_str_create_1( filesize + 1 ) ;

        uga_fs_read_file_into( filepath, &data ) ;
        uga_print_abort_if_err() ;

        fprintf( stdout, STR_FMT"\n", STR_ARG( data ) ) ;


        return 0 ;
}
