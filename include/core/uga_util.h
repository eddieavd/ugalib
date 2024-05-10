//
//
//      uganet
//      uga_util.h
//

#ifndef UGA_UTIL_H_
#define UGA_UTIL_H_

#define UGA_ALIGN( x ) __attribute__(( aligned( x ) ))


enum endian
{
#if defined( _MSC_VER ) && !defined( __clang__ )
        UGA_ENDIAN_LITTLE =                 0 ,
        UGA_ENDIAN_BIG    =                 1 ,
        UGA_ENDIAN_NATIVE = UGA_ENDIAN_LITTLE ,
#else
        UGA_ENDIAN_LITTLE = __ORDER_LITTLE_ENDIAN__ ,
        UGA_ENDIAN_BIG    = __ORDER_BIG_ENDIAN__    ,
        UGA_ENDIAN_NATIVE = __BYTE_ORDER__          ,
#endif
};


#endif // UGA_UTIL_H_
