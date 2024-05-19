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


i32_t work ( void * arg )
{
        uga_vector * thread_data = ( uga_vector * ) arg ;
        uga_vector data = uga_vec_copy( uga_vec_at( thread_data, 0 ) ) ;

        for( i32_t i = 0; i < data.size; ++i )
        {
                i32_t * val = uga_vec_at( &data, i ) ;
                UGA_INFO_S( "worker", "current value: %d", *val ) ;
                ++( *val ) ;
                UGA_INFO_S( "worker", "new value: %d", *val ) ;
        }
        uga_vec_destroy( thread_data ) ;
        *thread_data = uga_vec_copy( &data ) ;

        return 0 ;
}

int main ( void )
{
        uga_log_init() ;

        UGA_INFO_S( "main", "started." ) ;

        uga_vector data = uga_vec_create_1( i32_t, 4 ) ;
        for( i32_t i = 0; i < 4; ++i )
        {
                uga_vec_push_back( &data, &i ) ;
        }
        UGA_INFO_S( "main", "created data." ) ;

        uga_vector thread_data = uga_vec_create_1( uga_vector, 1 ) ;
        uga_vec_push_back( &thread_data, &data ) ;

        uga_task increment_task = { thread_data, work } ;

        UGA_INFO_S( "main", "created task." ) ;

        uga_thread_pool * pool = uga_pool_create_new( 1 ) ;

        UGA_INFO_S( "main", "created thread pool." ) ;
        UGA_INFO_S( "main", "adding tasks..." ) ;

        for( i64_t i = 0; i < 1; ++i )
        {
                uga_pool_add_task( pool, increment_task ) ;
        }
        UGA_INFO_S( "main", "tasks added, waiting for threads to finish..." ) ;
        uga_thread_sleep_s( 1 ) ;

        UGA_INFO_S( "main", "getting results..." ) ;
        uga_dl_list * results = uga_pool_get_finished_tasks( pool ) ;

        for( i32_t i = 0; i < results->size; ++i )
        {
                UGA_INFO_S( "main", "checking results for task %d", i ) ;
                uga_task * task_result = ( uga_task * ) uga_dl_list_at( results, i ) ;

                uga_vector * task_data = &task_result->data ;

                for( i32_t j = 0; j < task_data->size; ++j )
                {
                        UGA_INFO_S( "main", "value at %d is %d", j, *( i32_t * ) uga_vec_at( task_data, j ) ) ;
                }
                uga_vec_destroy( task_data ) ;
        }
        uga_pool_destroy( pool ) ;
        uga_vec_destroy( &data ) ;





        return 0 ;
}
