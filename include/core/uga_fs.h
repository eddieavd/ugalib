//
//
//      uganet
//      uga_fs.h
//

#ifndef UGA_FS_H_
#define UGA_FS_H_


#include <core/uga_types.h>


struct uga_string_t ;

i64_t uga_fs_filesize    ( char  const * filepath ) ;
i64_t uga_fs_filesize_fd ( i32_t const   filedesc ) ;

i32_t uga_fs_file_access ( char const * filepath, i32_t const mode ) ;

i32_t uga_fs_file_exists     ( char const * filepath ) ;
i32_t uga_fs_file_readable   ( char const * filepath ) ;
i32_t uga_fs_file_writeable  ( char const * filepath ) ;
i32_t uga_fs_file_executable ( char const * filepath ) ;

i32_t uga_fs_open_file   ( char const * filepath, i32_t const flags, i32_t const mode ) ;
i32_t uga_fs_open_read   ( char const * filepath                                      ) ;
i32_t uga_fs_open_write  ( char const * filepath                                      ) ;
i32_t uga_fs_open_append ( char const * filepath                                      ) ;
i32_t uga_fs_open_full   ( char const * filepath                                      ) ;

void uga_fs_close ( i32_t const file ) ;

u8_t * uga_fs_read_file      ( char const * filepath                                         ) ;
void   uga_fs_read_file_into ( char const * filepath, u8_t       * dest, i64_t const destlen ) ;
void   uga_fs_write_file     ( char const * filepath, u8_t const * data, i64_t const datalen ) ;
void   uga_fs_append_file    ( char const * filepath, u8_t const * data, i64_t const datalen ) ;

void uga_fs_write_file_str     ( char const * filepath, struct uga_string_t const * str ) ;
void uga_fs_read_file_into_str ( char const * filepath, struct uga_string_t       * str ) ;

struct uga_string_t uga_fs_read_file_str ( char const * filepath ) ;


#endif // UGA_FS_H_
