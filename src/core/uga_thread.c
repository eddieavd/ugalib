//
//
//      uganet
//      uga_thread.c
//

#include <core/uga_thread.h>
#include <core/uga_err.h>
#include <core/uga_alloc.h>

#include <pthread.h>


uga_thread * uga_thread_do_task ( uga_task task )
{
        uga_clr_errs() ;

        uga_thread * thread = uga_allocate( sizeof( uga_thread ) ) ;

        thread->id = -1 ;
        thread->task.data = uga_vec_copy( &task.data ) ;
        thread->task. run =                task. run   ;
        i32_t err = thrd_create( &thread->id, thread->task.run, &thread->task.data ) ;
        _uga_check_thrd_err( err, thread->id ) ;

        return thread ;
}

i32_t uga_thread_join ( uga_thread thread )
{
        uga_clr_errs() ;

        i32_t status ;
        i32_t err = thrd_join( thread.id, &status ) ;
        _uga_check_thrd_err( err, thread.id ) ;

        return status ;
}

void uga_thread_detach ( uga_thread thread )
{
        uga_clr_errs() ;

        i32_t err = thrd_detach( thread.id ) ;
        _uga_check_thrd_err( err, thread.id ) ;
}

void _uga_thread_sleep ( _timespec_t const * time )
{
        uga_clr_errs() ;

        i32_t err = thrd_sleep( time, NULL ) ;
        _uga_check_thrd_err( err, uga_thread_self() ) ;
}

void uga_thread_sleep_ns ( i64_t const nanos )
{
        _timespec_t sleepytime = uga_time_empty() ;
        sleepytime.tv_nsec += nanos ;

        _uga_thread_sleep( &sleepytime ) ;
}

void uga_thread_sleep_ms ( i64_t const millis )
{
        _timespec_t sleepytime = uga_time_empty() ;
        sleepytime.tv_nsec += millis * 1000000 ;

        _uga_thread_sleep( &sleepytime ) ;
}

void uga_thread_sleep_s ( i64_t const secs )
{
        _timespec_t sleepytime = uga_time_empty() ;
        sleepytime.tv_sec += secs ;

        _uga_thread_sleep( &sleepytime ) ;
}

uga_thread_id uga_thread_self ( void )
{
        return pthread_self() ;
}


uga_mtx_t uga_mtx_init ( uga_mtx_type const type )
{
        uga_clr_errs() ;

        uga_mtx_t mtx ;

        i32_t err = mtx_init( &mtx, type ) ;
        _uga_check_thrd_err( err, uga_thread_self() ) ;

        return mtx ;
}

i32_t uga_mtx_acquire ( uga_mtx_t * mtx )
{
        uga_clr_errs() ;

        i32_t err = mtx_lock( mtx ) ;
        _uga_check_thrd_err( err, uga_thread_self() ) ;

        return err ;
}

i32_t uga_mtx_release ( uga_mtx_t * mtx )
{
        uga_clr_errs() ;

        i32_t err = mtx_unlock( mtx ) ;
        _uga_check_thrd_err( err, uga_thread_self() ) ;

        return err ;
}

i32_t _uga_mtx_acquire_timed ( uga_mtx_t * mtx, _timespec_t const * timeout )
{
        uga_clr_errs() ;

        i32_t err = mtx_timedlock( mtx, timeout ) ;
        _uga_check_thrd_err( err, uga_thread_self() ) ;

        return err ;
}

i32_t uga_mtx_acquire_timed_ns ( uga_mtx_t * mtx, i64_t const nanos )
{
        _timespec_t timeout = uga_time_current() ;
        timeout.tv_nsec += nanos ;

        return _uga_mtx_acquire_timed( mtx, &timeout ) ;
}

i32_t uga_mtx_acquire_timed_ms ( uga_mtx_t * mtx, i64_t const millis )
{
        _timespec_t timeout = uga_time_current() ;
        timeout.tv_nsec += millis * 1000000 ;

        return _uga_mtx_acquire_timed( mtx, &timeout ) ;
}

i32_t uga_mtx_acquire_timed_s ( uga_mtx_t * mtx, i64_t const secs )
{
        _timespec_t timeout = uga_time_current() ;
        timeout.tv_sec += secs ;

        return _uga_mtx_acquire_timed( mtx, &timeout ) ;
}

void uga_mtx_destroy ( uga_mtx_t * mtx )
{
        mtx_destroy( mtx ) ;
}


uga_cnd_t uga_cnd_init ( void )
{
        uga_clr_errs() ;

        uga_cnd_t cnd ;

        i32_t err = cnd_init( &cnd ) ;
        _uga_check_thrd_err( err, uga_thread_self() ) ;

        return cnd ;
}

i32_t uga_cnd_wait ( uga_cnd_t * cnd, uga_mtx_t * mtx )
{
        uga_clr_errs() ;

        i32_t err = cnd_wait( cnd, mtx ) ;
        _uga_check_thrd_err( err, uga_thread_self() ) ;

        return err ;
}

i32_t _uga_cnd_wait_timed ( uga_cnd_t * cnd, uga_mtx_t * mtx, _timespec_t const * timeout )
{
        uga_clr_errs() ;

        i32_t err = cnd_timedwait( cnd, mtx, timeout ) ;
        _uga_check_thrd_err( err, uga_thread_self() ) ;

        return err ;
}

i32_t uga_cnd_wait_timed_ns ( uga_cnd_t * cnd, uga_mtx_t * mtx, i64_t const nanos )
{
        _timespec_t timeout = uga_time_current() ;
        timeout.tv_nsec += nanos ;

        return _uga_cnd_wait_timed( cnd, mtx, &timeout ) ;
}

i32_t uga_cnd_wait_timed_ms ( uga_cnd_t * cnd, uga_mtx_t * mtx, i64_t const millis )
{
        _timespec_t timeout = uga_time_current() ;
        timeout.tv_nsec += millis * 1000000 ;

        return _uga_cnd_wait_timed( cnd, mtx, &timeout ) ;
}

i32_t uga_cnd_wait_timed_s ( uga_cnd_t * cnd, uga_mtx_t * mtx, i64_t const secs )
{
        _timespec_t timeout = uga_time_current() ;
        timeout.tv_sec += secs ;

        return _uga_cnd_wait_timed( cnd, mtx, &timeout ) ;
}

i32_t uga_cnd_ping ( uga_cnd_t * cnd )
{
        uga_clr_errs() ;

        i32_t err = cnd_signal( cnd ) ;
        _uga_check_thrd_err( err, uga_thread_self() ) ;

        return err ;
}

i32_t uga_cnd_ping_all ( uga_cnd_t * cnd )
{
        uga_clr_errs() ;

        i32_t err = cnd_broadcast( cnd ) ;
        _uga_check_thrd_err( err, uga_thread_self() ) ;

        return err ;
}

void uga_cnd_destroy ( uga_cnd_t * cnd )
{
        cnd_destroy( cnd ) ;
}


uga_semaphore uga_sem_init ( i32_t init_val )
{
        uga_semaphore sem ;

        sem.val = init_val ;

        sem.mtx = uga_mtx_init( UGA_MTX_PLAIN ) ;
        sem.cnd = uga_cnd_init(               ) ;

        return sem ;
}

void uga_sem_acquire ( uga_semaphore * sem )
{
        UGA_DBG_S( "uga::sem::acquire", "acquiring sem mtx..." ) ;
        uga_mtx_acquire( &sem->mtx ) ;
        UGA_DBG_S( "uga::sem::acquire", "sem mtx acquired" ) ;

        if( sem->val > 0 )
        {
                UGA_DBG_S( "uga::sem::acquire", "semaphore green, decreasing and releasing..." ) ;
                --sem->val ;
                uga_mtx_release( &sem->mtx ) ;
                return ;
        }
        UGA_DBG_S( "uga::sem::acquire", "semaphore red, waiting for a release..." ) ;
        uga_cnd_wait( &sem->cnd, &sem->mtx ) ;
        UGA_DBG_S( "uga::sem::acquire", "got a green, decreasing and releasing..." ) ;
        --sem->val ;
        uga_mtx_release( &sem->mtx ) ;
}

void uga_sem_release ( uga_semaphore * sem )
{
        UGA_DBG_S( "uga::sem::release", "acquiring sem mtx..." ) ;
        uga_mtx_acquire( &sem->mtx ) ;
        UGA_DBG_S( "uga::sem::release", "sem mtx acquired" ) ;

        ++sem->val ;

        UGA_DBG_S( "uga::sem::release", "value increased, pinging and releasing..." ) ;
        uga_cnd_ping( &sem->cnd ) ;
        uga_mtx_release( &sem->mtx ) ;
}

void uga_sem_release_n ( uga_semaphore * sem, i32_t count )
{
        UGA_DBG_S( "uga::sem::release_n", "acquiring sem mtx..." ) ;
        uga_mtx_acquire( &sem->mtx ) ;
        UGA_DBG_S( "uga::sem::release_n", "sem mtx acquired" ) ;

        sem->val += count ;

        UGA_DBG_S( "uga::sem::release_n", "value increased, pinging all and releasing..." ) ;
        uga_cnd_ping_all( &sem->cnd ) ;
        uga_mtx_release( &sem->mtx ) ;
}

void uga_sem_destroy ( uga_semaphore * sem )
{
        uga_mtx_destroy( &sem->mtx ) ;
        uga_cnd_destroy( &sem->cnd ) ;
}
