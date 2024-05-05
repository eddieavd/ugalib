//
//
//      uganet
//      uga_fs.h
//

#ifndef UGA_FS_H_
#define UGA_FS_H_


#include <core/uga_types.h>


struct uga_string_t ;
typedef struct uga_string_t uga_string ;

struct uga_string_view_t ;
typedef struct uga_string_view_t uga_string_view ;

i64_t uga_fs_filesize    ( uga_string_view filepath ) ;
i64_t uga_fs_filesize_fd ( i32_t const     filedesc ) ;

i32_t uga_fs_file_access ( uga_string_view filepath, i32_t const mode ) ;

i32_t uga_fs_file_exists     ( uga_string_view filepath ) ;
i32_t uga_fs_file_readable   ( uga_string_view filepath ) ;
i32_t uga_fs_file_writeable  ( uga_string_view filepath ) ;
i32_t uga_fs_file_executable ( uga_string_view filepath ) ;

i32_t uga_fs_open_file   ( uga_string_view filepath, i32_t const flags, i32_t const mode ) ;
i32_t uga_fs_open_read   ( uga_string_view filepath                                      ) ;
i32_t uga_fs_open_write  ( uga_string_view filepath                                      ) ;
i32_t uga_fs_open_append ( uga_string_view filepath                                      ) ;
i32_t uga_fs_open_full   ( uga_string_view filepath                                      ) ;

void uga_fs_close ( i32_t const file ) ;

i64_t uga_fs_read_fd   ( i32_t const fd, uga_string    * dest ) ;
i64_t uga_fs_write_fd  ( i32_t const fd, uga_string_view data ) ;

uga_string uga_fs_read_file      ( uga_string_view filename                       ) ;
i64_t      uga_fs_read_file_into ( uga_string_view filename, uga_string    * dest ) ;
i64_t      uga_fs_write_file     ( uga_string_view filename, uga_string_view data ) ;
i64_t      uga_fs_append_file    ( uga_string_view filename, uga_string_view data ) ;


#endif // UGA_FS_H_
