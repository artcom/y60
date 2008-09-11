#ifndef _ITEST_INCLUDED
#define _ITEST_INCLUDED

#include <asl/base/UnitTest.h>

struct ITest {
	virtual void addTests(UnitTestSuite & theSuite) = 0;
};

#endif
