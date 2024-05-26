//
//
//      uganet
//      demo.c
//

#include <core/uga_log.h>
#include <core/uga_err.h>
#include <core/uga_alloc.h>
#include <core/uga_cli.h>
#include <core/uga_str.h>
#include <core/uga_strview.h>
#include <core/uga_vector.h>
#include <core/uga_thread.h>
#include <core/uga_pool.h>

#include <net/uga_sock.h>
#include <net/uga_talk.h>
#include <net/uga_async.h>

#define CLI_ARGC    1
#define CLI_DAEMON 'd'


/*
void print_help ( void ) ;

uga_cli_args prepare_args ( void ) ;

i32_t client_handler ( void * ) ;
*/

i32_t work ( void * data )
{
        UGA_DBG_S( "worker", "started." ) ;

        uga_string * datastr = ( uga_string * ) data ;

        uga_vector nums = uga_vec_copy( ( uga_vector * ) datastr->data ) ;

        for( i64_t i = 0; i < nums.size; ++i )
        {
                i64_t * val = uga_vec_at( &nums, i ) ;
                UGA_DBG_S( "worker::loop", "current value is %lld", *val ) ;
                *val += 1 ;
                UGA_DBG_S( "worker::loop", "new value is %lld", *val ) ;
        }
        UGA_DBG_S( "worker", "finished." ) ;
        return 0 ;
}

#define NUM_VALS    4
#define NUM_THREADS 3

i32_t work_1 ( void * arg )
{
        UGA_INFO_S( "worker", "started." ) ;
        uga_vector * task_data = ( uga_vector * ) arg ;
        UGA_INFO_S( "worker", "got pointer to vector, size is %lld", task_data->size ) ;
        uga_string * data = uga_vec_at( task_data, 0 ) ;

        UGA_INFO_S( "worker", "worker %lld printing main message: "STR_FMT, uga_thread_self(), STR_ARG( *data ) ) ;

        uga_thread_sleep_ms( 50 ) ;

        UGA_INFO_S( "worker", "finished." ) ;

        return 0 ;
}

int main ( i32_t argc, char ** argv )
{
        ( void ) argc ;
        ( void ) argv ;

        uga_log_init() ;

        UGA_LOG( 10, UGA_LOG_LONG, "some log" ) ;

        UGA_INFO_S( "main", "logs initialized" ) ;

        uga_thread_pool * pool = uga_pool_create( 4 ) ;

        UGA_INFO_S( "main", "created thread pool" ) ;

        uga_string message_1 = uga_str_create_from_1( "MSG_A" ) ;
        uga_string message_2 = uga_str_create_from_1( "MSG_B" ) ;
        uga_string message_3 = uga_str_create_from_1( "MSG_C" ) ;
        uga_string message_4 = uga_str_create_from_1( "MSG_D" ) ;

        uga_vector vec_1 = uga_vec_create_1( uga_string, 1 ) ;
        uga_vector vec_2 = uga_vec_create_1( uga_string, 1 ) ;
        uga_vector vec_3 = uga_vec_create_1( uga_string, 1 ) ;
        uga_vector vec_4 = uga_vec_create_1( uga_string, 1 ) ;

        uga_vec_push_back( &vec_1, &message_1 ) ;
        uga_vec_push_back( &vec_2, &message_2 ) ;
        uga_vec_push_back( &vec_3, &message_3 ) ;
        uga_vec_push_back( &vec_4, &message_4 ) ;

        UGA_INFO_S( "main", "created messages" ) ;

        uga_task task_1 = { vec_1, work_1 } ;
        uga_task task_2 = { vec_2, work_1 } ;
        uga_task task_3 = { vec_3, work_1 } ;
        uga_task task_4 = { vec_4, work_1 } ;

        UGA_INFO_S( "main", "created tasks" ) ;
        UGA_INFO_S( "main", "adding tasks..." ) ;

        uga_pool_add_task( pool, task_1 ) ;
        UGA_INFO_S( "main", "added task 1" ) ;
        uga_pool_add_task( pool, task_2 ) ;
        UGA_INFO_S( "main", "added task 2" ) ;
        uga_pool_add_task( pool, task_3 ) ;
        UGA_INFO_S( "main", "added task 3" ) ;
        uga_pool_add_task( pool, task_4 ) ;
        UGA_INFO_S( "main", "added task 4" ) ;

        uga_pool_add_task( pool, task_1 ) ;
        UGA_INFO_S( "main", "added task 1" ) ;
        uga_pool_add_task( pool, task_2 ) ;
        UGA_INFO_S( "main", "added task 2" ) ;
        uga_pool_add_task( pool, task_3 ) ;
        UGA_INFO_S( "main", "added task 3" ) ;
        uga_pool_add_task( pool, task_4 ) ;
        UGA_INFO_S( "main", "added task 4" ) ;

        uga_thread_sleep_s( 1 ) ;

        UGA_INFO_S( "main", "waiting for workers..." ) ;
        uga_pool_destroy( pool ) ;
        UGA_INFO_S( "main", "destroying resources..." ) ;
        uga_str_destroy( &message_1 ) ;
        uga_str_destroy( &message_2 ) ;
        uga_str_destroy( &message_3 ) ;
        uga_str_destroy( &message_4 ) ;

        UGA_INFO_S( "main", "finished." ) ;

        return 0 ;

        /*
        uga_vector data = uga_vec_create_1( i64_t, NUM_VALS ) ;

        for( i64_t i = 0; i < NUM_VALS; ++i )
        {
                uga_vec_push_back( &data, &i ) ;
        }
        uga_string datastr = uga_str_create_from_2( ( char const * ) &data, sizeof( data ) ) ;

        UGA_DBG_S( "main", "created data. vec.size is %lld, str.size is %lld, sizeof( vec ) is %lld", data.size, datastr.size, sizeof( data ) ) ;

        uga_task   task = { datastr, work } ;
        uga_vector threads = uga_vec_create_1( uga_thread, NUM_THREADS ) ;

        UGA_DBG_S( "main", "starting %lld threads...", threads.capacity ) ;
        for( i64_t i = 0; i < NUM_THREADS; ++i )
        {
                uga_thread thread = uga_thread_do_task( task ) ;
                uga_vec_push_back( &threads, &thread ) ;
        }
        UGA_DBG_S( "main", "%lld threads started.", threads.size ) ;
        UGA_DBG_S( "main", "joining threads..." ) ;
        for( i64_t i = 0; i < threads.size; ++i )
        {
                uga_thread * thread = uga_vec_at( &threads, i ) ;
                uga_thread_join( *thread ) ;
        }
        UGA_DBG_S( "main", "threads joined." ) ;
        UGA_DBG_S( "main", "checking results..." ) ;
        for( i64_t i = 0; i < threads.size; ++i )
        {
                uga_thread * thread = uga_vec_at( &threads, i ) ;

                UGA_DBG_S( "main", "checking thread %llu...", thread->id ) ;

                uga_vector * thread_data = ( uga_vector * ) thread->task.data.data ;

                for( i64_t j = 0; j < thread_data->size; ++j )
                {
                        i64_t * val = uga_vec_at( thread_data, j ) ;

                        UGA_DBG_S( "main", "value at index %lld is %lld", j, *val ) ;

                        if( *val != j + 1 )
                        {
                                UGA_ERR_S( "main", "found bad value" ) ;
                        }
                }
//                uga_vec_destroy(  thread_data       ) ;
//                uga_str_destroy( &thread->task.data ) ;
        }
        UGA_INFO_S( "main", "finished" ) ;

        uga_str_destroy( &datastr ) ;
        uga_vec_destroy( &data    ) ;


        return 0 ;
        */

        /*
        uga_cli_args args = prepare_args() ;
        i32_t parsed = uga_cli_parse_args( argc, argv, &args ) ;
        i32_t argcount = argc - parsed ;
        if( argcount < 2 )
        {
                print_help() ;
                return 1 ;
        }
        uga_string_view port = uga_sv_create_1( argv[ argc - 1 ] ) ;
        if( uga_cli_is_set( CLI_DAEMON, &args ) )
        {
                UGA_INFO_S( "main", "starting server in daemon mode on port "SV_FMT, SV_ARG( port ) ) ;

        }
        else
        {
                UGA_INFO_S( "main", "starting server on port "SV_FMT, SV_ARG( port ) ) ;
        }
        UGA_INFO_S( "main", "finished." ) ;


        return 0 ;
        */
}

void print_help ( void )
{
        UGA_ERR_S( "tftpserver", "usage: ./tftpserver [ -d ] port_name_or_number" ) ;
}

uga_cli_args prepare_args ( void )
{
        uga_cli_args args = { 0 } ;

        args.arg_count = CLI_ARGC ;

        args.args[ 0 ].name = CLI_DAEMON ;
        args.args[ 0 ].has_value = false ;

        return args ;
}
