//
//
//      uganet
//      uga_types.h
//

#ifndef UGA_TYPES_H_
#define UGA_TYPES_H_


#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define UGA_IPv4_STRLEN  INET_ADDRSTRLEN
#define UGA_IPv6_STRLEN INET6_ADDRSTRLEN


typedef  int8_t  i8_t ;
typedef int16_t i16_t ;
typedef int32_t i32_t ;
typedef int64_t i64_t ;

typedef  uint8_t  u8_t ;
typedef uint16_t u16_t ;
typedef uint32_t u32_t ;
typedef uint64_t u64_t ;

static  u8_t const  U8_T_MAX = -1 ;
static u16_t const U16_T_MAX = -1 ;
static u32_t const U32_T_MAX = -1 ;
static u64_t const U64_T_MAX = -1 ;

typedef i32_t uga_sockfd_t ;

typedef struct sockaddr          _sockaddr_t ;
typedef struct sockaddr_in    _sockaddr_in_t ;
typedef struct sockaddr_in6  _sockaddr_in6_t ;
typedef        socklen_t          _socklen_t ;
typedef struct     addrinfo      _addrinfo_t ;
typedef struct  in_addr           _in_addr_t ;
typedef struct in6_addr          _in6_addr_t ;

typedef struct sockaddr_storage _sockaddr_storage_t ;


#endif // UGA_TYPES_H_
