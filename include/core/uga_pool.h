//
//
//      uganet
//      uga_pool.h
//

#ifndef UGA_POOL_H_
#define UGA_POOL_H_

#include <core/uga_types.h>
#include <core/uga_vector.h>
#include <core/uga_thread.h>


typedef struct
{
        bool      is_active ;
        uga_vector    tasks ;
        uga_vector     pool ;
        uga_mtx_t  task_mtx ;
        uga_semaphore t_sem ;
} uga_thread_pool ;


uga_thread_pool * uga_pool_create_new ( i32_t const num_threads ) ;

void uga_pool_add_task ( uga_thread_pool * pool, uga_task task ) ;

void uga_pool_destroy      ( uga_thread_pool * pool ) ;
void uga_pool_destroy_void ( void            * pool ) ;


#endif // UGA_POOL_H_
