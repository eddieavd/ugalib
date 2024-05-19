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

        poolptr->tasks = uga_vec_create_1( uga_task    ,          16 ) ;
        poolptr-> pool = uga_vec_create_1( uga_thread *, num_threads ) ;

        UGA_DBG_S( "uga::pool::create_new", "allocated task and thread vectors" ) ;

        poolptr->task_mtx = uga_mtx_init( UGA_MTX_PLAIN ) ;
        poolptr->   t_sem = uga_sem_init(             0 ) ;

        UGA_DBG_S( "uga::pool::create_new", "initialized mutex and condition members" ) ;

        uga_string poll_data = uga_str_create_from_2( ( char const * ) &poolptr, sizeof( void * ) ) ;

        uga_task poll_task = { poll_data, _uga_pool_poll } ;

        UGA_DBG_S( "uga::pool::create_new", "created polling task" ) ;
        UGA_DBG_S( "uga::pool::create_new", "creating threads..." ) ;

        for( i64_t i = 0; i < num_threads; ++i )
        {
                uga_thread * thread = uga_thread_do_task( poll_task ) ;
                uga_vec_push_back( &poolptr->pool, &thread ) ;
        }
        UGA_DBG_S( "uga::pool::create_new", "created %lld threads", poolptr->pool.size ) ;

        uga_str_destroy( &poll_data ) ;

        return poolptr ;
}

void uga_pool_add_task ( uga_thread_pool * pool, uga_task task )
{
        uga_mtx_acquire( &pool->task_mtx ) ;

        uga_vec_push_back( &pool->tasks, &task ) ;
        UGA_DBG_S( "uga::pool::add_task", "pushed task to queue" ) ;

        uga_sem_release( &pool->   t_sem ) ;
        uga_mtx_release( &pool->task_mtx ) ;
        UGA_DBG_S( "uga::pool::add_task", "signaled threads" ) ;
}

i32_t _uga_pool_poll ( void * parent_pool_ptr )
{
        UGA_DBG_S( "worker::uga::pool::poll", "started." ) ;

        uga_string * task_data =  ( uga_string       * ) parent_pool_ptr ;
        uga_thread_pool * pool = *( uga_thread_pool ** ) task_data->data ;

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
                if( !uga_vec_empty( &pool->tasks ) )
                {
                        UGA_DBG_S( "worker::uga:pool::poll", "found a task." ) ;
                        task = *( uga_task * ) uga_vec_at_ptr( &pool->tasks, 0 ) ;
                        uga_vec_erase_stable( &pool->tasks, 0 ) ;
                }
                uga_mtx_release( &pool->task_mtx ) ;
                UGA_DBG_S( "worker::uga::pool::poll", "released mutex." ) ;

                if( task.run )
                {
                        UGA_DBG_S( "worker::uga::pool::poll", "executing task..." ) ;
                        task.run( &task.data ) ;
                        UGA_DBG_S( "worker::uga::pool::poll", "task finished." ) ;
                }
        }
        UGA_DBG_S( "worker::uga::pool::poll", "finished." ) ;
        return 0 ;
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
                uga_thread * thread = *( ( uga_thread ** ) uga_vec_at_ptr( &pool->pool, i ) ) ;
                uga_thread_join( *thread ) ;
        }
        UGA_DBG_S( "uga::pool::destroy", "destroying resources..." ) ;
        for( i64_t i = 0; i < pool->pool.size; ++i )
        {
                uga_thread * thread = *( uga_thread ** ) uga_vec_at_ptr( &pool->pool, i ) ;

                uga_str_destroy( &thread->task.data ) ;
                uga_deallocate( thread ) ;
        }
        uga_vec_destroy( &pool->     pool ) ;
        uga_vec_destroy( &pool->    tasks ) ;
        uga_mtx_destroy( &pool-> task_mtx ) ;
        uga_sem_destroy( &pool->    t_sem ) ;
}

void uga_pool_destroy_void ( void * pool )
{
        uga_pool_destroy( pool ) ;
}
