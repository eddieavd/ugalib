//
//
//      uganet
//      test.h
//

#ifndef UGA_TEST_H_
#define UGA_TEST_H_

#include <core/uga_types.h>
#include <core/uga_log.h>


static inline bool test_log ( char const * test, int condition )
{
        if( condition )
        {
                UGA_SUCC_S( test, "PASS" ) ;
                return true ;
        }
        else
        {
                UGA_FAIL_S( test, "FAIL" ) ;
                return false ;
        }
}


#endif // UGA_TEST_H_
