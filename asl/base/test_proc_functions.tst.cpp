/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: test_os_functions.tst.cpp,v $
//
//   $Revision: 1.1 $
//
// Description: unit test for file functions
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "proc_functions.h"
#include "UnitTest.h"
#include "Logger.h"

class ProcUnitTest : public UnitTest {
    public:
        ProcUnitTest() : UnitTest("ProcUnitTest") {
        }

        void run() {
            DPRINT(asl::getTotalMemory());
            ENSURE(asl::getTotalMemory() > 0);

            DPRINT(asl::getFreeMemory());
            ENSURE(asl::getFreeMemory() > 0 && asl::getFreeMemory() < asl::getTotalMemory());
            DPRINT(asl::getUsedMemory());
            ENSURE(asl::getUsedMemory() > 0);
            if (asl::getProcessMemoryUsage() > 0) {
                SUCCESS("asl::getProcessMemoryUsage()");
            } else {
#ifdef OSX
                AC_WARNING << "asl::getProcessMemoryUsage() on leopard needs a signed application";
#else
                FAILURE("asl::getProcessMemoryUsage() > 0");
#endif
            }
        }
};


class ProcTestSuite : public UnitTestSuite {
public:
    ProcTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new ProcUnitTest);
    }
};

int main(int argc, char *argv[]) {

    ProcTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();

    return mySuite.returnStatus();
}
