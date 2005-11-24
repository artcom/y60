/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: MultiThreadedUnitTest.h,v $
//
//   $Revision: 1.4 $
//
// Description: unit test for Ptr class
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _asl_MultiThreadedUnitTest_included_
#define _asl_MultiThreadedUnitTest_included_

#include "UnitTest.h"

#include "PosixThread.h"
#include "string_functions.h"

#include <string>
#include <iostream>
#include <vector>


namespace asl {


    /*! \addtogroup aslbase */
    /* @{ */
    
    struct ThreadInterface {
        virtual void fork() = 0;
        virtual void join() = 0;
    };


    template <class TEST>
        class ThreadedTemplateUnitTest : public TEST, public ThreadInterface {
            public:
#ifdef WIN32
#pragma warning(push)
// TODO: This is not a fix.
#pragma warning(disable:4355)     // This used in base member initializer list is unsafe.
#endif
                ThreadedTemplateUnitTest() : TEST(), _myThread(this) {
                    std::cerr << "Created ThreadedTemplateUnitTest this = " << this << ", _myThread = " << &_myThread << std::endl;
                }
#ifdef WIN32
#pragma warning(pop)
#endif

                PosixThread & getThread() {
                    return _myThread;
                }
                void fork() {
                    _myThread.fork();
                    std::cerr << "@@ Forked thread for test named '" << TEST::getMyName()
                        << "' with thread id " << _myThread.getThreadID() << std::endl;
                }
                void run() {
                    std::cerr << "run ThreadedTemplateUnitTest" << std::endl;

                    TEST::run();
                }
                void join() {
                    _myThread.join();
                }
            protected:
                struct MyThread : public PosixThread {
                    MyThread(TEST * theTest) : myTest(theTest) {}
                    TEST * myTest;
                    void run() {
                        std::cerr << "@@ Running test named '" << myTest->getMyName()
                            << "' with thread id " << getThreadID()
                            << " this= " << this << std::endl;
                        try {
                            myTest->run();
                        }
                        catch (asl::Exception & ex) {
                            std::cerr << "\n##### Exception in test '" << myTest->getMyName()
                                << "' with thread id " << getThreadID()
                                << " this= " << this <<  std::endl;
                            std::cerr << ex << std::endl;
                            myTest->FAILURE("Exception");
                        }
                        catch (...) {
                            std::cerr << "\n##### Unknown Exception in test '" << myTest->getMyName()
                                << "' with thread id " << getThreadID()
                                << " this= " << this << std::endl;
                            myTest->FAILURE("Exception");
                        }
                        std::cerr << "@@ Finished test named '" << myTest->getMyName()
                            << " with thread id '" << getThreadID() << std::endl;
                    };
                };
                MyThread _myThread;
        };

    class MultiThreadedTestSuite : public UnitTestSuite {
        public:
            MultiThreadedTestSuite(const char * theName) : UnitTestSuite(theName) {  }

            virtual void run();
    };

    class CatchingUnitTestSuite : public UnitTestSuite {
        public:
            CatchingUnitTestSuite(const char * theName) : UnitTestSuite(theName) {  }

            virtual void run();
    };


    /* @} */


}
#endif
