//
//
//      uganet
//      uga.c
//

#include <core/uga_log.h>
#include <core/uga_err.h>
#include <core/uga_str.h>
#include <core/uga_vector.h>
#include <core/uga_thread.h>
#include <core/uga_pool.h>
#include <net/uga_tftp.h>


int main ( void )
{
        uga_log_init() ;

        uga_tftp_server_config config = { UGA_TFTP_SRV_MAIN, { "8372", 4 } } ;

        uga_tftp_server tftp_server = uga_tftp_server_create( config ) ;

        return uga_tftp_server_run( &tftp_server ) ;
}
