//
//
//      uganet
//      uga_log.h
//

#ifndef UGA_NO_LOGS
#include <core/uga_log_enable.def>
#else
#include <core/uga_log_disable.def>
#endif

#ifndef UGA_LOG_H_
#define UGA_LOG_H_


typedef enum
{
        UGA_LVL_DBG  ,
        UGA_LVL_INFO ,
        UGA_LVL_WARN ,
        UGA_LVL_ERR  ,
        UGA_LVL_SUCC ,
        UGA_LVL_FAIL ,
} uga_log_lvl ;

void uga_log_2 ( uga_log_lvl level,                     char const * fmt, ... ) ;
void uga_log_3 ( uga_log_lvl level, char const * scope, char const * fmt, ... ) ;

void uga_log_init ( void ) ;

char const * uga_log_black      ( void ) ;
char const * uga_log_red        ( void ) ;
char const * uga_log_green      ( void ) ;
char const * uga_log_yellow     ( void ) ;
char const * uga_log_blue       ( void ) ;
char const * uga_log_purple     ( void ) ;
char const * uga_log_cyan       ( void ) ;
char const * uga_log_light_gray ( void ) ;
char const * uga_log_white      ( void ) ;
char const * uga_log_light_red  ( void ) ;

char const * uga_log_dim        ( void ) ;
char const * uga_log_bold       ( void ) ;
char const * uga_log_underline  ( void ) ;
char const * uga_log_reset      ( void ) ;


#endif // UGA_LOG_H_
