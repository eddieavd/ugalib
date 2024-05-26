//
//
//      uganet
//      uga_err.c
//

#include <core/uga_err.h>
#include <core/uga_log.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <threads.h>


static uga_error uga_errdata = { UGA_ERR_NONE, UGA_CAT_NONE, { 0 } } ;

char const * uga_strerror ( uga_errtype const err )
{
        switch( err )
        {
                case UGA_ERR_NONE:
                        return "success" ;
                case UGA_ERR_STD:
                        return strerror( ( ( uga_error_std * ) &uga_errdata )->err_no ) ;
                case UGA_ERR_GAI:
                        return gai_strerror( ( ( uga_error_gai * ) &uga_errdata )->err_no ) ;
                case UGA_ERR_BAD_ARG:
                        return "bad argument" ;
                case UGA_ERR_SOCKOPT:
                        return "sockopt" ;
                case UGA_ERR_BAD_SFAM:
                        return "bad socket family" ;
                case UGA_ERR_BAD_ALLOC:
                        return "bad alloc" ;
                case UGA_ERR_BAD_REALLOC:
                        return "bad realloc" ;
                case UGA_ERR_BAD_ACCESS:
                        return "bad access" ;
                case UGA_ERR_PARTIAL_READ:
                        return "partial read" ;
                case UGA_ERR_PARTIAL_WRITE:
                        return "partial write" ;
                case UGA_ERR_CLI:
                        return "cli" ;
                case UGA_ERR_THRD_NOMEM:
                        return "thread no mem" ;
                case UGA_ERR_THRD_TIMEDOUT:
                        return "thread timed out" ;
                case UGA_ERR_THRD_BUSY:
                        return "thread busy" ;
                case UGA_ERR_UNKNOWN:
                        return "unknown" ;
                default:
                        return "unexpected" ;
        }
}

i32_t uga_had_errs ( void )
{
        return uga_errdata.type != UGA_ERR_NONE ;
}

void uga_clr_errs ( void )
{
        memset( &uga_errdata, 0, sizeof( uga_errdata ) ) ;
}

uga_error       const * uga_get_errdata       ( void ) { return                             &uga_errdata ; }
uga_error_std   const * uga_get_std_errdata   ( void ) { return ( uga_error_std   const * ) &uga_errdata ; }
uga_error_gai   const * uga_get_gai_errdata   ( void ) { return ( uga_error_gai   const * ) &uga_errdata ; }
uga_error_io    const * uga_get_io_errdata    ( void ) { return ( uga_error_io    const * ) &uga_errdata ; }
uga_error_alloc const * uga_get_alloc_errdata ( void ) { return ( uga_error_alloc const * ) &uga_errdata ; }
uga_error_mem   const * uga_get_mem_errdata   ( void ) { return ( uga_error_mem   const * ) &uga_errdata ; }
uga_error_cli   const * uga_get_cli_errdata   ( void ) { return ( uga_error_cli   const * ) &uga_errdata ; }
uga_error_thrd  const * uga_get_thrd_errdata  ( void ) { return ( uga_error_thrd  const * ) &uga_errdata ; }

void uga_set_error ( uga_errtype const type, uga_err_category const cat )
{
        uga_clr_errs() ;

        uga_errdata.type = type ;
        uga_errdata. cat =  cat ;
}

void uga_set_std_error ( i32_t const err_no, i32_t const data )
{
        uga_error_std * err = ( uga_error_std * ) &uga_errdata ;

        err->  type = UGA_ERR_STD ;
        err->   cat = UGA_CAT_STD ;
        err->err_no =      err_no ;
        err->  data =        data ;
}

void uga_set_gai_error ( i32_t const err_no, i32_t const data )
{
        uga_error_gai * err = ( uga_error_gai * ) &uga_errdata ;

        err->  type = UGA_ERR_GAI ;
        err->   cat = UGA_CAT_GAI ;
        err->err_no =      err_no ;
        err->  data =        data ;
}

void uga_set_io_error ( uga_errtype const type, i32_t const wanted, i32_t const got )
{
        uga_error_io * err = ( uga_error_io * ) &uga_errdata ;

        err->  type =       type ;
        err->   cat = UGA_CAT_IO ;
        err->wanted =     wanted ;
        err->   got =        got ;
}

void uga_set_alloc_error ( uga_errtype const type, i64_t const wanted )
{
        uga_error_alloc * err = ( uga_error_alloc * ) &uga_errdata ;

        err->  type =          type ;
        err->   cat = UGA_CAT_ALLOC ;
        err->wanted =        wanted ;

}

void uga_set_mem_error ( uga_errtype const type, i32_t const size, i32_t const pos )
{
        uga_error_mem * err = ( uga_error_mem * ) &uga_errdata ;

        err->type =        type ;
        err-> cat = UGA_CAT_MEM ;
        err->size =        size ;
        err-> pos =         pos ;
}

void uga_set_cli_error ( uga_errtype const type )
{
        uga_error_cli * err = ( uga_error_cli * ) &uga_errdata ;

        err->type =        type ;
        err-> cat = UGA_CAT_CLI ;
}

void uga_set_thrd_error ( uga_errtype const type, i32_t const thrd_e, uga_thread_id const thrd_id )
{
        uga_error_thrd * err = ( uga_error_thrd * ) &uga_errdata ;

        err->     type =           type ;
        err->      cat = UGA_CAT_THREAD ;
        err->      err =         thrd_e ;
        err->thread_id =        thrd_id ;
}

uga_error   uga_current_error   ( void ) { return uga_errdata      ; }
uga_errtype uga_current_errtype ( void ) { return uga_errdata.type ; }

void _uga_print_errtype ( uga_errtype const errtype, uga_log_lvl const level )
{
        uga_log_2( level, UGA_LOG_LONG, "%s", uga_strerror( errtype ) ) ;
}

void uga_print_errtype ( void )
{
        _uga_print_errtype( uga_errdata.type, UGA_LVL_ERR ) ;
}

void uga_log_errtype ( uga_log_lvl const level )
{
        _uga_print_errtype( uga_errdata.type, level ) ;
}

void _uga_print_std_err   ( uga_error_std   const * error, uga_log_lvl const level ) ;
void _uga_print_gai_err   ( uga_error_gai   const * error, uga_log_lvl const level ) ;
void _uga_print_io_err    ( uga_error_io    const * error, uga_log_lvl const level ) ;
void _uga_print_alloc_err ( uga_error_alloc const * error, uga_log_lvl const level ) ;
void _uga_print_mem_err   ( uga_error_mem   const * error, uga_log_lvl const level ) ;
void _uga_print_cli_err   ( uga_error_cli   const * error, uga_log_lvl const level ) ;
void _uga_print_thrd_err  ( uga_error_thrd  const * error, uga_log_lvl const level ) ;

void _uga_print_error ( uga_error const * error, uga_log_lvl const level )
{
        switch( error->cat )
        {
                case UGA_CAT_STD:
                        _uga_print_std_err( uga_get_std_errdata(), level ) ;
                        break ;
                case UGA_CAT_GAI:
                        _uga_print_gai_err( uga_get_gai_errdata(), level ) ;
                        break ;
                case UGA_CAT_IO:
                        _uga_print_io_err( uga_get_io_errdata(), level ) ;
                        break ;
                case UGA_CAT_ALLOC:
                        _uga_print_alloc_err( uga_get_alloc_errdata(), level ) ;
                        break ;
                case UGA_CAT_MEM:
                        _uga_print_mem_err( uga_get_mem_errdata(), level ) ;
                        break ;
                case UGA_CAT_CLI:
                        _uga_print_cli_err( uga_get_cli_errdata(), level ) ;
                        break ;
                case UGA_CAT_THREAD:
                        _uga_print_thrd_err( uga_get_thrd_errdata(), level ) ;
                        break ;
                case UGA_CAT_UNKNOWN:
                        UGA_ERR( "unknown error category" ) ;
                        break ;
                default:
                        UGA_ERR( "unexpected error category" ) ;
                        break ;
        }
}

void uga_print_error ( void )
{
        _uga_print_error( &uga_errdata, UGA_LVL_ERR ) ;
}

void uga_log_error ( uga_log_lvl const level )
{
        _uga_print_error( &uga_errdata, level ) ;
}

void uga_print_if_err ( void )
{
        if( uga_had_errs() )
        {
                uga_print_error() ;
                uga_clr_errs() ;
        }
}

void uga_log_if_err ( uga_log_lvl const level )
{
        if( uga_had_errs() )
        {
                uga_log_error( level ) ;
                uga_clr_errs() ;
        }
}

void uga_abort_if_err ( void )
{
        if( uga_had_errs() )
        {
                exit( uga_errdata.type ) ;
        }
}

void uga_print_abort_if_err ( void )
{
        if( uga_had_errs() )
        {
                uga_print_error() ;
                exit( uga_errdata.type ) ;
        }
}

////////////////////////////////////////////////////////////////////////////////
/// private
////////////////////////////////////////////////////////////////////////////////

void _uga_check_std_err ( i32_t const std_errno )
{
        if( std_errno != 0 )
        {
                UGA_WARN_S( "uga::err", "possible std error: %s", strerror( errno ) ) ;
                uga_set_std_error( errno, 0 ) ;
        }
}

void _uga_check_std_errno ( void )
{
        _uga_check_std_err( errno ) ;
}

void _uga_check_thrd_err ( i32_t const thrd_err, uga_thread_id const thrd_id )
{
        if( thrd_err != thrd_success )
        {
                UGA_WARN_S( "uga::err", "possible thread error" ) ;
                uga_set_thrd_error( UGA_ERR_NONE, thrd_err, thrd_id ) ;
        }
}

void _uga_print_std_err ( uga_error_std const * error, uga_log_lvl const level )
{
        ( void ) error ;
        ( void ) level ;
        UGA_ERR( "libc error: %s", strerror( error->err_no ) ) ;
}

void _uga_print_gai_err ( uga_error_gai const * error, uga_log_lvl const level )
{
        ( void ) error ;
        ( void ) level ;
        UGA_ERR( "gai error: %s", gai_strerror( error->err_no ) ) ;
}

void _uga_print_io_err ( uga_error_io const * error, uga_log_lvl const level  )
{
        ( void ) error ;
        ( void ) level ;
        UGA_ERR( "io error: partial read/write, wanted %db, got %db", error->wanted, error->got ) ;
}

void _uga_print_alloc_err ( uga_error_alloc const * error, uga_log_lvl const level  )
{
        ( void ) error ;
        ( void ) level ;
        UGA_ERR( "alloc error: failed to alloc %lldb", error->wanted ) ;
}

void _uga_print_mem_err ( uga_error_mem const * error, uga_log_lvl const level  )
{
        ( void ) error ;
        ( void ) level ;
        UGA_ERR( "mem error: block size is %d, bad access at %d", error->size, error->pos ) ;
}

void _uga_print_cli_err ( uga_error_cli const * error, uga_log_lvl const level )
{
        ( void ) error ;
        ( void ) level ;

        UGA_ERR( "cli error: unknown option or missing option value" ) ;
}

void _uga_print_thrd_err ( uga_error_thrd const * error, uga_log_lvl const level )
{
        ( void ) error ;
        ( void ) level ;

        UGA_ERR( "%s on thread %ull", uga_strerror( error->type ), error->thread_id ) ;
}
