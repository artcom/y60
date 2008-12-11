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
//    $RCSfile: testMemoryPool.tst.cpp,v $
//
//   $Revision: 1.2 $
//
// Description: unit test for MemoryPool class
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "settings.h"
#include "MemoryPool.h"
#include "MultiThreadedUnitTest.h"
#include "string_functions.h"
#include "Time.h"

#ifndef WIN32
#include <unistd.h>
#endif

#include <string>
#include <iostream>
#include <vector>

using namespace std;

template <int N, template <class> class Allocator>
class TestClassBase : public Allocator<TestClassBase<N,Allocator> > {
public:
    TestClassBase() {
        ++_theInstanceCount_;
    }
    virtual ~TestClassBase() {
        --_theInstanceCount_;
    }
    virtual const char * getName() {
        return "TestClassBase";
    }
    static unsigned int _theInstanceCount_;

    char dummy[16];
    // char dummy[1000000];
};

template<>
unsigned int TestClassBase<0,asl::StandardAllocator>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<0,asl::PoolAllocator>::_theInstanceCount_ = 0;
/*
unsigned int TestClassBase<1>::_theInstanceCount_ = 0;
unsigned int TestClassBase<2>::_theInstanceCount_ = 0;
unsigned int TestClassBase<3>::_theInstanceCount_ = 0;
unsigned int TestClassBase<4>::_theInstanceCount_ = 0;
template <int N>
class TestClassDerived : public TestClassBase<N> {
public:
    virtual const char * getName() {
        return "TestClassDerived";
    }
};
*/

using namespace asl;

template <int N, template <class> class Allocator, class ThreadingModel = asl::MultiProcessor>
class MemoryPoolUnitTest : public TemplateUnitTest {
public:
    MemoryPoolUnitTest(std::string theName) : TemplateUnitTest("MemoryPoolUnitTest-",std::string(theName+asl::as_string(N)).c_str()) {  }
    void run() {

        const int myTestCount = 1000000;
        typedef TestClassBase<N,Allocator> TestClass;
        std::vector<TestClass *> myObjects(myTestCount);
        ENSURE(TestClass::_theInstanceCount_ == 0);
        asl::Time myAllocStart;
        for (int i = 0; i < myTestCount; ++i) {
            myObjects[i] = new TestClass;
        }
        double myAllocTime = asl::Time() - myAllocStart;
        ENSURE(TestClass::_theInstanceCount_ == myTestCount);
        ENSURE(TestClass::getAllocatedObjectsCount() == myTestCount);
        ENSURE(TestClass::getFreedObjectsCount() == 0);

        asl::Time myFreeStart;
        for (int i = 0; i < myTestCount; ++i) {
            delete myObjects[i];
        }
        double myFreeTime = asl::Time() - myFreeStart;
        ENSURE(TestClass::_theInstanceCount_ == 0);
        ENSURE(TestClass::getAllocatedObjectsCount() == myTestCount);
        ENSURE(TestClass::getFreedObjectsCount() == myTestCount);

        // second run
        asl::Time myReAllocStart;
        for (int i = 0; i < myTestCount; ++i) {
            myObjects[i] = new TestClass;
        }
        double myReAllocTime = asl::Time() - myReAllocStart;
        ENSURE(TestClass::_theInstanceCount_ == myTestCount);
        ENSURE(TestClass::getAllocatedObjectsCount() == myTestCount * 2);
        ENSURE(TestClass::getFreedObjectsCount() == myTestCount);

        asl::Time myReFreeStart;
        for (int i = 0; i < myTestCount; ++i) {
            delete myObjects[i];
        }
        double myReFreeTime = asl::Time() - myReFreeStart;
        ENSURE(TestClass::_theInstanceCount_ == 0);
        ENSURE(TestClass::getAllocatedObjectsCount() == myTestCount * 2);
        ENSURE(TestClass::getFreedObjectsCount() == myTestCount * 2);

        cerr << "First  Allocating took " << myAllocTime << ", " << myTestCount/myAllocTime << " objects/sec" << endl;
        cerr << "First  Freeing    took " << myFreeTime << ", " << myTestCount/myFreeTime << " objects/sec" << endl;
        cerr << "Second Allocating took " << myReAllocTime << ", " << myTestCount/myReAllocTime << " objects/sec" << endl;
        cerr << "Second Freeing    took " << myReFreeTime << ", " << myTestCount/myReFreeTime << " objects/sec" << endl;
   }
};

/*

class MemoryPoolMultiThreadTest : public MultiThreadedTestSuite  {
//class MemoryPoolMultiThreadTest : public UnitTestSuite  {
public:
    MemoryPoolMultiThreadTest() : MultiThreadedTestSuite("MemoryPoolMultiThreadTest") {
    //MemoryPoolMultiThreadTest() : UnitTestSuite("MemoryPoolMultiThreadTest") {
        cerr << "Created MemoryPoolMultiThreadTest()" << endl;
    }

    void setup() {
        addTest(new ThreadedTemplateUnitTest< Repeat<100000,MemoryPoolUnitTest<1> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<100000,MemoryPoolUnitTest<2> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<1000,MemoryPoolUnitTest2<3> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<1000,MemoryPoolUnitTest2<4> > > );
    }
};
*/

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new MemoryPoolUnitTest<0,StandardAllocator,SingleThreaded>("StandardAllocator"));
        addTest(new MemoryPoolUnitTest<0,PoolAllocator,SingleThreaded>("PoolAllocator"));
        //addTest(new MemoryPoolUnitTest<0,SingleProcessor>);
        //addTest(new MemoryPoolUnitTest<0,MultiProcessor>);
        //addTest(new MemoryPoolMultiThreadTest);
    }
};


int main(int argc, char *argv[]) {

    asl::initialize();
    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
