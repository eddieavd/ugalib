//
//
//      uganet
//      tftp::server.c
//

#include <core/uga_log.h>
#include <core/uga_err.h>
#include <core/uga_str.h>
#include <core/uga_vector.h>
#include <core/uga_thread.h>
#include <core/uga_pool.h>
#include <net/uga_ftp.h>


int main ( void )
{
        uga_log_init() ;

        UGA_INFO_S( "main", "started." ) ;

        uga_string_view port = uga_sv_create_1( "8372" ) ;

        UGA_INFO_S( "main", "creating server on port "SV_FMT"...", SV_ARG( port ) ) ;

        uga_tftp_server tftp_server = uga_tftp_server_create( port ) ;

        UGA_INFO_S( "main", "server created, starting..." ) ;

        i32_t ret = uga_tftp_server_run( &tftp_server ) ;

        uga_tftp_server_destroy( &tftp_server ) ;

        UGA_INFO_S( "main", "finished." ) ;

        return ret ;
}
