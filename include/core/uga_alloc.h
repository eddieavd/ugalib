//
//
//      uganet
//      uga_alloc.h
//

#ifndef UGA_ALLOC_H_
#define UGA_ALLOC_H_

#include <core/uga_types.h>


/*
typedef struct
{
        void * buff ;
        i64_t  size ;
} block ;

block uga_allocate   (              i64_t const bytes ) ;
void  uga_reallocate ( block * mem, i64_t const bytes ) ;
void  uga_deallocate ( block * mem                    ) ;
*/

void * uga_allocate   (              i64_t const bytes ) ;
void   uga_reallocate ( void * buff, i64_t const bytes ) ;
void   uga_deallocate ( void * buff                    ) ;


#endif // UGA_ALLOC_H_
