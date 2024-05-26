//
//
//      uganet
//      uga_fs.c
//

#include <core/uga_fs.h>
#include <core/uga_log.h>
#include <core/uga_err.h>
#include <core/uga_alloc.h>
#include <core/uga_str.h>
#include <core/uga_strview.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef O_PATH
#define O_PATH __O_PATH
#endif


i64_t uga_fs_filesize ( uga_string_view filepath )
{
        uga_clr_errs() ;

        uga_string fname_cstr = uga_str_create_cstr( filepath ) ;

        i32_t file ;

#if !defined( O_PATH ) || !defined( __O_PATH )
        if( ( file = open( fname_cstr.data, O_RDONLY ) ) == -1 )
#else
        if( ( file = open( fname_cstr.data, O_PATH ) ) == -1 )
#endif
        {
                _uga_check_std_errno() ;
                uga_str_destroy( &fname_cstr ) ;
                return -1 ;
        }
        i64_t filesize = uga_fs_filesize_fd( file ) ;
        uga_str_destroy( &fname_cstr ) ;
        uga_fs_close( file ) ;
        return filesize ;
}

i64_t uga_fs_filesize_fd ( i32_t const filedesc )
{
        uga_clr_errs() ;

        struct stat filestats ;

        if( fstat( filedesc, &filestats ) == -1 )
        {
                _uga_check_std_errno() ;
                return -1 ;
        }
        return filestats.st_size ;
}

i32_t uga_fs_file_access ( uga_string_view filepath, i32_t const mode )
{
        uga_string fname_cstr = uga_str_create_cstr( filepath ) ;

        if( access( fname_cstr.data, mode ) == 0 )
        {
                uga_str_destroy( &fname_cstr ) ;
                return 1 ;
        }
        else
        {
                _uga_check_std_errno() ;
                uga_str_destroy( &fname_cstr ) ;
                return 0 ;
        }
}

i32_t uga_fs_file_exists ( uga_string_view filepath )
{
        return uga_fs_file_access( filepath, F_OK ) ;
}

i32_t uga_fs_file_readable ( uga_string_view filepath )
{
        return uga_fs_file_access( filepath, R_OK ) ;
}

i32_t uga_fs_file_writeable ( uga_string_view filepath )
{
        return uga_fs_file_access( filepath, W_OK ) ;
}

i32_t uga_fs_file_executable ( uga_string_view filepath )
{
        return uga_fs_file_access( filepath, X_OK ) ;
}

i32_t uga_fs_open_file ( uga_string_view filepath, i32_t const flags, i32_t const mode )
{
        uga_clr_errs() ;

        uga_string fname_cstr = uga_str_create_cstr( filepath ) ;

        i32_t file ;

        if( ( file = open( fname_cstr.data, flags, mode ) ) == -1 )
        {
                _uga_check_std_errno() ;
        }
        uga_str_destroy( &fname_cstr ) ;
        return file ;
}

i32_t uga_fs_open_read ( uga_string_view filepath )
{
        return uga_fs_open_file( filepath, O_RDONLY, 0 ) ;
}

i32_t uga_fs_open_write ( uga_string_view filepath )
{
        return uga_fs_open_file( filepath, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR ) ;
}

i32_t uga_fs_open_append ( uga_string_view filepath )
{
        return uga_fs_open_file( filepath, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR ) ;
}

i32_t uga_fs_open_full ( uga_string_view filepath )
{
        return uga_fs_open_file( filepath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR ) ;
}

void uga_fs_close ( i32_t const file )
{
        if( close( file ) == -1 )
        {
                _uga_check_std_errno() ;
        }
}

i64_t uga_fs_read_fd ( i32_t const fd, uga_string * dest )
{
        uga_clr_errs() ;

        i64_t bytes_read = read( fd, dest->data, dest->capacity ) ;

        if( bytes_read == -1 )
        {
                _uga_check_std_errno() ;
        }
        else if( bytes_read < dest->capacity )
        {
//              UGA_DBG_S( "uga::fs::read_fd", "partial read, wanted %db, got %db", dest->capacity, bytes_read ) ;
                uga_set_io_error( UGA_ERR_PARTIAL_READ, dest->capacity, bytes_read ) ;
        }
        dest->size = bytes_read ;
        return bytes_read ;
}

i64_t uga_fs_write_fd ( i32_t const fd, uga_string_view data )
{
        uga_clr_errs() ;

        i64_t bytes_written = write( fd, data.data, data.size ) ;

        if( bytes_written == -1 )
        {
                _uga_check_std_errno() ;
        }
        else if( bytes_written < data.size )
        {
//              UGA_DBG_S( "uga::fs::write_fd", "partial write, wanted %db, written %db", data.size, bytes_written ) ;
                uga_set_io_error( UGA_ERR_PARTIAL_WRITE, data.size, bytes_written ) ;
        }
        return bytes_written ;
}

uga_string uga_fs_read_file ( uga_string_view filename )
{
        i32_t fd = uga_fs_open_read( filename ) ;
        i64_t filesize = uga_fs_filesize_fd( fd ) ;

        uga_string data = uga_str_create_1( filesize ) ;
        UGA_RETURN_ON_ERR( data ) ;

        uga_fs_read_fd( fd, &data ) ;

        return data ;
}

i64_t uga_fs_read_file_into ( uga_string_view filename, uga_string * dest )
{
        i32_t fd = uga_fs_open_read( filename ) ;
        UGA_RETURN_ON_ERR( -1 ) ;

        return uga_fs_read_fd( fd, dest ) ;
}

i64_t uga_fs_write_file ( uga_string_view filename, uga_string_view data )
{
        i32_t fd = uga_fs_open_write( filename ) ;
        UGA_RETURN_ON_ERR( -1 ) ;

        return uga_fs_write_fd( fd, data ) ;
}

i64_t uga_fs_append_file ( uga_string_view filename, uga_string_view data )
{
        i32_t fd = uga_fs_open_append( filename ) ;
        UGA_RETURN_ON_ERR( -1 ) ;

        return uga_fs_write_fd( fd, data ) ;
}
