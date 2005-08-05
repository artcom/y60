#ifndef _ITEST_INCLUDED
#define _ITEST_INCLUDED

#include <asl/UnitTest.h>

struct ITest {
	virtual void addTests(UnitTestSuite & theSuite) = 0;
};

#endif
