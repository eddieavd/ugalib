//
//
//      uganet
//      uga_log.h
//

#ifndef UGA_LOG_H_
#define UGA_LOG_H_

#include <core/uga_types.h>

#define UGA_LOG_LVL_NONE 0
#define UGA_LOG_LVL_ERR  2
#define UGA_LOG_LVL_WARN 3
#define UGA_LOG_LVL_INFO 4
#define UGA_LOG_LVL_DBG  5

#ifndef UGA_LOG_LVL
#define UGA_LOG_LVL UGA_LOG_LVL_DBG
#endif

#include <core/uga_log.def>

#include <stdio.h>


typedef enum
{
        UGA_LVL_SUCC ,
        UGA_LVL_FAIL ,
        UGA_LVL_ERR  ,
        UGA_LVL_WARN ,
        UGA_LVL_INFO ,
        UGA_LVL_DBG  ,
} uga_log_lvl ;

typedef enum
{
        UGA_LOG_SHORT ,
        UGA_LOG_LONG  ,
} uga_log_type ;

_timespec_t uga_time_empty     ( void ) ;
_timespec_t uga_time_current   ( void ) ;
_timespec_t uga_time_current_m ( void ) ;

void uga_log_2 ( uga_log_lvl level, uga_log_type type,                     char const * fmt, ... ) ;
void uga_log_3 ( uga_log_lvl level, uga_log_type type, char const * scope, char const * fmt, ... ) ;

void uga_log_2_v ( uga_log_lvl level, uga_log_type type,                     char const * fmt, va_list args ) ;
void uga_log_3_v ( uga_log_lvl level, uga_log_type type, char const * scope, char const * fmt, va_list args ) ;

void uga_log_sys ( i32_t const level, i32_t const facility, char const * fmt, ... ) ;

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
