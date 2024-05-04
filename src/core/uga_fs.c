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

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef O_PATH
#define O_PATH __O_PATH
#endif


i64_t uga_fs_filesize ( char const * filepath )
{
        uga_clr_errs() ;

        i32_t file ;

#if !defined( O_PATH ) || !defined( __O_PATH )
        if( ( file = open( filepath, O_RDONLY ) ) == -1 )
#else
        if( ( file = open( filepath, O_PATH ) ) == -1 )
#endif
        {
                _uga_check_std_errno() ;
                return -1 ;
        }
        struct stat filestats ;

        if( fstat( file, &filestats ) == -1 )
        {
                _uga_check_std_errno() ;
                return -1 ;
        }
        uga_fs_close( file ) ;
        return filestats.st_size ;
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

i32_t uga_fs_file_access ( char const * filepath, i32_t const mode )
{
        if( access( filepath, mode ) == 0 )
        {
                return 1 ;
        }
        else
        {
                _uga_check_std_errno() ;
                return 0 ;
        }
}

i32_t uga_fs_file_exists ( char const * filepath )
{
        return uga_fs_file_access( filepath, F_OK ) ;
}

i32_t uga_fs_file_readable ( char const * filepath )
{
        return uga_fs_file_access( filepath, R_OK ) ;
}

i32_t uga_fs_file_writeable ( char const * filepath )
{
        return uga_fs_file_access( filepath, W_OK ) ;
}

i32_t uga_fs_file_executable ( char const * filepath )
{
        return uga_fs_file_access( filepath, X_OK ) ;
}

i32_t uga_fs_open_file ( char const * filepath, i32_t const flags, i32_t const mode )
{
        uga_clr_errs() ;

        i32_t file ;

        if( ( file = open( filepath, flags, mode ) ) == -1 )
        {
                _uga_check_std_errno() ;
        }
        return file ;
}

i32_t uga_fs_open_read ( char const * filepath )
{
        return uga_fs_open_file( filepath, O_RDONLY, 0 ) ;
}

i32_t uga_fs_open_write ( char const * filepath )
{
        return uga_fs_open_file( filepath, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR ) ;
}

i32_t uga_fs_open_append ( char const * filepath )
{
        return uga_fs_open_file( filepath, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR ) ;
}

i32_t uga_fs_open_full ( char const * filepath )
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

u8_t * uga_fs_read_file ( char const * filepath )
{
        i32_t file = uga_fs_open_read( filepath ) ;

        if( uga_had_errs() ) return NULL ;

        i64_t filesize = uga_fs_filesize_fd( file ) ;

        if( uga_had_errs() )
        {
                uga_fs_close( file ) ;
                return NULL ;
        }
        u8_t * data = ( u8_t * ) uga_allocate( filesize ) ;
        if( uga_had_errs() )
        {
                uga_fs_close( file ) ;
                return NULL ;
        }
        i64_t bytes_read = read( file, data, filesize ) ;

        if( bytes_read == -1 )
        {
                _uga_check_std_errno() ;
                uga_fs_close( file ) ;
                free( data ) ;
                return NULL ;
        }
        else if( bytes_read < filesize )
        {
                UGA_WARN_S( "uga::fs::read_file", "partial read, wanted %db, got %db", filesize, bytes_read ) ;
                uga_set_io_error( UGA_ERR_PARTIAL_READ, filesize, bytes_read ) ;
        }
        uga_fs_close( file ) ;
        return data ;
}

void uga_fs_read_file_into ( char const * filepath, u8_t * dest, i64_t const destlen )
{
        i32_t file = uga_fs_open_read( filepath ) ;

        if( uga_had_errs() ) return ;

        i64_t filesize = uga_fs_filesize_fd( file ) ;

        if( uga_had_errs() ) return ;

        if( filesize > destlen ) filesize = destlen ;

        i64_t bytes_read = read( file, dest, filesize ) ;
        if( bytes_read == -1 )
        {
                _uga_check_std_errno() ;
        }
        else if( bytes_read < filesize )
        {
                UGA_WARN_S( "uga::fs::read_file_into", "partial read, wanted %db, got %db", filesize, bytes_read ) ;
                uga_set_io_error( UGA_ERR_PARTIAL_READ, filesize, bytes_read ) ;
        }
        uga_fs_close( file ) ;
}

void uga_fs_read_file_into_str ( char const * filepath, uga_string * str )
{
        i64_t filesize = uga_fs_filesize( filepath ) ;
        uga_str_reserve( str, filesize ) ;
        if( uga_had_errs() ) return ;

        uga_fs_read_file_into( filepath, ( u8_t * ) str->data, str->capacity ) ;
        if( uga_had_errs() ) return ;

        str->size = filesize ;
}

uga_string uga_fs_read_file_str ( char const * filepath )
{
        i64_t filesize = uga_fs_filesize( filepath ) ;
        uga_string contents = uga_str_create_1( filesize ) ;
        if( uga_had_errs() ) return contents ;

        uga_fs_read_file_into( filepath, ( u8_t * ) contents.data, contents.capacity ) ;
        if( uga_had_errs() ) return contents ;

        contents.size = filesize ;

        return contents ;
}

void uga_fs_write_file ( char const * filepath, u8_t const * data, i64_t const datalen )
{
        i32_t file = uga_fs_open_full( filepath ) ;

        if( uga_had_errs() ) return ;

        i64_t bytes_written = write( file, data, datalen ) ;

        if( bytes_written == -1 )
        {
                _uga_check_std_errno() ;
        }
        else if( bytes_written < datalen )
        {
                UGA_WARN_S( "uga::fs::write_file", "partial write, wanted %db, got %db", datalen, bytes_written ) ;
                uga_set_io_error( UGA_ERR_PARTIAL_WRITE, datalen, bytes_written ) ;
        }
        uga_fs_close( file ) ;
}

void uga_fs_append_file ( char const * filepath, u8_t const * data, i64_t const datalen )
{
        i32_t file = uga_fs_open_append( filepath ) ;

        if( uga_had_errs() ) return ;

        i64_t bytes_written = write( file, data, datalen ) ;

        if( bytes_written == -1 )
        {
                _uga_check_std_errno() ;
        }
        else if( bytes_written < datalen )
        {
                UGA_WARN_S( "uga::fs::write_file", "partial write, wanted %db, got %db", datalen, bytes_written ) ;
                uga_set_io_error( UGA_ERR_PARTIAL_WRITE, datalen, bytes_written ) ;
        }
        uga_fs_close( file ) ;
}

void uga_fs_write_file_str ( char const * filepath, uga_string const * str )
{
        uga_fs_write_file( filepath, ( u8_t const * ) str->data, str->size ) ;
}
