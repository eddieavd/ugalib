//
//
//      uganet
//      test.h
//

#ifndef UGA_TEST_H_
#define UGA_TEST_H_

#include <core/uga_log.h>

//#include <stdlib.h>


static inline void test_log ( char const * test, int condition )
{
        if( condition )
        {
                UGA_SUCC_S( test, "PASS" ) ;
        }
        else
        {
                UGA_FAIL_S( test, "FAIL" ) ;
//              exit( 1 ) ;
        }
}


#endif // UGA_TEST_H_
