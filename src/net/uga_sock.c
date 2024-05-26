//
//
//      uganet
//      uga_sock.c
//

#include <net/uga_sock.h>

#include <core/uga_util.h>
#include <core/uga_log.h>
#include <core/uga_err.h>
#include <core/uga_fs.h>

#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>


////////////////////////////////////////////////////////////////////////////////
/// socket config
////////////////////////////////////////////////////////////////////////////////

uga_sock_conf uga_sock_get_config ( void )
{
        return uga_default_sock_config ;
}

void uga_sock_set_config ( uga_sock_conf const conf )
{
        uga_default_sock_config = conf ;
}

void uga_sock_reuse ( uga_socket const sock )
{
        uga_clr_errs() ;

        i32_t yes = 1 ;

        if( setsockopt( sock.sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( yes ) ) == -1 )
        {
                _uga_check_std_errno() ;
        }
}

void uga_sock_set_ttl ( uga_socket * sock, char const * ttl )
{
        i32_t ttlval = atoi( ttl ) ;

        if( setsockopt( sock->sockfd, IPPROTO_IP, IP_TTL, &ttlval, sizeof( ttlval ) ) == -1 )
        {
                _uga_check_std_errno() ;
        }
}

////////////////////////////////////////////////////////////////////////////////
/// ip <-> str
////////////////////////////////////////////////////////////////////////////////

uga_sockaddr_ipv4 uga_ipv4_from_str ( uga_string_view ip )
{
        uga_clr_errs() ;

        _sockaddr_in_t          sa ;
        uga_sockaddr_ipv4 sockaddr ;

        if( inet_pton( UGA_SFAM_IPv4, ip.data, &( sa.sin_addr ) ) == 0 )
        {
                UGA_ERR_S( "uga::sock::ipv4_from_str", "bad ip" ) ;
                uga_set_error( UGA_ERR_BAD_ARG, UGA_CAT_NONE ) ;
        }
        _uga_check_std_errno() ;

        memset( &sockaddr, 0, sizeof( sockaddr ) ) ;
        sockaddr.addr = sa.sin_addr ;

        return sockaddr ;
}

uga_sockaddr_ipv6 uga_ipv6_from_str ( uga_string_view ip )
{
        uga_clr_errs() ;

        _sockaddr_in6_t         sa ;
        uga_sockaddr_ipv6 sockaddr ;

        if( inet_pton( UGA_SFAM_IPv6, ip.data, &( sa.sin6_addr ) ) == 0 )
        {
                UGA_ERR_S( "uga::sock::ipv6_from_str", "bad ip" ) ;
                uga_set_error( UGA_ERR_BAD_ARG, UGA_CAT_NONE ) ;
        }
        _uga_check_std_errno() ;

        memset( &sockaddr, 0, sizeof( sockaddr ) ) ;
        sockaddr.addr = sa.sin6_addr ;

        return sockaddr ;
}

uga_string uga_ipv4_to_str ( uga_sockaddr_ipv4 const * ip )
{
        uga_clr_errs() ;

        uga_string ip_str = uga_str_create_1( UGA_IPv4_STRLEN + 1 ) ;

        if( !inet_ntop( UGA_SFAM_IPv4, &( ip->addr ), ip_str.data, UGA_IPv4_STRLEN ) )
        {
                _uga_check_std_errno() ;
        }
        uga_str_check_size( &ip_str ) ;
        return ip_str ;
}

uga_string uga_ipv6_to_str ( uga_sockaddr_ipv6 const * ip )
{
        uga_clr_errs() ;

        uga_string ip_str = uga_str_create_1( UGA_IPv6_STRLEN + 1 ) ;

        if( !inet_ntop( UGA_SFAM_IPv6, &( ip->addr ), ip_str.data, UGA_IPv6_STRLEN ) )
        {
                _uga_check_std_errno() ;
        }
        uga_str_check_size( &ip_str ) ;
        return ip_str ;
}

uga_string uga_ip_to_str ( uga_addrinfo const * addr )
{
        if( addr->family == UGA_SFAM_IPv4 )
        {
                return uga_ipv4_to_str( ( uga_sockaddr_ipv4 const * ) &addr->addr ) ;
        }
        else if( addr->family == UGA_SFAM_IPv6 )
        {
                return uga_ipv6_to_str( ( uga_sockaddr_ipv6 const * ) &addr->addr ) ;
        }
        else
        {
                return uga_str_create_0() ;
        }
}

////////////////////////////////////////////////////////////////////////////////
/// addrinfo
////////////////////////////////////////////////////////////////////////////////

uga_addrinfo uga_addrinfo_local_configured ( uga_string_view service, uga_sock_conf const * conf )
{
        return uga_addrinfo_get_configured( uga_sv_create_0(), service, conf ) ;
}

uga_addrinfo uga_addrinfo_get_configured ( uga_string_view node, uga_string_view service, uga_sock_conf const * conf )
{
        uga_clr_errs() ;

        i32_t           status ;
        _addrinfo_t      hints ;
        _addrinfo_t * servinfo ;

        memset( &hints, 0, sizeof( hints ) ) ;
        hints.ai_family   = conf->family ;
        hints.ai_socktype = conf->type   ;
        hints.ai_flags    = node.data == NULL
                          ?   conf->flags | UGA_PASSIVE
                          : ( conf->flags | UGA_PASSIVE ) ^ UGA_PASSIVE ;
        uga_addrinfo addr = { 0 } ;
        if( ( status = getaddrinfo( node.data, service.data, &hints, &servinfo ) ) != 0 )
        {
                UGA_WARN_S( "uga::sock::addrinfo_get_configured", "possible gai error: %s", gai_strerror( status ) ) ;
                uga_set_gai_error( status, 0 ) ;
                return addr ;
        }
        addr = _uga_addr_from_posix( &servinfo[ 0 ] ) ;

        freeaddrinfo( servinfo ) ;

        return addr ;
}

_addrinfo_t * uga_addrinfo_raw_configured ( uga_string_view node, uga_string_view service, uga_sock_conf const * conf )
{
        uga_clr_errs() ;

        i32_t           status ;
        _addrinfo_t      hints ;
        _addrinfo_t * servinfo ;

        memset( &hints, 0, sizeof( hints ) ) ;
        hints.ai_family   = conf->family ;
        hints.ai_socktype = conf->type   ;
        hints.ai_flags    = node.data == NULL
                          ?   conf->flags | UGA_PASSIVE
                          : ( conf->flags | UGA_PASSIVE ) ^ UGA_PASSIVE ;
        if( ( status = getaddrinfo( node.data, service.data, &hints, &servinfo ) ) != 0 )
        {
                UGA_WARN_S( "uga::sock::addrinfo_get_configured", "possible gai error: %s", gai_strerror( status ) ) ;
                uga_set_gai_error( status, 0 ) ;
                return NULL ;
        }
        return servinfo ;
}

uga_addrinfo uga_addrinfo_local ( uga_string_view service )
{
        return uga_addrinfo_local_configured( service, &uga_default_sock_config ) ;
}

uga_addrinfo uga_addrinfo_get ( uga_string_view node, uga_string_view service )
{
        return uga_addrinfo_get_configured( node, service, &uga_default_sock_config ) ;
}

_addrinfo_t * uga_addrinfo_raw ( uga_string_view node, uga_string_view service )
{
        return uga_addrinfo_raw_configured( node, service, &uga_default_sock_config ) ;
}

uga_addrinfo uga_addrinfo_from_sockaddr ( uga_sockaddr_storage const sockaddr )
{
        uga_clr_errs() ;
        uga_addrinfo addr = { 0 } ;

        if( sockaddr.family == UGA_SFAM_IPv4 )
        {
                uga_sockaddr_ipv4 * ipv4_addr = ( uga_sockaddr_ipv4 * ) &sockaddr;

                addr.family = ipv4_addr->family ;
                addr.addrlen = sizeof( *ipv4_addr ) ;
                memcpy( &addr.addr, ipv4_addr, sizeof( *ipv4_addr ) ) ;
        }
        else if( sockaddr.family == UGA_SFAM_IPv6 )
        {
                uga_sockaddr_ipv6 * ipv6_addr = ( uga_sockaddr_ipv6 * ) &sockaddr;

                addr.family = ipv6_addr->family ;
                addr.addrlen = sizeof( *ipv6_addr ) ;
                memcpy( &addr.addr, ipv6_addr, sizeof( &ipv6_addr ) ) ;
        }
        else
        {
                uga_set_error( UGA_ERR_BAD_SFAM, UGA_CAT_NONE ) ;
        }
        return addr ;
}

uga_string uga_get_ip_str ( uga_string_view node, uga_string_view service )
{
        uga_addrinfo addr = uga_addrinfo_get( node, service ) ;

        uga_string ip_str = uga_str_create_0() ;

        if( uga_had_errs() )
        {}
        else if( addr.family == UGA_SFAM_IPv4 )
        {
                ip_str = uga_ipv4_to_str( ( uga_sockaddr_ipv4 * ) &addr.addr ) ;
        }
        else if( addr.family == UGA_SFAM_IPv6 )
        {
                ip_str = uga_ipv6_to_str( ( uga_sockaddr_ipv6 * ) &addr.addr ) ;
        }
        else
        {}
        return ip_str ;
}

void uga_lookup_configured ( uga_string_view host, uga_string_view service, char * ip, u16_t * port, uga_sock_conf const * conf )
{
        uga_addrinfo info = uga_addrinfo_get_configured( host, service, conf ) ;
        if( uga_had_errs() ) return ;

        uga_string ip_str = uga_ip_to_str( &info ) ;
        memcpy( ip, ip_str.data, ip_str.size ) ;
        uga_str_destroy( &ip_str ) ;

        *port = uga_ntohs( info.addr.port ) ;
}

void uga_reverse_lookup_configured ( uga_string_view ip, uga_string_view port, char * host, char * service, uga_sock_conf const * conf )
{
        uga_addrinfo info = uga_addrinfo_get_configured( ip, port, conf ) ;
        if( uga_had_errs() ) return ;

        uga_get_nameinfo( &info.addr, sizeof( info.addr ), host, 256, service, 256, 0 ) ; // TODO: remove hardcoded hostlen and servlen
}

void uga_lookup ( uga_string_view host, uga_string_view service, char * ip, u16_t * port )
{
        uga_lookup_configured( host, service, ip, port, &uga_default_sock_config ) ;
}

void uga_reverse_lookup ( uga_string_view ip, uga_string_view port, char * host, char * service )
{
        uga_reverse_lookup_configured( ip, port, host, service, &uga_default_sock_config ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// socket
////////////////////////////////////////////////////////////////////////////////

uga_socket uga_sock_create_configured ( uga_string_view host, uga_string_view service, uga_sock_conf const * conf )
{
        uga_socket   sock = { false, -1, { 0 } } ;
        uga_addrinfo addr = uga_addrinfo_get_configured( host, service, conf ) ;

        if( uga_had_errs() ) return sock ;

        uga_sockfd_t sockfd ;

        if( ( sockfd = socket( addr.family, addr.type, addr.prot ) ) == -1 )
        {
                _uga_check_std_errno() ;
                return sock ;
        }
        sock.sockfd = sockfd ;
        sock.addr   =   addr ;

        return sock ;
}

uga_socket uga_sock_create_and_bind_configured ( uga_string_view host, uga_string_view service, uga_sock_conf const * conf )
{
        uga_socket sock = uga_sock_create_configured( host, service, conf ) ;

        if( uga_had_errs() ) return sock ;

        uga_sock_bind( &sock ) ;
        return sock ;
}

uga_socket uga_sock_create_and_connect_configured ( uga_string_view host, uga_string_view service, uga_sock_conf const * conf )
{
        uga_socket sock = uga_sock_create_configured( host, service, conf ) ;

        if( uga_had_errs() ) return sock ;

        uga_sock_connect( &sock ) ;
        return sock ;
}

uga_socket uga_sock_create_and_listen_configured ( uga_string_view service, i32_t const backlog, uga_sock_conf const * conf )
{
        uga_socket sock = uga_sock_create_and_bind_configured( uga_sv_create_0(), service, conf ) ;

        if( uga_had_errs() ) return sock ;

        uga_sock_listen( &sock, backlog ) ;
        return sock ;
}

uga_socket uga_sock_create ( uga_string_view host, uga_string_view service )
{
        return uga_sock_create_configured( host, service, &uga_default_sock_config ) ;
}

uga_socket uga_sock_create_and_bind ( uga_string_view host, uga_string_view service )
{
        return uga_sock_create_and_bind_configured( host, service, &uga_default_sock_config ) ;
}

uga_socket uga_sock_create_and_connect ( uga_string_view host, uga_string_view service )
{
        return uga_sock_create_and_connect_configured( host, service, &uga_default_sock_config ) ;
}

uga_socket uga_sock_create_and_listen ( uga_string_view service, i32_t const backlog )
{
        return uga_sock_create_and_listen_configured( service, backlog, &uga_default_sock_config ) ;
}

void uga_sock_bind ( uga_socket * sock )
{
        uga_clr_errs() ;
        if( bind( sock->sockfd, ( _sockaddr_t * ) &sock->addr.addr, sock->addr.addrlen ) == -1 )
        {
                _uga_check_std_errno() ;
        }
}

void uga_sock_connect ( uga_socket * sock )
{
        uga_clr_errs() ;
        if( connect( sock->sockfd, ( _sockaddr_t * ) &sock->addr.addr, sock->addr.addrlen ) == -1 )
        {
                _uga_check_std_errno() ;
        }
        else sock->is_connected = true ;
}

void uga_sock_listen ( uga_socket * sock, i32_t const backlog )
{
        uga_clr_errs() ;
        if( listen( sock->sockfd, backlog ) == -1 )
        {
                _uga_check_std_errno() ;
        }
}

uga_socket uga_sock_accept ( uga_socket const listen_sock )
{
        uga_clr_errs() ;

        uga_sockaddr_storage remote_addr ;
        _socklen_t remote_addrlen = sizeof( remote_addr ) ;

        uga_socket remote_sock = { false, -1, { 0 } } ;

        remote_sock.sockfd = accept( listen_sock.sockfd, ( _sockaddr_t * ) &remote_addr, &remote_addrlen ) ;
        if( remote_sock.sockfd == -1 )
        {
                _uga_check_std_errno() ;
                return remote_sock ;
        }
        remote_sock.is_connected = true ;

        remote_sock.addr = uga_addrinfo_from_sockaddr( remote_addr ) ;

        return remote_sock ;
}

void uga_sock_close ( uga_socket * sock )
{
        uga_fs_close( sock->sockfd ) ;
}

void uga_sock_close_void ( void * sock )
{
        uga_sock_close( sock ) ;
}

void uga_sock_shutdown ( uga_socket * sock, i32_t const mode )
{
        if( shutdown( sock->sockfd, mode ) == -1 )
        {
                _uga_check_std_errno() ;
        }
}

////////////////////////////////////////////////////////////////////////////////
/// byte order conversions
////////////////////////////////////////////////////////////////////////////////

u16_t uga_htons ( u16_t const bytes ) { return htons( bytes ) ; }
u16_t uga_ntohs ( u16_t const bytes ) { return ntohs( bytes ) ; }
u32_t uga_htonl ( u32_t const bytes ) { return htonl( bytes ) ; }
u32_t uga_ntohl ( u32_t const bytes ) { return ntohl( bytes ) ; }

u64_t uga_htonll ( u64_t const bytes )
{
        if( UGA_ENDIAN_NATIVE == UGA_ENDIAN_LITTLE )
        {
                u32_t top    = bytes >> 32       ;
                u32_t bottom = bytes & U32_T_MAX ;

                top    = uga_htonl(    top ) ;
                bottom = uga_htonl( bottom ) ;

                u64_t val = ( u64_t ) bottom << 32 ;
                val |= top ;

                return val ;
        }
        else
        {
                return bytes ;
        }
}
u64_t uga_ntohll ( u64_t const bytes )
{
        if( UGA_ENDIAN_NATIVE == UGA_ENDIAN_LITTLE )
        {
                u32_t top    = bytes >> 32       ;
                u32_t bottom = bytes & U32_T_MAX ;

                top    = uga_htonl(    top ) ;
                bottom = uga_htonl( bottom ) ;

                u64_t val = ( u64_t ) bottom << 32 ;
                val |= top ;

                return val ;
        }
        else
        {
                return bytes ;
        }
}

////////////////////////////////////////////////////////////////////////////////
/// utils
////////////////////////////////////////////////////////////////////////////////

void uga_get_peer ( uga_socket const * sock, uga_sockaddr_storage * addr, _socklen_t * addrlen )
{
        if( getpeername( sock->sockfd, ( _sockaddr_t * ) addr, addrlen ) == -1 )
        {
                _uga_check_std_errno() ;
        }
}

void uga_get_nameinfo ( uga_sockaddr_storage const * addr, _socklen_t const addrlen,
                        char * host, _socklen_t hostlen,
                        char * serv, _socklen_t servlen,
                        i32_t const flags )
{
        if( getnameinfo( ( _sockaddr_t * ) addr, addrlen, host, hostlen, serv, servlen, flags ) != 0 )
        {
                _uga_check_std_errno() ;
        }
}

void uga_get_hostname ( char * hostname, u64_t const hostlen )
{
        if( gethostname( hostname, hostlen ) == -1 )
        {
                _uga_check_std_errno() ;
        }
}

void uga_sock_log ( uga_socket const sock )
{
        UGA_INFO( "socket info:" ) ;
        UGA_INFO( "\tsockfd: %d", sock.sockfd ) ;
        switch( sock.addr.family )
        {
                case UGA_SFAM_IPv4:
                        UGA_INFO( "\tfamily: IPv4" ) ;
                        break ;
                case UGA_SFAM_IPv6:
                        UGA_INFO( "\tfamily: IPv6" ) ;
                        break ;
                default:
                        UGA_INFO( "\tfamily: unknown" ) ;
                        break ;
        }
        switch( sock.addr.type )
        {
                case UGA_SOCK_STREAM:
                        UGA_INFO( "\ttype: tcp" ) ;
                        break ;
                case UGA_SOCK_DGRAM:
                        UGA_INFO( "\ttype: udp" ) ;
                        break ;
                case UGA_SOCK_RAW:
                        UGA_INFO( "\ttype: raw" ) ;
                        break ;
                default:
                        UGA_INFO( "\ttype: unknown" ) ;
                        break ;
        }
}

void uga_sock_log_s ( char const * scope, uga_socket const sock )
{
        ( void ) scope ;
        ( void )  sock ;

        UGA_INFO_S( scope, "socket info:" ) ;
        UGA_INFO_S( scope, "    sockfd: %d", sock.sockfd ) ;
        switch( sock.addr.family )
        {
                case UGA_SFAM_IPv4:
                        UGA_INFO_S( scope, "    family: IPv4" ) ;
                        break ;
                case UGA_SFAM_IPv6:
                        UGA_INFO_S( scope, "    family: IPv6" ) ;
                        break ;
                default:
                        UGA_INFO_S( scope, "    family: unknown" ) ;
                        break ;
        }
        switch( sock.addr.type )
        {
                case UGA_SOCK_STREAM:
                        UGA_INFO_S( scope, "    type: tcp" ) ;
                        break ;
                case UGA_SOCK_DGRAM:
                        UGA_INFO_S( scope, "    type: udp" ) ;
                        break ;
                case UGA_SOCK_RAW:
                        UGA_INFO_S( scope, "    type: raw" ) ;
                        break ;
                default:
                        UGA_INFO_S( scope, "    type: unknown" ) ;
                        break ;
        }
}

////////////////////////////////////////////////////////////////////////////////
/// conversions
////////////////////////////////////////////////////////////////////////////////

uga_addrinfo _uga_addr_from_posix ( _addrinfo_t const * addr )
{
        if( !addr )
        {
                uga_addrinfo info = { 0 } ;
                return info ;
        }
        uga_addrinfo addrinfo = {
                addr->ai_flags     ,
                addr->ai_family    ,
                addr->ai_socktype  ,
                addr->ai_protocol  ,
                addr->ai_addrlen   ,
                { 0 }              ,
                { 0 }              ,
                addr->ai_next
        };
        memcpy( &addrinfo.addr, addr->ai_addr, addr->ai_addrlen ) ;
        if( addr->ai_canonname )
        {
                memcpy(  addrinfo.canon_name, addr->ai_canonname, strlen( addr->ai_canonname ) ) ;
        }
        return addrinfo ;
}
