//
//
//      uganet
//      uga_log.c
//

#include <core/uga_log.h>
#include <core/uga_types.h>
#include <core/uga_str.h>
#include <core/uga_thread.h>

#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define VTSEQ(ID) ("\x1b[" #ID "m")


uga_mtx_t _uga_log_mtx ;

_timespec_t uga_time_empty ( void )
{
        _timespec_t time = { 0 } ;
        return time ;
}

_timespec_t uga_time_current ( void )
{
        _timespec_t time ;
        clock_gettime( CLOCK_REALTIME, &time ) ;
        return time ;
}

_timespec_t uga_time_current_m ( void )
{
        _timespec_t time ;
        clock_gettime( CLOCK_MONOTONIC_RAW, &time ) ;
        return time ;
}

uga_string uga_time_string ( void )
{
        uga_string time_str = uga_str_create_1( 2 + 1 + 2 + 1 + 2 + 1 + 3 + 1 ) ; // HH:MM:SS:mmm\0

        _timespec_t time = uga_time_current() ;

        struct tm const * tms = localtime( &time.tv_sec ) ;

        i32_t msec = ( i32_t ) time.tv_nsec / 1000000 ;

        uga_str_push_back( &time_str, '0' + ( tms->tm_hour / 10 ) ) ;
        uga_str_push_back( &time_str, '0' + ( tms->tm_hour % 10 ) ) ;
        uga_str_push_back( &time_str, ':' ) ;
        uga_str_push_back( &time_str, '0' + ( tms->tm_min / 10 ) ) ;
        uga_str_push_back( &time_str, '0' + ( tms->tm_min % 10 ) ) ;
        uga_str_push_back( &time_str, ':' ) ;
        uga_str_push_back( &time_str, '0' + ( tms->tm_sec / 10 ) ) ;
        uga_str_push_back( &time_str, '0' + ( tms->tm_sec % 10 ) ) ;
        uga_str_push_back( &time_str, ':' ) ;
        uga_str_push_back( &time_str, '0' +   ( msec / 100 )        ) ;
        uga_str_push_back( &time_str, '0' + ( ( msec /  10 ) % 10 ) ) ;
        uga_str_push_back( &time_str, '0' +   ( msec %  10 )        ) ;

        return time_str ;
}

void uga_log_2 ( uga_log_lvl level, uga_log_type type, char const * fmt, ... )
{
        va_list args ;
        va_start( args, fmt ) ;
        uga_log_2_v( level, type, fmt, args ) ;
        va_end( args ) ;
}

void uga_log_3 ( uga_log_lvl level, uga_log_type type, char const * scope, char const * fmt, ... )
{
        va_list args ;
        va_start( args, fmt ) ;
        uga_log_3_v( level, type, scope, fmt, args ) ;
        va_end( args ) ;
}

void uga_log_2_v ( uga_log_lvl level, uga_log_type type, char const * fmt, va_list args )
{
        if( level > UGA_LOG_LVL ) return ;

        uga_string time = uga_time_string() ;

        uga_mtx_acquire( &_uga_log_mtx ) ;
        if( type == UGA_LOG_LONG )
        {
                switch( level )
                {
                        case UGA_LVL_DBG:
                                fprintf( stderr, "%s%s:: "STR_FMT" : %lu : UGA::DBG  : ", uga_log_purple(), uga_log_bold(), STR_ARG( time ), uga_thread_self() ) ;
                                break ;
                        case UGA_LVL_INFO:
                                fprintf( stderr, "%s:: "STR_FMT" : %lu : UGA::INFO : ", uga_log_bold(), STR_ARG( time ), uga_thread_self() ) ;
                                break ;
                        case UGA_LVL_WARN:
                                fprintf( stderr, "%s%s:: "STR_FMT" : %lu : UGA::WARN : ", uga_log_yellow(), uga_log_bold(), STR_ARG( time ), uga_thread_self() ) ;
                                break ;
                        case UGA_LVL_ERR:
                                fprintf( stderr, "%s%s:: "STR_FMT" : %lu : UGA::ERR  : ", uga_log_red(), uga_log_bold(), STR_ARG( time ), uga_thread_self() ) ;
                                break ;
                        case UGA_LVL_SUCC:
                                fprintf( stderr, "%s%s:: "STR_FMT" : %lu : UGA::SUCC : ", uga_log_green(), uga_log_bold(), STR_ARG( time ), uga_thread_self() ) ;
                                break ;
                        case UGA_LVL_FAIL:
                                fprintf( stderr, "%s%s:: "STR_FMT" : %lu : UGA::FAIL : ", uga_log_red(), uga_log_bold(), STR_ARG( time ), uga_thread_self() ) ;
                                break ;
                        default:
                                fprintf( stderr, "%s%s:: "STR_FMT" : %lu : UGA::%4d : ", uga_log_red(), uga_log_dim(),STR_ARG( time ), uga_thread_self(), level ) ;
                                break ;
                }
        }
        else if( type == UGA_LOG_SHORT )
        {
                switch( level )
                {
                        case UGA_LVL_DBG:
                                fprintf( stderr, "%s%sUGA::DBG  : ", uga_log_purple(), uga_log_bold() ) ;
                                break ;
                        case UGA_LVL_INFO:
                                fprintf( stderr, "%sUGA::INFO : ", uga_log_bold() ) ;
                                break ;
                        case UGA_LVL_WARN:
                                fprintf( stderr, "%s%sUGA::WARN : ", uga_log_yellow(), uga_log_bold() ) ;
                                break ;
                        case UGA_LVL_ERR:
                                fprintf( stderr, "%s%sUGA::ERR  : ", uga_log_red(), uga_log_bold() ) ;
                                break ;
                        case UGA_LVL_SUCC:
                                fprintf( stderr, "%s%sUGA::SUCC : ", uga_log_green(), uga_log_bold() ) ;
                                break ;
                        case UGA_LVL_FAIL:
                                fprintf( stderr, "%s%sUGA::FAIL : ", uga_log_red(), uga_log_bold() ) ;
                                break ;
                        default:
                                fprintf( stderr, "%s%sUGA::%4d : ", uga_log_red(), uga_log_dim(), level ) ;
                                break ;
                }
        }
        fprintf( stderr, "%s", uga_log_reset() ) ;

        vfprintf( stderr, fmt, args ) ;
        if( type == UGA_LOG_LONG )
        {
                fprintf( stderr, "\n" ) ;
        }
        uga_mtx_release( &_uga_log_mtx ) ;
        uga_str_destroy( &time ) ;
}

void uga_log_3_v ( uga_log_lvl level, uga_log_type type, char const * scope, char const * fmt, va_list args )
{
        if( level > UGA_LOG_LVL ) return ;

        uga_string time = uga_time_string() ;

        uga_mtx_acquire( &_uga_log_mtx ) ;
        if( type == UGA_LOG_LONG )
        {
                switch( level )
                {
                        case UGA_LVL_DBG:
                                fprintf( stderr, "%s%s:: "STR_FMT" : %lu : UGA::DBG  : %s : ", uga_log_purple(), uga_log_bold(), STR_ARG( time ), uga_thread_self(), scope ) ;
                                break ;
                        case UGA_LVL_INFO:
                                fprintf( stderr, "%s:: "STR_FMT" : %lu : UGA::INFO : %s : ", uga_log_bold(), STR_ARG( time ), uga_thread_self(), scope ) ;
                                break ;
                        case UGA_LVL_WARN:
                                fprintf( stderr, "%s%s:: "STR_FMT" : %lu : UGA::WARN : %s : ", uga_log_yellow(), uga_log_bold(), STR_ARG( time ), uga_thread_self(), scope ) ;
                                break ;
                        case UGA_LVL_ERR:
                                fprintf( stderr, "%s%s:: "STR_FMT" : %lu : UGA::ERR  : %s : ", uga_log_red(), uga_log_bold(), STR_ARG( time ), uga_thread_self(), scope ) ;
                                break ;
                        case UGA_LVL_SUCC:
                                fprintf( stderr, "%s%s:: "STR_FMT" : %lu : UGA::SUCC : %s : ", uga_log_green(), uga_log_bold(), STR_ARG( time ), uga_thread_self(), scope ) ;
                                break ;
                        case UGA_LVL_FAIL:
                                fprintf( stderr, "%s%s:: "STR_FMT" : %lu : UGA::FAIL : %s : ", uga_log_red(), uga_log_bold(), STR_ARG( time ), uga_thread_self(), scope ) ;
                                break ;
                        default:
                                fprintf( stderr, "%s%s:: "STR_FMT" : %lu : UGA::%.4d : %s : ", uga_log_red(), uga_log_dim(), STR_ARG( time ), uga_thread_self(), level, scope ) ;
                                break ;
                }
        }
        else if( type == UGA_LOG_SHORT )
        {
                switch( level )
                {
                        case UGA_LVL_DBG:
                                fprintf( stderr, "%s%s%s : ", uga_log_purple(), uga_log_bold(), scope ) ;
                                break ;
                        case UGA_LVL_INFO:
                                fprintf( stderr, "%s%s : ", uga_log_bold(), scope ) ;
                                break ;
                        case UGA_LVL_WARN:
                                fprintf( stderr, "%s%s%s : ", uga_log_yellow(), uga_log_bold(), scope ) ;
                                break ;
                        case UGA_LVL_ERR:
                                fprintf( stderr, "%s%s%s : ", uga_log_red(), uga_log_bold(), scope ) ;
                                break ;
                        case UGA_LVL_SUCC:
                                fprintf( stderr, "%s%s%s : ", uga_log_green(), uga_log_bold(), scope ) ;
                                break ;
                        case UGA_LVL_FAIL:
                                fprintf( stderr, "%s%s%s : ", uga_log_red(), uga_log_bold(), scope ) ;
                                break ;
                        default:
                                fprintf( stderr, "%s%s%s : ", uga_log_red(), uga_log_dim(), scope ) ;
                                break ;
                }
        }
        fprintf( stderr, "%s", uga_log_reset() ) ;

        vfprintf( stderr, fmt, args ) ;
        if( type == UGA_LOG_LONG )
        {
                fprintf( stderr, "\n" ) ;
        }
        uga_mtx_release( &_uga_log_mtx ) ;
        uga_str_destroy( &time ) ;
}


static bool uga_term_has_color = false ;

char const * uga_log_black      ( void ) { return uga_term_has_color ? VTSEQ( 30 ) : "" ; }
char const * uga_log_red        ( void ) { return uga_term_has_color ? VTSEQ( 31 ) : "" ; }
char const * uga_log_green      ( void ) { return uga_term_has_color ? VTSEQ( 32 ) : "" ; }
char const * uga_log_yellow     ( void ) { return uga_term_has_color ? VTSEQ( 33 ) : "" ; }
char const * uga_log_blue       ( void ) { return uga_term_has_color ? VTSEQ( 34 ) : "" ; }
char const * uga_log_purple     ( void ) { return uga_term_has_color ? VTSEQ( 35 ) : "" ; }
char const * uga_log_cyan       ( void ) { return uga_term_has_color ? VTSEQ( 36 ) : "" ; }
char const * uga_log_light_gray ( void ) { return uga_term_has_color ? VTSEQ( 37 ) : "" ; }
char const * uga_log_white      ( void ) { return uga_term_has_color ? VTSEQ( 37 ) : "" ; }
char const * uga_log_light_red  ( void ) { return uga_term_has_color ? VTSEQ( 91 ) : "" ; }

char const * uga_log_dim        ( void ) { return uga_term_has_color ? VTSEQ(  2 ) : "" ; }
char const * uga_log_bold       ( void ) { return uga_term_has_color ? VTSEQ(  1 ) : "" ; }
char const * uga_log_underline  ( void ) { return uga_term_has_color ? VTSEQ(  4 ) : "" ; }
char const * uga_log_reset      ( void ) { return uga_term_has_color ? VTSEQ(  0 ) : "" ; }

bool uga_term_check_color ( void )
{
        if( !isatty( STDERR_FILENO ) ) return false ;

        char const * term ;

        if( ( term = getenv( "TERM" ) ) )
        {
                return 0 == strcmp( term, "cygwin"                )
                    || 0 == strcmp( term, "linux"                 )
                    || 0 == strcmp( term, "rxvt-unicode-256color" )
                    || 0 == strcmp( term, "screen"                )
                    || 0 == strcmp( term, "screen-256color"       )
                    || 0 == strcmp( term, "screen.xterm-256color" )
                    || 0 == strcmp( term, "tmux-256color"         )
                    || 0 == strcmp( term, "xterm"                 )
                    || 0 == strcmp( term, "xterm-256color"        )
                    || 0 == strcmp( term, "xterm-termite"         )
                    || 0 == strcmp( term, "xterm-color"           ) ;
        }
        return false ;
}

void uga_log_init ( void )
{
        uga_term_has_color = uga_term_check_color() ;

        _uga_log_mtx = uga_mtx_init( UGA_MTX_PLAIN ) ;
}
