//
//
//      uganet
//      uga_log.c
//

#include <core/uga_log.h>
#include <core/uga_types.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

#define VTSEQ(ID) ("\x1b[" #ID "m")


void uga_log_2 ( uga_log_lvl level, char const * fmt, ... )
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
                        break ;
        }
        fprintf( stderr, "%s", uga_log_reset() ) ;

        va_list args ;
        va_start( args, fmt ) ;
        vfprintf( stderr, fmt, args ) ;
        va_end( args ) ;
        fprintf( stderr, "\n" ) ;
}

void uga_log_3 ( uga_log_lvl level, char const * scope, char const * fmt, ... )
{
        switch( level )
        {
                case UGA_LVL_DBG:
                        fprintf( stderr, "%s%sUGA::DBG  : %s : ", uga_log_purple(), uga_log_bold(), scope ) ;
                        break ;
                case UGA_LVL_INFO:
                        fprintf( stderr, "%sUGA::INFO : %s : ", uga_log_bold(), scope ) ;
                        break ;
                case UGA_LVL_WARN:
                        fprintf( stderr, "%s%sUGA::WARN : %s : ", uga_log_yellow(), uga_log_bold(), scope ) ;
                        break ;
                case UGA_LVL_ERR:
                        fprintf( stderr, "%s%sUGA::ERR  : %s : ", uga_log_red(), uga_log_bold(), scope ) ;
                        break ;
                case UGA_LVL_SUCC:
                        fprintf( stderr, "%s%sUGA::SUCC : %s : ", uga_log_green(), uga_log_bold(), scope ) ;
                        break ;
                case UGA_LVL_FAIL:
                        fprintf( stderr, "%s%sUGA::FAIL : %s : ", uga_log_red(), uga_log_bold(), scope ) ;
                        break ;
                default:
                        break ;
        }
        fprintf( stderr, "%s", uga_log_reset() ) ;

        va_list args ;
        va_start( args, fmt ) ;
        vfprintf( stderr, fmt, args ) ;
        va_end( args ) ;
        fprintf( stderr, "\n" ) ;
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
}
