//
//
//      uganet
//      uga_pool.h
//

#ifndef UGA_POOL_H_
#define UGA_POOL_H_

#include <core/uga_types.h>
#include <core/uga_list.h>
#include <core/uga_vector.h>
#include <core/uga_thread.h>


typedef struct
{
        bool      is_active ;
        uga_dl_list   tasks ;
        uga_dl_list  t_done ;
        uga_vector     pool ;
        uga_mtx_t  task_mtx ;
        uga_mtx_t  done_mtx ;
        uga_semaphore t_sem ;
} uga_thread_pool ;


uga_thread_pool * uga_pool_create_new ( i32_t const num_threads ) ;

void uga_pool_add_task ( uga_thread_pool * pool, uga_task task ) ;

uga_dl_list * uga_pool_get_finished_tasks ( uga_thread_pool * pool ) ;

void uga_pool_clear_finished_tasks ( uga_thread_pool * pool ) ;

void uga_pool_destroy      ( uga_thread_pool * pool ) ;
void uga_pool_destroy_void ( void            * pool ) ;


#endif // UGA_POOL_H_
