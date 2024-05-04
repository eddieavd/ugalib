//
//
//      uganet
//      uga_err.h
//

#ifndef UGA_ERR_H_
#define UGA_ERR_H_


#include <core/uga_types.h>
#include <core/uga_log.h>


typedef enum
{
        UGA_ERR_NONE          ,
        UGA_ERR_STD           ,
        UGA_ERR_GAI           ,
        UGA_ERR_BAD_ARG       ,
        UGA_ERR_SOCKOPT       ,
        UGA_ERR_BAD_SFAM      ,
        UGA_ERR_BAD_ALLOC     ,
        UGA_ERR_BAD_REALLOC   ,
        UGA_ERR_BAD_ACCESS    ,
        UGA_ERR_PARTIAL_READ  ,
        UGA_ERR_PARTIAL_WRITE ,
        UGA_ERR_CLI           ,
        UGA_ERR_UNKNOWN       ,
} uga_errtype ;

typedef enum
{
        UGA_CAT_NONE    ,
        UGA_CAT_STD     ,
        UGA_CAT_GAI     ,
        UGA_CAT_IO      ,
        UGA_CAT_ALLOC   ,
        UGA_CAT_MEM     ,
        UGA_CAT_CLI     ,
        UGA_CAT_UNKNOWN ,
} uga_err_category ;

typedef struct
{
        uga_errtype     type ;
        uga_err_category cat ;
        u8_t       data[ 8 ] ;
} uga_error ;

typedef struct
{
        uga_errtype     type ;
        uga_err_category cat ;
        i32_t         err_no ;
        i32_t           data ;
} uga_error_std ;

typedef struct
{
        uga_errtype     type ;
        uga_err_category cat ;
        i32_t         err_no ;
        i32_t           data ;
} uga_error_gai ;

typedef struct
{
        uga_errtype      type ;
        uga_err_category  cat ;
        i32_t          wanted ;
        i32_t             got ;
} uga_error_io ;

typedef struct
{
        uga_errtype     type ;
        uga_err_category cat ;
        i64_t         wanted ;
} uga_error_alloc ;

typedef struct
{
        uga_errtype     type ;
        uga_err_category cat ;
        i32_t           size ;
        i32_t            pos ;
} uga_error_mem ;

typedef struct
{
        uga_errtype      type ;
        uga_err_category  cat ;
        i8_t const *     opts ;
} uga_error_cli ;


void _uga_check_std_err   ( i32_t const std_errno ) ;
void _uga_check_std_errno ( void                  ) ;

char const * uga_strerror ( uga_errtype const errtype ) ;

i32_t uga_had_errs ( void ) ;
void  uga_clr_errs ( void ) ;

uga_error       const * uga_get_errdata       ( void ) ;
uga_error_std   const * uga_get_std_errdata   ( void ) ;
uga_error_gai   const * uga_get_gai_errdata   ( void ) ;
uga_error_io    const * uga_get_io_errdata    ( void ) ;
uga_error_alloc const * uga_get_alloc_errdata ( void ) ;
uga_error_mem   const * uga_get_mem_errdata   ( void ) ;
uga_error_cli   const * uga_get_cli_errdata   ( void ) ;

void uga_set_error ( uga_errtype const type, uga_err_category const cat ) ;

void uga_set_std_error   (                         i32_t const err_no, i32_t const data ) ;
void uga_set_gai_error   (                         i32_t const err_no, i32_t const data ) ;
void uga_set_io_error    ( uga_errtype const type, i32_t const wanted, i32_t const  got ) ;
void uga_set_alloc_error ( uga_errtype const type, i64_t const wanted                   ) ;
void uga_set_mem_error   ( uga_errtype const type, i32_t const   size, i32_t const  pos ) ;
void uga_set_cli_error   ( uga_errtype const type                                       ) ;

uga_error   uga_current_error   ( void ) ;
uga_errtype uga_current_errtype ( void ) ;

void uga_print_error   ( void ) ;
void uga_print_errtype ( void ) ;

void uga_log_error   ( uga_log_lvl const level ) ;
void uga_log_errtype ( uga_log_lvl const level ) ;
void uga_log_if_err  ( uga_log_lvl const level ) ;

void uga_print_if_err       ( void ) ;
void uga_abort_if_err       ( void ) ;
void uga_print_abort_if_err ( void ) ;


#endif // UGA_ERR_H_
