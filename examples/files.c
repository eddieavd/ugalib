//
//
//      uganet
//      files.c
//

#include <uganet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main ( void )
{
        uga_log_init() ;

        char const * filepath = "examples/files.c" ;

        i64_t filesize = uga_fs_filesize( filepath ) ;

        u8_t * data = ( u8_t * ) malloc( filesize + 1 ) ;
        memset( data, 0, filesize + 1 ) ;

        uga_fs_read_file_into( filepath, data, filesize ) ;
        uga_print_abort_if_err() ;

        fprintf( stdout, "%s\n", data ) ;


        return 0 ;
}
