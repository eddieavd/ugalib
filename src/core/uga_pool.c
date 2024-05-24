//
//
//      uganet
//      uga_pool.c
//

#include <core/uga_pool.h>
#include <core/uga_log.h>
#include <core/uga_err.h>
#include <core/uga_str.h>
#include <core/uga_alloc.h>
#include <core/uga_thread.h>


i32_t _uga_pool_poll ( void * parent_pool_ptr ) ;

uga_thread_pool * uga_pool_create_new ( i32_t const num_threads )
{
        uga_thread_pool * poolptr = uga_allocate( sizeof( uga_thread_pool ) ) ;

        UGA_DBG_S( "uga::pool::create_new", "allocated pool" ) ;

        poolptr->is_active = true ;

        poolptr->tasks  = uga_dl_list_create( uga_task ) ;
        poolptr->t_done = uga_dl_list_create( uga_task ) ;
        poolptr-> pool  = uga_vec_create_1( uga_thread *, num_threads ) ;

        UGA_DBG_S( "uga::pool::create_new", "allocated task and thread vectors" ) ;

        poolptr->task_mtx = uga_mtx_init( UGA_MTX_PLAIN ) ;
        poolptr->done_mtx = uga_mtx_init( UGA_MTX_PLAIN ) ;
        poolptr->   t_sem = uga_sem_init(             0 ) ;

        UGA_DBG_S( "uga::pool::create_new", "initialized mutex and condition members" ) ;

        uga_vector poll_data = uga_vec_create_1( uga_thread_pool *, 1 ) ;
        uga_vec_push_back( &poll_data, &poolptr ) ;

        uga_task poll_task = { poll_data, _uga_pool_poll } ;

        UGA_DBG_S( "uga::pool::create_new", "created polling task" ) ;
        UGA_DBG_S( "uga::pool::create_new", "creating threads..." ) ;

        for( i64_t i = 0; i < num_threads; ++i )
        {
                uga_thread * thread = uga_thread_do_task( poll_task ) ;
                uga_vec_push_back( &poolptr->pool, &thread ) ;
        }
        UGA_DBG_S( "uga::pool::create_new", "created %lld threads", poolptr->pool.size ) ;

        uga_vec_destroy( &poll_data ) ;

        return poolptr ;
}

void uga_pool_add_task ( uga_thread_pool * pool, uga_task task )
{
        uga_mtx_acquire( &pool->task_mtx ) ;

        uga_dl_list_push_back( &pool->tasks, &task ) ;
        UGA_DBG_S( "uga::pool::add_task", "pushed task to queue" ) ;

        uga_sem_release( &pool->   t_sem ) ;
        uga_mtx_release( &pool->task_mtx ) ;
        UGA_DBG_S( "uga::pool::add_task", "signaled threads" ) ;
}

i32_t _uga_pool_poll ( void * parent_pool_ptr )
{
        UGA_DBG_S( "worker::uga::pool::poll", "started." ) ;

        uga_vector * task_data = ( uga_vector * ) parent_pool_ptr ;
        uga_thread_pool * pool = *( uga_thread_pool ** ) uga_vec_at( task_data, 0 ) ;

        UGA_DBG_S( "worker::uga::pool::poll", "retrieved pointer to pool." ) ;

        while( pool->is_active )
        {
                uga_task task = { 0 } ;

                UGA_DBG_S( "worker::uga::pool::poll", "waiting on semaphore" ) ;
                uga_sem_acquire( &pool->   t_sem ) ;
                uga_mtx_acquire( &pool->task_mtx ) ;
                UGA_DBG_S( "worker::uga::pool::poll", "entered critical section." ) ;

                if( !pool->is_active )
                {
                        uga_mtx_release( &pool->task_mtx ) ;
                        break ;
                }
                if( !uga_dl_list_empty( &pool->tasks ) )
                {
                        UGA_DBG_S( "worker::uga:pool::poll", "found a task." ) ;
                        uga_dl_list_pop_front( &pool->tasks, &task ) ;
                }
                uga_mtx_release( &pool->task_mtx ) ;
                UGA_DBG_S( "worker::uga::pool::poll", "released mutex." ) ;

                if( task.run )
                {
                        UGA_DBG_S( "worker::uga::pool::poll", "executing task..." ) ;
                        task.run( &task.data ) ;
                        UGA_DBG_S( "worker::uga::pool::poll", "task finished." ) ;
                        uga_mtx_acquire( &pool->done_mtx ) ;
                        uga_dl_list_push_back( &pool->t_done, &task ) ;
                        uga_mtx_release( &pool->done_mtx ) ;
                }
        }
        UGA_DBG_S( "worker::uga::pool::poll", "finished." ) ;
        return 0 ;
}

uga_dl_list * uga_pool_get_finished_tasks ( uga_thread_pool * pool )
{
        return &pool->t_done ;
}

void uga_pool_clear_finished_tasks ( uga_thread_pool * pool )
{
        uga_mtx_acquire( &pool->done_mtx ) ;
        uga_dl_list_clear( &pool->t_done ) ;
        uga_mtx_release( &pool->done_mtx ) ;
}

void uga_pool_destroy ( uga_thread_pool * pool )
{
        pool->is_active = false ;
        UGA_DBG_S( "uga::pool::destroy", "status set to inactive." ) ;

        UGA_DBG_S( "uga::pool::destroy", "signaling threads..." ) ;
        uga_sem_release_n( &pool->t_sem, pool->pool.size ) ;
        UGA_DBG_S( "uga::pool::destroy", "joining threads..." ) ;
        for( i64_t i = 0; i < pool->pool.size; ++i )
        {
                uga_thread * thread = *( ( uga_thread ** ) uga_vec_at( &pool->pool, i ) ) ;
                uga_thread_join( *thread ) ;
        }
        UGA_DBG_S( "uga::pool::destroy", "destroying resources..." ) ;
        for( i64_t i = 0; i < pool->pool.size; ++i )
        {
                uga_thread * thread = *( uga_thread ** ) uga_vec_at( &pool->pool, i ) ;

                uga_vec_destroy( &thread->task.data ) ;
                uga_deallocate( thread ) ;
        }
        uga_vec_destroy( &pool->      pool ) ;
        uga_dl_list_destroy( &pool-> tasks ) ;
        uga_dl_list_destroy( &pool->t_done ) ;
        uga_mtx_destroy( &pool->  task_mtx ) ;
        uga_mtx_destroy( &pool->  done_mtx ) ;
        uga_sem_destroy( &pool->     t_sem ) ;

        uga_deallocate( pool ) ;
}

void uga_pool_destroy_void ( void * pool )
{
        uga_pool_destroy( pool ) ;
}
