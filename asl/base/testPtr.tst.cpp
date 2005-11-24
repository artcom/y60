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
//    $RCSfile: testPtr.tst.cpp,v $
//
//   $Revision: 1.14 $
//
// Description: unit test for Ptr class
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "settings.h"

#include "Ptr.h"
#include "Time.h"
#include "Dashboard.h"

#include "MultiThreadedUnitTest.h"

#include "string_functions.h"

#ifndef WIN32
#include <unistd.h>
#endif

#include <string>
#include <iostream>
#include <vector>

using namespace std;

// #define SINGLE_CPU

#define X_NEW_PTR

template <int N>
class TestClassBase {
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

    // char dummy[1000000];
};

template<>
unsigned int TestClassBase<0>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<1>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<2>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<3>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<4>::_theInstanceCount_ = 0;

template <int N>
class TestClassDerived : public TestClassBase<N> {
public:
    virtual const char * getName() {
        return "TestClassDerived";
    }
};

using namespace asl;

#ifdef _SETTING_USE_SGI_STRINGS_
#endif
#define perform_std_string_test

template <int N, class ThreadingModel = asl::MultiProcessor>
class PtrUnitTest : public TemplateUnitTest {
public:
    PtrUnitTest() : TemplateUnitTest("PtrUnitTest-",asl::as_string(N).c_str()) {  }
    void run() {

        {
            // Test empty pointer behavior
            ENSURE(TestClassBase<N>::_theInstanceCount_ == 0);
            Ptr<TestClassBase<N>,ThreadingModel> emptyBasePtr;
            ENSURE(!emptyBasePtr);
            ENSURE(emptyBasePtr.getNativePtr() == 0);
            //DPRINT(emptyBasePtr.getRefCount());
            ENSURE(emptyBasePtr.getRefCount() == 1);
            //DPRINT(emptyBasePtr.getRefCount());
            // Test single non-empty pointer behaviour
            Ptr<TestClassBase<N>,ThreadingModel> myBasePtr(new TestClassBase<N> );
            ENSURE(myBasePtr);
            ENSURE(TestClassBase<N>::_theInstanceCount_ == 1);
            //DPRINT(TestClassBase<N>::_theInstanceCount_);
            ENSURE(myBasePtr.getNativePtr() != 0);
            ENSURE(myBasePtr.getRefCount() == 1);

            ENSURE(myBasePtr != emptyBasePtr);
#ifdef perform_std_string_test
            ENSURE(string("TestClassBase") == myBasePtr->getName());
            ENSURE(string("TestClassBase") == (*myBasePtr).getName());
#else
            ENSURE(strcmp("TestClassBase", myBasePtr->getName()) == 0);
            ENSURE(strcmp("TestClassBase", (*myBasePtr).getName()) == 0);
#endif
            // Test assignment behaviour
            {
                Ptr<TestClassBase<N>,ThreadingModel> anotherPtr = myBasePtr;
                ENSURE(anotherPtr);
                ENSURE(TestClassBase<N>::_theInstanceCount_ == 1);
                ENSURE(anotherPtr == myBasePtr);
                ENSURE(myBasePtr.getRefCount() == 2);
                ENSURE(anotherPtr.getRefCount() == 2);
                ENSURE(anotherPtr.getNativePtr() == myBasePtr.getNativePtr());
#ifdef perform_std_string_test
                ENSURE(string("TestClassBase") == anotherPtr->getName());
                ENSURE(string("TestClassBase") == (*anotherPtr).getName());
#else
                ENSURE(strcmp("TestClassBase",anotherPtr->getName()) == 0);
                ENSURE(strcmp("TestClassBase",(*anotherPtr).getName()) == 0);
#endif
            }
            ENSURE(myBasePtr.getRefCount() == 1);
            //DPRINT(myBasePtr.getRefCount());

            // Test polymorph behaviour
            Ptr<TestClassDerived<N>,ThreadingModel> derivedPtr(new TestClassDerived<N>);
            ENSURE(derivedPtr);
            ENSURE(TestClassBase<N>::_theInstanceCount_ == 2);
            //DPRINT(TestClassBase<N>::_theInstanceCount_);
#ifdef perform_std_string_test
            ENSURE(string("TestClassDerived") == (*derivedPtr).getName());
#else
            ENSURE(strcmp("TestClassDerived",(*derivedPtr).getName()) == 0);
#endif
            ENSURE(myBasePtr.getRefCount() == 1);

            Ptr<TestClassBase<N>,ThreadingModel> anotherBase = derivedPtr;
            ENSURE(anotherBase);
            ENSURE(anotherBase == derivedPtr);

            Ptr<TestClassDerived<N>,ThreadingModel> anotherDerived;
            ENSURE(!anotherDerived);
#ifdef X_NEW_PTR
            anotherDerived = dynamic_cast_Ptr<TestClassDerived<N> >(anotherBase);
#else
            anotherDerived = anotherBase;
#endif
            ENSURE(anotherDerived);
        }
        ENSURE(TestClassBase<N>::_theInstanceCount_ == 0);

        {
            WeakPtr<int, ThreadingModel> myEmptyPtr;
            ENSURE( ! myEmptyPtr );
            ENSURE( ! myEmptyPtr.lock() );

            Ptr<int, ThreadingModel> mySmartPtr = Ptr<int, ThreadingModel>( new int(23));
            WeakPtr<int, ThreadingModel> myWeakPtr(mySmartPtr);
            ENSURE(myWeakPtr);
            ENSURE( myWeakPtr.getWeakCount() == 2);
            ENSURE( myWeakPtr.lock());
            // test second weak pointer
            WeakPtr<int, ThreadingModel> myOtherWeakPtr(mySmartPtr);
            ENSURE( myOtherWeakPtr );
            ENSURE( myOtherWeakPtr.getWeakCount() == 3);
            ENSURE( myOtherWeakPtr.lock());
            // test copy constructor
            WeakPtr<int, ThreadingModel> myCopyWeakPtr(myWeakPtr);
            ENSURE( myCopyWeakPtr );
            ENSURE( myCopyWeakPtr.getWeakCount() == 4);
            ENSURE( myCopyWeakPtr.lock());

            WeakPtr<int, ThreadingModel> myAssignedPtr;
            ENSURE( ! myAssignedPtr );
            ENSURE( ! myAssignedPtr.lock() );
            myAssignedPtr = myWeakPtr;
            ENSURE( myAssignedPtr );
            ENSURE( myCopyWeakPtr.getWeakCount() == 5);
            ENSURE( myAssignedPtr.lock() );

            // test locking
            // This also demonstrates how weak pointers should be used in an application
            if (Ptr<int, ThreadingModel> myLockedPtr = myWeakPtr.lock()) {
                ENSURE( * myLockedPtr == 23);
            } else {
                FAILURE("Could not lock WeakPtr");
            }
            if (Ptr<int, ThreadingModel> myCopyLockedPtr = myCopyWeakPtr.lock()) {
                ENSURE( * myCopyLockedPtr == 23);
            } else {
                FAILURE("Could not lock WeakPtr");
            }

            // dispose smart ptr, invalidating the weak pointers
            mySmartPtr = Ptr<int, ThreadingModel>(0);
            ENSURE( ! myWeakPtr);
            ENSURE( ! myWeakPtr.lock());

            ENSURE( ! myCopyWeakPtr);
            ENSURE( ! myCopyWeakPtr.lock());

            ENSURE( ! myOtherWeakPtr);
            ENSURE( ! myOtherWeakPtr.lock());

            // check the weak refcount decreases (mySmartPtr is null)
            myAssignedPtr = WeakPtr<int, ThreadingModel>(mySmartPtr);
            ENSURE( myWeakPtr.getWeakCount() == 3);
            myOtherWeakPtr = WeakPtr<int, ThreadingModel>(mySmartPtr);
            ENSURE( myWeakPtr.getWeakCount() == 2);
            ENSURE( myCopyWeakPtr.getWeakCount() == 2);
            myCopyWeakPtr = WeakPtr<int, ThreadingModel>(mySmartPtr);
            ENSURE( myWeakPtr.getWeakCount() == 1);
        }

        // test equality operator
        {
            WeakPtr<int, ThreadingModel> myEmptyPtr;
            ENSURE( ! myEmptyPtr );
            ENSURE( ! myEmptyPtr.lock() );

            Ptr<int, ThreadingModel> mySmartPtr = Ptr<int, ThreadingModel>( new int(23));
            Ptr<int, ThreadingModel> mySmartPtrClone = mySmartPtr;
            Ptr<int, ThreadingModel> mySmartPtr2 = Ptr<int, ThreadingModel>( new int(23));
            WeakPtr<int, ThreadingModel> myWeakPtr(mySmartPtr);
            ENSURE(myWeakPtr);
            ENSURE( myWeakPtr.getWeakCount() == 2);
            ENSURE( myWeakPtr.lock());

            WeakPtr<int, ThreadingModel> myWeakPtr2(mySmartPtr2);
            ENSURE(myWeakPtr2);
            ENSURE(myWeakPtr2.getWeakCount() == 2);
            ENSURE(myWeakPtr2.lock());

            ENSURE( mySmartPtr != mySmartPtr2);
            ENSURE( mySmartPtr == myWeakPtr.lock());
            ENSURE( mySmartPtr == mySmartPtrClone);
            ENSURE( mySmartPtrClone == mySmartPtr);
            ENSURE( mySmartPtr == myWeakPtr);
            ENSURE( myWeakPtr == mySmartPtr);
            ENSURE( myWeakPtr == mySmartPtrClone);
            ENSURE( mySmartPtrClone == myWeakPtr);
            ENSURE( myWeakPtr != myWeakPtr2);
            ENSURE( myWeakPtr2 != myWeakPtr);
            ENSURE( myWeakPtr != myEmptyPtr);
            ENSURE(! (myWeakPtr == myEmptyPtr));
        }
#if 0  // TODO: NOT POSSIBLE?
        {
            // test weak pointer polymorphism
            Ptr<TestClassDerived<N>,ThreadingModel> derivedPtr(new TestClassDerived<N>);
            WeakPtr<TestClassDerived<N>, ThreadingModel> weakDerived(derivedPtr);
            ENSURE(weakDerived);
            WeakPtr<TestClassBase<N>, ThreadingModel> weakBase(weakDerived);
            ENSURE(weakBase);
            derivedPtr = Ptr<TestClassDerived<N>, ThreadingModel>(0);
            ENSURE(weakBase == false);
            ENSURE(weakDerived == false);
        }
#endif
    }
};

template <int N>
class PtrUnitTest2 : public TemplateUnitTest {
public:
    PtrUnitTest2() : TemplateUnitTest("PtrUnitTest2-",asl::as_string(N).c_str()) {  }
    void run() {
        ENSURE(TestClassBase<N>::_theInstanceCount_ == 0);
        std::vector<Ptr<TestClassDerived<N> > > myVec;
        for (int i = 0; i < 1000; ++i) {
            myVec.push_back(Ptr<TestClassDerived<N> >(new TestClassDerived<N> ));
        }
        ENSURE(TestClassBase<N>::_theInstanceCount_ == 1000);

        std::vector<Ptr<TestClassDerived<N> > > otherBaseVec;
        otherBaseVec = myVec;

        std::vector<Ptr<TestClassBase<N> > > myBaseVec(1000);

        std::copy(myVec.begin(),myVec.end(),myBaseVec.begin());
        ENSURE(equal(myBaseVec.begin(), myBaseVec.end(), otherBaseVec.begin()));

        ENSURE(TestClassBase<N>::_theInstanceCount_ == 1000);
        myBaseVec.resize(0);
        otherBaseVec.resize(0);

        myVec.resize(0);
        ENSURE(TestClassBase<N> ::_theInstanceCount_ == 0);

    }
};

class PtrMultiThreadTest : public MultiThreadedTestSuite  {
//class PtrMultiThreadTest : public UnitTestSuite  {
public:
    PtrMultiThreadTest() : MultiThreadedTestSuite("PtrMultiThreadTest") {
    //PtrMultiThreadTest() : UnitTestSuite("PtrMultiThreadTest") {
        cerr << "Created PtrMultiThreadTest()" << endl;
    }

    void setup() {
        addTest(new ThreadedTemplateUnitTest< Repeat<100000,PtrUnitTest<1> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<100000,PtrUnitTest<2> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<1000,PtrUnitTest2<3> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<1000,PtrUnitTest2<4> > > );
    }
};


template <int N, class Threading, class Allocator>
class AllocatorUnitTest : public TemplateUnitTest {
public:
    AllocatorUnitTest(std::string theName) : TemplateUnitTest("AllocatorUnitTest-",std::string(theName+asl::as_string(N)).c_str()) {  }
    void run() {

#if DEBUG_VARIANT
        const int myTestCount = 10000;
#else
        const int myTestCount = 1000000;
#endif
        std::vector<int*> myNativePtrs(myTestCount);
        for (int i = 0; i < myTestCount; ++i) {
            myNativePtrs[i] = new int;
        }

        std::vector<Ptr<int,Threading, Allocator> > myPointers(myTestCount);
        asl::Time myAllocStart;
        for (int i = 0; i < myTestCount; ++i) {
            myPointers[i] = Ptr<int,Threading, Allocator>(myNativePtrs[i]);
        }
        double myAllocTime = asl::Time() - myAllocStart;

        asl::Time myFreeStart;
        myPointers.resize(0);
        double myFreeTime = asl::Time() - myFreeStart;

        // second run
        for (int i = 0; i < myTestCount; ++i) {
            myNativePtrs[i] = new int;
        }
        std::vector<Ptr<int,Threading, Allocator> > myPointers2(myTestCount);
        asl::Time myReAllocStart;
        for (int i = 0; i < myTestCount; ++i) {
            myPointers2[i] = Ptr<int,Threading, Allocator>(myNativePtrs[i]);
        }
        double myReAllocTime = asl::Time() - myReAllocStart;

        asl::Time myReFreeStart;
        myPointers2.resize(0);
        double myReFreeTime = asl::Time() - myReFreeStart;

        cerr << "First  Allocating took " << myAllocTime << ", " << myTestCount/myAllocTime << " ptr/sec" << endl;
        cerr << "First  Freeing    took " << myFreeTime << ", " << myTestCount/myFreeTime << " ptr/sec" << endl;
        cerr << "Second Allocating took " << myReAllocTime << ", " << myTestCount/myReAllocTime << " ptr/sec" << endl;
        cerr << "Second Freeing    took " << myReFreeTime << ", " << myTestCount/myReFreeTime << " ptr/sec" << endl;
   }
};
template <int N, class ThreadingModel>
class PtrUnitPerfTest : public TemplateUnitTest {
public:
    PtrUnitPerfTest() : TemplateUnitTest("PtrUnitPerfTest-",asl::as_string(N).c_str()) {  }
    void run() {
#if DEBUG_VARIANT
        const int repeatCount = 10000;
#else
        const int repeatCount = 1000000;
#endif
       for (int f = 0; f < 4; ++f) {
           {
               MAKE_SCOPE_TIMER(PtrUnitPerfTest);
               ENSURE(TestClassBase<N>::_theInstanceCount_ == 0);
               std::vector<Ptr<TestClassDerived<N>, ThreadingModel> > myVec;
               for (int i = 0; i < repeatCount; ++i) {
                   myVec.push_back(Ptr<TestClassDerived<N>, ThreadingModel>(new TestClassDerived<N> ));
               }
               ENSURE(TestClassBase<N>::_theInstanceCount_ == repeatCount);

               START_TIMER(vector_copy1);
               std::vector<Ptr<TestClassDerived<N>, ThreadingModel> > otherBaseVec;
               otherBaseVec = myVec;
               STOP_TIMER_N(vector_copy1,repeatCount);

               std::vector<Ptr<TestClassBase<N>, ThreadingModel> > myBaseVec(repeatCount);

               START_TIMER(vector_copy2);
               std::copy(myVec.begin(),myVec.end(),myBaseVec.begin());
               STOP_TIMER_N(vector_copy2,repeatCount);
               ENSURE(equal(myBaseVec.begin(), myBaseVec.end(), otherBaseVec.begin()));

               START_TIMER(vector_equal);
               bool myEqual = equal(myBaseVec.begin(), myBaseVec.end(), otherBaseVec.begin());
               STOP_TIMER_N(vector_equal,repeatCount);
               ENSURE(myEqual);


               ENSURE(TestClassBase<N>::_theInstanceCount_ == repeatCount);
               START_TIMER(vector_clear);
               myBaseVec.resize(0);
               otherBaseVec.resize(0);

               myVec.resize(0);
               STOP_TIMER_N(vector_clear, repeatCount);
               ENSURE(TestClassBase<N> ::_theInstanceCount_ == 0);
           }
           getDashboard().cycle();
       }
       getDashboard().print(std::cerr);
       getDashboard().reset();
    }
};
class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new PtrUnitTest<0,SingleThreaded>);
        PtrUnitTest<0,SingleProcessor> * mySingleProcessorTest = new PtrUnitTest<0,SingleProcessor>();
#ifdef SINGLE_CPU
        addTest(mySingleProcessorTest);
#else
        delete mySingleProcessorTest;
#endif        
        addTest(new PtrUnitTest<0,MultiProcessor>);

#if 1
        // TODO: adapt all these nifty allocators to support the WeakPtr stuff
        addTest(new AllocatorUnitTest<0,SingleThreaded,PtrFreeListAllocator>("PtrFreeListAllocator"));
        addTest(new AllocatorUnitTest<0,SingleThreaded,PtrFreeListChunkAllocator<SingleThreaded> >("PtrFreeListChunkAllocator"));

        // addTest(new AllocatorUnitTest<0,MultiProcessor,PtrHeapAllocator<MultiProcessor> >("PtrHeapAllocator"));
        addTest(new AllocatorUnitTest<0,MultiProcessor,MutexPtrFreeListAllocator>("MutexPtrFreeListAllocator"));
        //addTest(new AllocatorUnitTest<0,MultiProcessor,PtrThreadSpecificFreeListAllocator>("PtrThreadSpecificFreeListAllocator"));
        addTest(new AllocatorUnitTest<0,MultiProcessor,SpinLockedPtrFreeListAllocator>("SpinLockedPtrFreeListAllocator"));
        addTest(new AllocatorUnitTest<0,MultiProcessor,PtrThreadSpecificFreeListChunkAllocator>("PtrThreadSpecificFreeListChunkAllocator"));
        addTest(new PtrUnitTest2<0>);
        addTest(new PtrMultiThreadTest);
#endif

        addTest(new PtrUnitPerfTest<0,SingleThreaded>);
        PtrUnitPerfTest<0,SingleProcessor> * mySingleProcessorPrefTest = new PtrUnitPerfTest<0,SingleProcessor>();
#ifdef SINGLE_CPU
        addTest(mySingleProcessorPrefTest);
#else
        delete mySingleProcessorPrefTest;
#endif        
        addTest(new PtrUnitPerfTest<0,MultiProcessor>);
    }
};


int main(int argc, char *argv[]) {

    asl::initialize();

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
