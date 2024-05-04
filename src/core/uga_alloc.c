//
//
//      uganet
//      uga_alloc.c
//

#include <core/uga_alloc.h>
#include <core/uga_log.h>
#include <core/uga_err.h>

#include <stdlib.h>


void * uga_allocate ( i64_t const bytes )
{
        uga_clr_errs() ;

        void * buff = malloc( bytes ) ;
        if( !buff )
        {
                UGA_ERR_S( "uga::allocate", "failed allocating %d bytes", bytes ) ;
                uga_set_alloc_error( UGA_ERR_BAD_ALLOC, bytes ) ;
        }
        return buff ;
}

void uga_reallocate ( void * buff, i64_t const bytes )
{
        uga_clr_errs() ;

        void * reallocation = realloc( buff, bytes ) ;
        if( !reallocation )
        {
                UGA_ERR_S( "uga::reallocate", "failed reallocating block of size %d to new size of %d", -1, bytes ) ;
                uga_set_alloc_error( UGA_ERR_BAD_REALLOC, bytes ) ;
                return ;
        }
        buff = reallocation ;
}

void uga_deallocate ( void * buff )
{
        free( buff ) ;
}
