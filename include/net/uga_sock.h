//
//
//      uganet
//      uga_sock.h
//

#ifndef UGA_SOCK_H_
#define UGA_SOCK_H_

#include <core/uga_types.h>

#include <netdb.h>


////////////////////////////////////////////////////////////////////////////////
/// types
////////////////////////////////////////////////////////////////////////////////

typedef enum
{
        UGA_PASSIVE   = AI_PASSIVE     ,
        UGA_CANONNAME = AI_CANONNAME   ,
        UGA_NUMHOST   = AI_NUMERICHOST ,
        UGA_NUMSERV   = AI_NUMERICSERV ,
        UGA_V4MAPPED  = AI_V4MAPPED    ,
        UGA_ADDRCONF  = AI_ADDRCONFIG  ,
        UGA_ALL       = AI_ALL         ,
} uga_sock_flags ;

typedef enum
{
        UGA_SFAM_IPv4   = AF_INET   ,
        UGA_SFAM_IPv6   = AF_INET6  ,
        UGA_SFAM_UNSPEC = AF_UNSPEC ,
} uga_sock_fam ;

typedef enum
{
        UGA_SOCK_STREAM = SOCK_STREAM ,
        UGA_SOCK_DGRAM  = SOCK_DGRAM  ,
        UGA_SOCK_RAW    = SOCK_RAW    ,
} uga_sock_type ;

typedef enum
{
        UGA_PROT_COUNT ,
} uga_protocol ;

typedef struct
{
        u16_t family ;
        u8_t data[ 14 ] ;
} uga_sockaddr ;

typedef struct
{
        u16_t    family ;
        u16_t      port ;
        _in_addr_t addr ;
        u8_t  zero[ 8 ] ;
} uga_sockaddr_ipv4 ;

typedef struct
{
        u16_t     family ;
        u16_t       port ;
        u32_t   flowinfo ;
        _in6_addr_t addr ;
        u32_t   scope_id ;
} uga_sockaddr_ipv6 ;

typedef struct
{
        u16_t    family ;
        u16_t      port ;
        u32_t data[ 6 ] ;
} uga_sockaddr_storage ;

struct uga_addrinfo_t
{
        uga_sock_flags      flags ;
        uga_sock_fam       family ;
        uga_sock_type        type ;
        uga_protocol         prot ;
        _socklen_t        addrlen ;
        uga_sockaddr_storage addr ;
        char           canon_name [ 256 ] ;
        _addrinfo_t *        next ;
};
typedef struct uga_addrinfo_t uga_addrinfo ;

typedef struct
{
        uga_sock_fam  family ;
        uga_sock_type   type ;
        uga_sock_flags flags ;
} uga_sock_conf ;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

static uga_sock_conf uga_default_sock_config = { UGA_SFAM_UNSPEC, UGA_SOCK_STREAM, 0 } ;

#pragma GCC diagnostic pop

typedef struct
{
        i32_t      sockfd ;
        uga_addrinfo addr ;
} uga_socket ;

////////////////////////////////////////////////////////////////////////////////
/// socket config
////////////////////////////////////////////////////////////////////////////////

uga_sock_conf uga_sock_get_config (                     void ) ;
void          uga_sock_set_config ( uga_sock_conf const conf ) ;

void uga_sock_reuse ( uga_socket const sock ) ;

void uga_sock_set_ttl ( uga_socket * sock, char const * ttl ) ;

////////////////////////////////////////////////////////////////////////////////
/// ip <-> str
////////////////////////////////////////////////////////////////////////////////

uga_sockaddr_ipv4 uga_ipv4_from_str ( char const * ip ) ;
uga_sockaddr_ipv6 uga_ipv6_from_str ( char const * ip ) ;

void uga_ipv4_to_str ( uga_sockaddr_ipv4 const * ip, char * dest ) ;
void uga_ipv6_to_str ( uga_sockaddr_ipv6 const * ip, char * dest ) ;

void uga_ip_to_str ( uga_addrinfo const * addr, char * dest ) ;

////////////////////////////////////////////////////////////////////////////////
/// addrinfo
////////////////////////////////////////////////////////////////////////////////

uga_addrinfo uga_addrinfo_local_configured ( char const * service, uga_sock_conf const * conf ) ;

uga_addrinfo  uga_addrinfo_get_configured ( char const * node, char const * service, uga_sock_conf const * conf ) ;
_addrinfo_t * uga_addrinfo_raw_configured ( char const * node, char const * service, uga_sock_conf const * conf ) ;

uga_addrinfo uga_addrinfo_local ( char const * service ) ;

uga_addrinfo  uga_addrinfo_get ( char const * node, char const * service ) ;
_addrinfo_t * uga_addrinfo_raw ( char const * node, char const * service ) ;

uga_addrinfo uga_addrinfo_from_sockaddr ( uga_sockaddr_storage const sockaddr ) ;

void uga_get_ip_str ( char const * node, char const * service, char * dest ) ;

void uga_lookup_configured         ( char const * host, char const * service, char *   ip, u16_t *    port, uga_sock_conf const * conf ) ;
void uga_reverse_lookup_configured ( char const *   ip, char const *    port, char * host, char  * service, uga_sock_conf const * conf ) ;

void uga_lookup         ( char const * host, char const * service, char *   ip, u16_t *    port ) ;
void uga_reverse_lookup ( char const *   ip, char const *    port, char * host, char  * service ) ;

////////////////////////////////////////////////////////////////////////////////
/// socket
////////////////////////////////////////////////////////////////////////////////

uga_socket uga_sock_create_configured             ( char const * host, char const * service, uga_sock_conf const * conf ) ;
uga_socket uga_sock_create_and_bind_configured    ( char const * host, char const * service, uga_sock_conf const * conf ) ;
uga_socket uga_sock_create_and_connect_configured ( char const * host, char const * service, uga_sock_conf const * conf ) ;

uga_socket uga_sock_create_and_listen_configured ( char const * service, i32_t const backlog, uga_sock_conf const * conf ) ;

uga_socket uga_sock_create             ( char const * host, char const * service ) ;
uga_socket uga_sock_create_and_bind    ( char const * host, char const * service ) ;
uga_socket uga_sock_create_and_connect ( char const * host, char const * service ) ;

uga_socket uga_sock_create_and_listen ( char const * service, i32_t const backlog ) ;

void uga_sock_bind    ( uga_socket const sock ) ;
void uga_sock_connect ( uga_socket const sock ) ;

void uga_sock_listen ( uga_socket const sock, i32_t const backlog ) ;

uga_socket uga_sock_accept ( uga_socket const listenfd ) ;

void uga_sock_close    ( uga_socket const * sock                   ) ;
void uga_sock_shutdown ( uga_socket const * sock, i32_t const mode ) ;

////////////////////////////////////////////////////////////////////////////////
/// byte order conversions
////////////////////////////////////////////////////////////////////////////////

u16_t uga_htons  ( u16_t const bytes ) ;
u16_t uga_ntohs  ( u16_t const bytes ) ;
u32_t uga_htonl  ( u32_t const bytes ) ;
u32_t uga_ntohl  ( u32_t const bytes ) ;
u64_t uga_htonll ( u64_t const bytes ) ;
u64_t uga_ntohll ( u64_t const bytes ) ;

////////////////////////////////////////////////////////////////////////////////
/// utils
////////////////////////////////////////////////////////////////////////////////

void uga_get_peer ( uga_socket const * sock, uga_sockaddr_storage * addr, u32_t * addrlen ) ;

void uga_get_nameinfo ( uga_sockaddr_storage const * addr, _socklen_t const addrlen,
                        char * host, _socklen_t hostlen,
                        char * serv, _socklen_t servlen,
                        i32_t const flags ) ;

void uga_get_hostname ( char * hostname, u64_t const hostlen ) ;

void uga_sock_log   (                     uga_socket const sock ) ;
void uga_sock_log_s ( char const * scope, uga_socket const sock ) ;

////////////////////////////////////////////////////////////////////////////////
/// conversions
////////////////////////////////////////////////////////////////////////////////

uga_addrinfo _uga_addr_from_posix ( _addrinfo_t const * addr ) ;


#endif // UGA_SOCK_H_
