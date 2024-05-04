//
//
//      uganet
//      demo.c
//

#include <uganet.h>

#include <stdio.h>


void configure_uga ( void ) ;

int main ( void )
{
        uga_log_init () ;

        char const *    host = "www.fer.unizg.hr" ;
        char const * service = "https" ;

        char    ip [ UGA_IPv6_STRLEN ] = { 0 } ;
        u16_t port ;

        uga_lookup( host, service, ip, &port ) ;

        printf( "%s (%s) %s (%d)\n", host, ip, service, port ) ;


        return 0;
}
