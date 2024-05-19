//
//
//      uganet
//      uga_thread.h
//

#ifndef UGA_THREAD_H_
#define UGA_THREAD_H_

#ifdef __STDC_NO_THREADS__
#error "threads not available"
#endif

#include <core/uga_types.h>
#include <core/uga_str.h>

#include <threads.h>


struct uga_task_t
{
        uga_string data ;
        i32_t ( *run )( void * ) ;
} ;
typedef struct uga_task_t uga_task ;

struct uga_thread_t
{
        uga_thread_id id ;
        uga_task    task ;
} ;
typedef struct uga_thread_t uga_thread ;


typedef enum
{
        UGA_MTX_PLAIN           = mtx_plain ,
        UGA_MTX_TIMED           = mtx_timed ,
        UGA_MTX_RECURSIVE       = mtx_plain | mtx_recursive ,
        UGA_MTX_TIMED_RECURSIVE = mtx_timed | mtx_recursive ,
} uga_mtx_type ;


_timespec_t uga_time_current ( void ) ;

uga_thread * uga_thread_do_task ( uga_task task ) ;

i32_t uga_thread_join   ( uga_thread thread ) ;
void  uga_thread_detach ( uga_thread thread ) ;

void uga_thread_sleep_ns ( i64_t const  nanos ) ;
void uga_thread_sleep_ms ( i64_t const millis ) ;
void uga_thread_sleep_s  ( i64_t const   secs ) ;

uga_thread_id uga_thread_self ( void ) ;


uga_mtx_t uga_mtx_init ( uga_mtx_type const type ) ;

i32_t uga_mtx_acquire ( uga_mtx_t * mtx ) ;
i32_t uga_mtx_release ( uga_mtx_t * mtx ) ;

i32_t uga_mtx_acquire_timed_ns ( uga_mtx_t * mtx, i64_t const  nanos ) ;
i32_t uga_mtx_acquire_timed_ms ( uga_mtx_t * mtx, i64_t const millis ) ;
i32_t uga_mtx_acquire_timed_s  ( uga_mtx_t * mtx, i64_t const   secs ) ;

void uga_mtx_destroy ( uga_mtx_t * mtx ) ;


uga_cnd_t uga_cnd_init ( void ) ;

i32_t uga_cnd_wait ( uga_cnd_t * cnd, uga_mtx_t * mtx ) ;

i32_t uga_cnd_wait_timed_ns ( uga_cnd_t * cnd, uga_mtx_t * mtx, i64_t const  nanos ) ;
i32_t uga_cnd_wait_timed_ms ( uga_cnd_t * cnd, uga_mtx_t * mtx, i64_t const millis ) ;
i32_t uga_cnd_wait_timed_s  ( uga_cnd_t * cnd, uga_mtx_t * mtx, i64_t const   secs ) ;

i32_t uga_cnd_ping     ( uga_cnd_t * cnd ) ;
i32_t uga_cnd_ping_all ( uga_cnd_t * cnd ) ;

void uga_cnd_destroy ( uga_cnd_t * cnd ) ;


typedef struct
{
        i32_t     val ;
        uga_mtx_t mtx ;
        uga_cnd_t cnd ;
} uga_semaphore ;


uga_semaphore uga_sem_init ( i32_t init_val ) ;

void uga_sem_acquire   ( uga_semaphore * sem              ) ;
void uga_sem_release   ( uga_semaphore * sem              ) ;
void uga_sem_release_n ( uga_semaphore * sem, i32_t count ) ;

void uga_sem_destroy ( uga_semaphore * sem ) ;


#endif // UGA_THREAD_H_
