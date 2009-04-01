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
//    $RCSfile: testGarbageCollector.tst.cpp,v $
//
//   $Revision: 1.14 $
//
// Description: unit test for Ptr class
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "GarbageCollector.h"

#include <asl/base/initialization.h>

#include <asl/base/Time.h>
#include <asl/base/Dashboard.h>

#include <asl/base/MultiThreadedUnitTest.h>

#include <asl/base/string_functions.h>

#include <stdlib.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include <string>
#include <iostream>
#include <vector>

using namespace asl;
using namespace std;

#define DB(x) // x
#ifdef OSX
namespace asl {
#endif

// #define SINGLE_CPU

#define X_NEW_PTR
    template <class ThreadingModel, class Allocator>
    std::ostream & operator<<(std::ostream & os, const CollectablePtrBase<ThreadingModel, Allocator>* & thePtr) {
        return os << (void*)thePtr;
    }

template <int N, class ThreadingModel>
class TestClassBase : public CollectableAllocated<ThreadingModel, CollectablePtrAllocator<ThreadingModel> > {
public:
    TestClassBase() {
        ++_theInstanceCount_;
		DB(std::cerr << "TestClassBase(), instances=" <<_theInstanceCount_<<std::endl);
    }
    virtual ~TestClassBase() {
        --_theInstanceCount_;
		DB(std::cerr << "~TestClassBase(), instances=" <<_theInstanceCount_<<std::endl);
    }
    virtual const char * getName() {
        return "TestClassBase";
    }
    static unsigned int _theInstanceCount_;

    // char dummy[1000000];
};

template<>
unsigned int TestClassBase<0, SingleThreaded>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<1, SingleThreaded>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<2, SingleThreaded>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<3, SingleThreaded>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<4, SingleThreaded>::_theInstanceCount_ = 0;

template<>
unsigned int TestClassBase<0, SingleProcessor>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<1, SingleProcessor>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<2, SingleProcessor>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<3, SingleProcessor>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<4, SingleProcessor>::_theInstanceCount_ = 0;

template<>
unsigned int TestClassBase<0, MultiProcessor>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<1, MultiProcessor>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<2, MultiProcessor>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<3, MultiProcessor>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassBase<4, MultiProcessor>::_theInstanceCount_ = 0;

template <int N, class ThreadingModel>
class TestClassDerived : public TestClassBase<N, ThreadingModel> {
public:
    virtual const char * getName() {
        return "TestClassDerived";
    }
};

template <int N, class ThreadingModel>
class TestClassLink : public TestClassBase<N, ThreadingModel> {
public:
    TestClassLink()  {
        ++_theInstanceCount_;
		DB(std::cerr << "TestClassLink(), instances=" <<_theInstanceCount_<<std::endl);
    }
    virtual ~TestClassLink() {
        --_theInstanceCount_;
		DB(std::cerr << "~TestClassLink(), instances=" <<_theInstanceCount_<<std::endl);
    }
    virtual const char * getName() {
        return "TestClassLink";
    }
    static unsigned int _theInstanceCount_;

    //CollectableWeakPtr<TestClassLink<N,ThreadingModel>, ThreadingModel> mySelf;
    CollectablePtr<TestClassLink<N,ThreadingModel>, ThreadingModel> myLink1;
    CollectablePtr<TestClassLink<N,ThreadingModel>, ThreadingModel> myLink2;
};
template<>
unsigned int TestClassLink<0, SingleThreaded>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassLink<0, SingleProcessor>::_theInstanceCount_ = 0;
template<>
unsigned int TestClassLink<0, MultiProcessor>::_theInstanceCount_ = 0;


template <int N, class ThreadingModel>
class TestClassLinkDerived : public TestClassLink<N, ThreadingModel> {
public:
    virtual const char * getName() {
        return "TestClassLink";
    }
    typedef TestClassLink<N, ThreadingModel> Base;
    typedef CollectablePtr<Base, ThreadingModel> BaseGCPtr;
    BaseGCPtr myDLink;
    std::vector<BaseGCPtr, typename BaseGCPtr::StdContainerAllocator> myDVector;
};


#ifdef _SETTING_USE_SGI_STRINGS_
#endif
#define perform_std_string_test
template <int N, class ThreadingModel = asl::MultiProcessor>
class PtrUnitTest : public TemplateUnitTest {
public:
    PtrUnitTest() : TemplateUnitTest("PtrUnitTest-",asl::as_string(N).c_str()) {  }
    void run() {
        setAbortOnFailure(true);

        typedef TestClassBase<N, ThreadingModel> BaseN;
        typedef TestClassDerived<N, ThreadingModel> DerivedN;
        {
            // Test empty pointer behavior
            ENSURE(BaseN::_theInstanceCount_ == 0);
            CollectablePtr<BaseN,ThreadingModel> emptyBasePtr;
            ENSURE(!emptyBasePtr);
            ENSURE(emptyBasePtr.getNativePtr() == 0);
            //DPRINT(emptyBasePtr.getRefCount());
            ENSURE(emptyBasePtr.getRefCount() == 0);
            //DPRINT(emptyBasePtr.getRefCount());
            // Test single non-empty pointer behaviour
            CollectablePtr<BaseN,ThreadingModel> myBasePtr(new BaseN );
            ENSURE(myBasePtr);
            ENSURE(BaseN::_theInstanceCount_ == 1);
            //DPRINT(BaseN::_theInstanceCount_);
            ENSURE(myBasePtr.getNativePtr() != 0);
            ENSURE(myBasePtr.getRefCount() == 1);
            DPRINT(myBasePtr.getRefCount());

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
                CollectablePtr<BaseN,ThreadingModel> anotherPtr = myBasePtr;
                ENSURE(anotherPtr);
                ENSURE(BaseN::_theInstanceCount_ == 1);
                ENSURE(anotherPtr == myBasePtr);
                ENSURE(myBasePtr.getRefCount() == 2);
                DPRINT(myBasePtr.getRefCount());
                ENSURE(anotherPtr.getRefCount() == 2);
                DPRINT(anotherPtr.getRefCount());
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
            CollectablePtr<DerivedN,ThreadingModel> derivedPtr(new DerivedN);
            ENSURE(derivedPtr);
            ENSURE(derivedPtr.getRefCount() == 1);
           
            ENSURE(BaseN::_theInstanceCount_ == 2);
            //DPRINT(TestBase::_theInstanceCount_);
#ifdef perform_std_string_test
            ENSURE(string("TestClassDerived") == (*derivedPtr).getName());
#else
            ENSURE(strcmp("TestClassDerived",(*derivedPtr).getName()) == 0);
#endif
            ENSURE(myBasePtr.getRefCount() == 1);

            CollectablePtr<BaseN, ThreadingModel> anotherBase = derivedPtr;
            ENSURE(anotherBase);
            ENSURE(anotherBase == derivedPtr);
            ENSURE(derivedPtr.getRefCount() == 2);
            ENSURE(anotherBase.getRefCount() == 2);

            CollectablePtr<DerivedN,ThreadingModel> anotherDerived;
            ENSURE(!anotherDerived);
            ENSURE(anotherDerived.getRefCount() == 0);

#ifdef X_NEW_PTR
            anotherDerived = dynamic_cast_CollectablePtr<TestClassDerived<N,ThreadingModel> >(anotherBase);
#else
            anotherDerived = anotherBase;
#endif
            ENSURE(anotherDerived);
            ENSURE(anotherDerived.getDescriptorPtr() == anotherBase.getDescriptorPtr());
            ENSURE(anotherDerived.getRefCount() == 3);
            ENSURE(anotherBase.getRefCount() == 3);
			DPRINT(anotherBase.getRefCount());
        }
        ENSURE(BaseN::_theInstanceCount_ == 0);
        DPRINT(BaseN::_theInstanceCount_);

        {
            CollectableWeakPtr<MakeCollectable<int,ThreadingModel>, ThreadingModel> myEmptyPtr;
            ENSURE( ! myEmptyPtr );
            ENSURE( ! myEmptyPtr.lock() );

            CollectablePtr<MakeCollectable<int,ThreadingModel>, ThreadingModel> mySmartPtr = CollectablePtr<MakeCollectable<int,ThreadingModel>, ThreadingModel>( new MakeCollectable<int,ThreadingModel>(23));
            CollectableWeakPtr<MakeCollectable<int,ThreadingModel>, ThreadingModel> myWeakPtr(mySmartPtr);
            ENSURE(myWeakPtr);
            ENSURE( myWeakPtr.getWeakCount() == 2);
            ENSURE( myWeakPtr.lock());
            // test second weak pointer
            CollectableWeakPtr<MakeCollectable<int,ThreadingModel>, ThreadingModel> myOtherWeakPtr(mySmartPtr);
            ENSURE( myOtherWeakPtr );
            ENSURE( myOtherWeakPtr.getWeakCount() == 3);
            ENSURE( myOtherWeakPtr.lock());
            // test copy constructor
            CollectableWeakPtr<MakeCollectable<int,ThreadingModel>, ThreadingModel> myCopyWeakPtr(myWeakPtr);
            ENSURE( myCopyWeakPtr );
            ENSURE( myCopyWeakPtr.getWeakCount() == 4);
            ENSURE( myCopyWeakPtr.lock());

            CollectableWeakPtr<MakeCollectable<int,ThreadingModel>, ThreadingModel> myAssignedPtr;
            ENSURE( ! myAssignedPtr );
            ENSURE( ! myAssignedPtr.lock() );
            myAssignedPtr = myWeakPtr;
            ENSURE( myAssignedPtr );
            ENSURE( myCopyWeakPtr.getWeakCount() == 5);
            ENSURE( myAssignedPtr.lock() );

            // test locking
            // This also demonstrates how weak pointers should be used in an application
            if (CollectablePtr<MakeCollectable<int,ThreadingModel>, ThreadingModel> myLockedPtr = myWeakPtr.lock()) {
                ENSURE( * myLockedPtr == 23);
            } else {
                FAILURE("Could not lock WeakPtr");
            }
            if (CollectablePtr<MakeCollectable<int,ThreadingModel>, ThreadingModel> myCopyLockedPtr = myCopyWeakPtr.lock()) {
                ENSURE( * myCopyLockedPtr == 23);
            } else {
                FAILURE("Could not lock WeakPtr");
            }

            // dispose smart ptr, invalidating the weak pointers
            mySmartPtr = CollectablePtr<MakeCollectable<int,ThreadingModel>, ThreadingModel>(0);
            ENSURE( ! myWeakPtr);
            ENSURE( ! myWeakPtr.lock());

            ENSURE( ! myCopyWeakPtr);
            ENSURE( ! myCopyWeakPtr.lock());

            ENSURE( ! myOtherWeakPtr);
            ENSURE( ! myOtherWeakPtr.lock());

            // check the weak refcount decreases (mySmartPtr is null)
            myAssignedPtr = CollectableWeakPtr<MakeCollectable<int,ThreadingModel>, ThreadingModel>(mySmartPtr);
            ENSURE( myWeakPtr.getWeakCount() == 3);
            myOtherWeakPtr = CollectableWeakPtr<MakeCollectable<int,ThreadingModel>, ThreadingModel>(mySmartPtr);
            ENSURE( myWeakPtr.getWeakCount() == 2);
            ENSURE( myCopyWeakPtr.getWeakCount() == 2);
            myCopyWeakPtr = CollectableWeakPtr<MakeCollectable<int,ThreadingModel>, ThreadingModel>(mySmartPtr);
            ENSURE( myWeakPtr.getWeakCount() == 1);
        }

        // test equality operator
        {
            CollectableWeakPtr<MakeCollectable<int,ThreadingModel>, ThreadingModel> myEmptyPtr;
            ENSURE( ! myEmptyPtr );
            ENSURE( ! myEmptyPtr.lock() );

            CollectablePtr<MakeCollectable<int,ThreadingModel>, ThreadingModel> mySmartPtr = CollectablePtr<MakeCollectable<int,ThreadingModel>, ThreadingModel>( new MakeCollectable<int,ThreadingModel>(23));
            CollectablePtr<MakeCollectable<int,ThreadingModel>, ThreadingModel> mySmartPtrClone = mySmartPtr;
            CollectablePtr<MakeCollectable<int,ThreadingModel>, ThreadingModel> mySmartPtr2 = CollectablePtr<MakeCollectable<int,ThreadingModel>, ThreadingModel>( new MakeCollectable<int,ThreadingModel>(23));
            CollectableWeakPtr<MakeCollectable<int,ThreadingModel>, ThreadingModel> myWeakPtr(mySmartPtr);
            ENSURE(myWeakPtr);
            ENSURE( myWeakPtr.getWeakCount() == 2);
            ENSURE( myWeakPtr.lock());

            CollectableWeakPtr<MakeCollectable<int,ThreadingModel>, ThreadingModel> myWeakPtr2(mySmartPtr2);
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
            CollectablePtr<TestClassDerived<N>,ThreadingModel> derivedPtr(new TestClassDerived<N>);
            CollectableWeakPtr<TestClassDerived<N>, ThreadingModel> weakDerived(derivedPtr);
            ENSURE(weakDerived);
            CollectableWeakPtr<TestClassBase<N>, ThreadingModel> weakBase(weakDerived);
            ENSURE(weakBase);
            derivedPtr = CollectablePtr<TestClassDerived<N>, ThreadingModel>(0);
            ENSURE(weakBase == false);
            ENSURE(weakDerived == false);
        }
#endif
    }
};

template <int N, class ThreadingModel = asl::MultiProcessor>
class PtrUnitTest2 : public TemplateUnitTest {
public:
    PtrUnitTest2() : TemplateUnitTest("PtrUnitTest2-",asl::as_string(N).c_str()) {  }
    void run() {
        typedef CollectablePtrAllocator<ThreadingModel> Allocator;
        typedef TestClassBase<N, ThreadingModel> BaseN;
        typedef TestClassDerived<N, ThreadingModel> DerivedN;
        ENSURE(BaseN::_theInstanceCount_ == 0);
        std::vector<CollectablePtr<DerivedN>, CollectableContainerAllocator<CollectablePtr<DerivedN>,ThreadingModel,Allocator> > myVec;
        for (int i = 0; i < 1000; ++i) {
            myVec.push_back(CollectablePtr<DerivedN>(new DerivedN ));
        }
        ENSURE(BaseN::_theInstanceCount_ == 1000);

        std::vector<CollectablePtr<DerivedN>, CollectableContainerAllocator<CollectablePtr<DerivedN>,ThreadingModel,Allocator> > otherBaseVec;
        otherBaseVec = myVec;

        std::vector<CollectablePtr<BaseN>, CollectableContainerAllocator<CollectablePtr<BaseN>,ThreadingModel,Allocator> > myBaseVec(1000);

        std::copy(myVec.begin(),myVec.end(),myBaseVec.begin());
        ENSURE(equal(myBaseVec.begin(), myBaseVec.end(), otherBaseVec.begin()));

        ENSURE(BaseN::_theInstanceCount_ == 1000);
        myBaseVec.resize(0);
        otherBaseVec.resize(0);

        myVec.resize(0);
        ENSURE(BaseN::_theInstanceCount_ == 0);

    }
};
template <int N, class ThreadingModel = asl::MultiProcessor>
class PtrLinkTest : public TemplateUnitTest {
public:
    PtrLinkTest() : TemplateUnitTest("PtrLinkTest-",asl::as_string(N).c_str()) {  }
    void run() {
        setAbortOnFailure(true);
        typedef TestClassLink<N, ThreadingModel> BaseN;
        typedef TestClassLinkDerived<N, ThreadingModel> DerivedN;
        typedef CollectablePtrAllocator<ThreadingModel> Allocator;
        typedef Collector<ThreadingModel, Allocator> MyCollector;

        ENSURE(MyCollector::get().getRootPtrCount() == 0);
        DPRINT(MyCollector::get().getRootPtrCount());

        ENSURE(MyCollector::get().getManagedObjectCount() == 0);
        DPRINT(MyCollector::get().getManagedObjectCount());
        
        ENSURE(MyCollector::get().calcManagedPtrCount() == 0);
        DPRINT(MyCollector::get().calcManagedPtrCount());

        ENSURE(BaseN::_theInstanceCount_ == 0);

        const int COUNT = 10;
        DPRINT(COUNT);

        std::vector<CollectablePtr<DerivedN, ThreadingModel> > myVec;
        for (int i = 0; i < COUNT; ++i) {
            myVec.push_back(CollectablePtr<DerivedN, ThreadingModel>(new DerivedN ));
            MyCollector::get().printObjectMap();
        }
        ENSURE(BaseN::_theInstanceCount_ == COUNT);
        DPRINT(BaseN::_theInstanceCount_);
        
        ENSURE(MyCollector::get().getRootPtrCount() == COUNT);
        DPRINT(MyCollector::get().getRootPtrCount());
        
        ENSURE(MyCollector::get().getManagedObjectCount() == COUNT);
        DPRINT(MyCollector::get().getManagedObjectCount());
        
        ENSURE(MyCollector::get().calcManagedPtrCount() == 3*COUNT);
        DPRINT(MyCollector::get().calcManagedPtrCount());
        
        DPRINT(" Starting Crosslinks");
        for (int i = 0; i < COUNT; ++i) {
            if (i>0) myVec[i]->myLink1 = myVec[i-1];
            if (i<COUNT-1) myVec[i]->myLink2 = myVec[i+1];
            myVec[i]->myDLink = myVec[i];
            myVec[i]->myDLink = myVec[i];
            myVec[i]->myDVector.push_back(myVec[i]);
        }
        DPRINT(" Done Crosslinks");
        MyCollector::get().printObjectMap();
        unsigned inUse = MyCollector::get().markAllUsedObjects();
        MyCollector::get().printObjectMap();
        ENSURE(inUse == COUNT);
        DPRINT(inUse);

        myVec.resize(1);
        ENSURE(MyCollector::get().getRootPtrCount() == 1);
        MyCollector::get().printObjectMap();
        inUse = MyCollector::get().markAllUsedObjects();
        ENSURE(inUse == COUNT);
        DPRINT(inUse);
        unsigned swept = MyCollector::get().sweepUnused();
        ENSURE(swept == 0);
        DPRINT(swept);

        myVec.resize(0);
        ENSURE(MyCollector::get().getRootPtrCount() == 0);
        ENSURE(BaseN::_theInstanceCount_ == COUNT);

        inUse = MyCollector::get().markAllUsedObjects();
        ENSURE(inUse == 0);
        DPRINT(inUse);
        swept = MyCollector::get().sweepUnused();
        ENSURE(swept == COUNT);
        DPRINT(swept);

        ENSURE(MyCollector::get().getManagedObjectCount() == 0);
        DPRINT(MyCollector::get().getManagedObjectCount());
        
        ENSURE(MyCollector::get().calcManagedPtrCount() == 0);
        DPRINT(MyCollector::get().calcManagedPtrCount());
     }
};

class PtrMultiThreadTest : public MultiThreadedTestSuite  {
//class PtrMultiThreadTest : public UnitTestSuite  {
public:
    PtrMultiThreadTest(int argc, char *argv[]) : MultiThreadedTestSuite("PtrMultiThreadTest", argc, argv) {
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
        const int myTestCount = 1000;
#else
        const int myTestCount = 100000;
#endif
        std::vector<MakeCollectable<int, Threading>*> myNativePtrs(myTestCount);
        for (int i = 0; i < myTestCount; ++i) {
            myNativePtrs[i] = new MakeCollectable<int, Threading>;
        }

        std::vector<CollectablePtr<MakeCollectable<int, Threading>,Threading, Allocator> > myPointers(myTestCount);
        asl::Time myAllocStart;
        for (int i = 0; i < myTestCount; ++i) {
            myPointers[i] = CollectablePtr<MakeCollectable<int, Threading>,Threading, Allocator>(myNativePtrs[i]);
        }
        double myAllocTime = asl::Time() - myAllocStart;

        asl::Time myFreeStart;
        myPointers.resize(0);
        double myFreeTime = asl::Time() - myFreeStart;

        // second run
        for (int i = 0; i < myTestCount; ++i) {
            myNativePtrs[i] = new MakeCollectable<int, Threading>;
        }
        std::vector<CollectablePtr<MakeCollectable<int, Threading>,Threading, Allocator> > myPointers2(myTestCount);
        asl::Time myReAllocStart;
        for (int i = 0; i < myTestCount; ++i) {
            myPointers2[i] = CollectablePtr<MakeCollectable<int, Threading>,Threading, Allocator>(myNativePtrs[i]);
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
        const int repeatCount = 1000;
#else
        const int repeatCount = 100000;
#endif
        typedef TestClassBase<N, ThreadingModel> BaseN;
        typedef TestClassDerived<N, ThreadingModel> DerivedN;
        typedef CollectablePtr<DerivedN, ThreadingModel> DerivedPtr;
        typedef CollectablePtr<BaseN, ThreadingModel> BasePtr;
        
        typedef std::vector<BasePtr, CollectableContainerAllocator<BasePtr, ThreadingModel> > BaseVector;
        typedef std::vector<DerivedPtr, CollectableContainerAllocator<DerivedPtr, ThreadingModel> > DerivedVector;
        for (int f = 0; f < 4; ++f) {
           {
               MAKE_SCOPE_TIMER(PtrUnitPerfTest);
               ENSURE(BaseN::_theInstanceCount_ == 0);
               DerivedVector myVec;
               for (int i = 0; i < repeatCount; ++i) {
                   myVec.push_back(DerivedPtr(new DerivedN ));
               }
               ENSURE(BaseN::_theInstanceCount_ == repeatCount);

               START_TIMER(vector_copy1);
               DerivedVector otherBaseVec;
               otherBaseVec = myVec;
               STOP_TIMER_N(vector_copy1,repeatCount);

               BaseVector myBaseVec(repeatCount);

               START_TIMER(vector_copy2);
               std::copy(myVec.begin(),myVec.end(),myBaseVec.begin());
               STOP_TIMER_N(vector_copy2,repeatCount);
               ENSURE(equal(myBaseVec.begin(), myBaseVec.end(), otherBaseVec.begin()));

               START_TIMER(vector_equal);
               bool myEqual = equal(myBaseVec.begin(), myBaseVec.end(), otherBaseVec.begin());
               STOP_TIMER_N(vector_equal,repeatCount);
               ENSURE(myEqual);


               ENSURE(BaseN::_theInstanceCount_ == repeatCount);
               START_TIMER(vector_clear);
               myBaseVec.resize(0);
               otherBaseVec.resize(0);

               myVec.resize(0);
               STOP_TIMER_N(vector_clear, repeatCount);
               ENSURE(BaseN::_theInstanceCount_ == 0);
           }
           getDashboard().cycle();
       }
       getDashboard().print(std::cerr);
       getDashboard().reset();
    }
};

template <class T>
class RangeSetUnitTest : public TemplateUnitTest {
public:
    RangeSetUnitTest(const char * theTemplateArgument, const T & theFirstValue) 
    : TemplateUnitTest("RangeSetUnitTest",theTemplateArgument), firstValue(theFirstValue) {  }
    
    enum {myTestSize = 20};

    set<T> mySet;
    RangeSet<T> myRange;
    std::vector<T> v;

    T firstValue;

    bool checkContent() {
        bool rVal = true;
        for (int i = 0; i < v.size();++i) {
            std::cerr << "|"<< setw(2) << i;
        }
        std::cerr << std::endl;
        for (int i = 0; i < v.size();++i) {
            bool existsInSet = mySet.count(v[i])==1; 
            bool existsInRange = myRange.contains(v[i]);
            bool equallyExists =( existsInSet== existsInRange);
            if (!equallyExists) rVal=false;
            std::cerr << "|";
            if (existsInSet) { cerr << "S"; } else cerr << "-";
            if (existsInRange) { cerr << "R"; } else cerr << "-";
        }
        std::cerr << std::endl;
        std::cerr << myRange << endl;
        return rVal; 
    }

    void checkIterating() {
        T rIter;
        for (typename set<T>::iterator it = mySet.begin(); it != mySet.end(); ++it) {
            if (it == mySet.begin()) {
                ENSURE(myRange.getFirst(rIter));
            }
            ENSURE(rIter == *it);
            //DPRINT(rIter);
            //DPRINT(*it);
            typename set<T>::iterator it2 = it;
            ++it2;
            if (it2 != mySet.end()) {
                ENSURE(myRange.advance(rIter));
            } else {
                ENSURE(!myRange.advance(rIter));
            }
        }
        ENSURE(myRange.size() == mySet.size());
    } 

    bool insertTest(const T & bgn, const T & end) {
        checkIterating();
        bool exists = false;
        for (T n = bgn; n < end; ++n) {
            if (mySet.count(n) > 0) {
                exists = true;
            }
        }
        if (!exists) {
            for (T n = bgn; n < end; ++n) {
                mySet.insert(n);
            }
        }

        bool existsInRange = !myRange.insert(bgn, end);
        ENSURE(existsInRange == exists);
        return checkContent() && (existsInRange == exists);
    }
    bool removeTest(const T & bgn, const T & end) {
        checkIterating();
        bool exists = true;
        for (T n = bgn; n < end; ++n) {
            if (mySet.count(n) == 0) {
                exists = false;
            }
        }
        if (exists) {
            for (T n = bgn; n < end; ++n) {
                mySet.erase(n);
            }
        }

        bool existsInRange = myRange.remove(bgn, end);
        ENSURE(existsInRange == exists);
        return checkContent() && (existsInRange == exists);
    }

    void run() {
        
        for (int i=0; i< myTestSize;++i) {
            v.push_back(firstValue++);
        }

        ENSURE(insertTest(v[7],v[8]));
        ENSURE(insertTest(v[1],v[3]));
        ENSURE(insertTest(v[8],v[9]));
        ENSURE(insertTest(v[6],v[7]));
        ENSURE(insertTest(v[3],v[5]));
        
        ENSURE(insertTest(v[13],v[15]));
        ENSURE(insertTest(v[13],v[15]));
        ENSURE(insertTest(v[13],v[14]));
        ENSURE(insertTest(v[14],v[15]));
        ENSURE(insertTest(v[1],v[16]));
        
        ENSURE(insertTest(v[5],v[6]));
        ENSURE(insertTest(v[0],v[1]));
        ENSURE(insertTest(v[9],v[13]));
        ENSURE(insertTest(v[15],v[19]));
        
        ENSURE(removeTest(v[18],v[19]));
        ENSURE(removeTest(v[0],v[1]));
        ENSURE(removeTest(v[15],v[18]));
        ENSURE(removeTest(v[1],v[5]));
        ENSURE(removeTest(v[7],v[8]));
        ENSURE(removeTest(v[8],v[10]));
        ENSURE(removeTest(v[5],v[6]));
        ENSURE(removeTest(v[6],v[7]));
        ENSURE(removeTest(v[11],v[15]));
        ENSURE(removeTest(v[10],v[11]));
        
    }
};
template <class T>
class RangeSetPerfTest : public TemplateUnitTest {
public:
    RangeSetPerfTest(const char * theTemplateArgument, const T & theFirstValue) 
    : TemplateUnitTest("RangeSetUnitTest",theTemplateArgument), firstValue(theFirstValue) {  }
    
    enum {myTestSize = 20};

    set<T> mySet;
    RangeSet<T> myRange;
    std::vector<T> v;

    T firstValue;
    void run() {
        const int n = 10000;
        std::vector<long> myRandom;
        for (int i = 0; i< n; ++i){
            myRandom.push_back(i);
        }
        for (int i = 0; i< n; ++i) {
            std::swap(myRandom[abs(rand())%n], myRandom[abs(rand())%n]);    
        }

        for (int f = 0; f < 40; ++f)
        {
            {
                MAKE_SCOPE_TIMER(RangeSetPerfTest);

                START_TIMER(set_linear_forward_insert);
                for (int i = 0; i< n; ++i) {
                    mySet.insert(firstValue+i);
                }
                STOP_TIMER_N(set_linear_forward_insert,n);
                ENSURE(mySet.size() == n);

                START_TIMER(set_linear_forward_erase);
                for (int i = 0; i< n; ++i) {
                    mySet.erase(firstValue+i);
                }
                STOP_TIMER_N(set_linear_forward_erase,n);
                ENSURE(mySet.size() == 0);


                START_TIMER(set_linear_reverse_insert);
                for (int i = n-1; i >=0 ; --i) {
                    mySet.insert(firstValue+i);
                }
                STOP_TIMER_N(set_linear_reverse_insert,n);
                ENSURE(mySet.size() == n);

                START_TIMER(set_linear_reverse_erase);
                for (int i = n-1; i >=0 ; --i) {
                    mySet.erase(firstValue+i);
                }
                STOP_TIMER_N(set_linear_reverse_erase,n);
                ENSURE(mySet.size() == 0);


                START_TIMER(set_random_insert);
                for (int i = n-1; i >=0 ; --i) {
                    mySet.insert(firstValue+myRandom[i]);
                }
                STOP_TIMER_N(set_random_insert,n);
                ENSURE(mySet.size() == n);

                START_TIMER(set_random_erase);
                for (int i = n-1; i >=0 ; --i) {
                    mySet.erase(firstValue+myRandom[i]);
                }
                STOP_TIMER_N(set_random_erase,n);
                ENSURE(mySet.size() == 0);
// now RangeSet

                START_TIMER(range_linear_forward_insert);
                for (int i = 0; i< n; ++i) {
                    myRange.insert(firstValue+i);
                }
                STOP_TIMER_N(range_linear_forward_insert,n);
                ENSURE(myRange.size() == n);

                START_TIMER(range_linear_forward_remove);
                for (int i = 0; i< n; ++i) {
                    myRange.remove(firstValue+i);
                }
                STOP_TIMER_N(range_linear_forward_remove,n);
                ENSURE(myRange.size() == 0);


                START_TIMER(range_linear_reverse_insert);
                for (int i = n-1; i >=0 ; --i) {
                    myRange.insert(firstValue+i);
                }
                STOP_TIMER_N(range_linear_reverse_insert,n);
                ENSURE(myRange.size() == n);

                START_TIMER(range_linear_reverse_remove);
                for (int i = n-1; i >=0 ; --i) {
                    myRange.remove(firstValue+i);
                }
                STOP_TIMER_N(range_linear_reverse_remove,n);
                ENSURE(myRange.size() == 0);


                START_TIMER(range_random_insert);
                for (int i = n-1; i >=0 ; --i) {
                    myRange.insert(firstValue+myRandom[i]);
                }
                STOP_TIMER_N(range_random_insert,n);
                ENSURE(myRange.size() == n);

                START_TIMER(range_random_remove);
                for (int i = n-1; i >=0 ; --i) {
                    myRange.remove(firstValue+myRandom[i]);
                }
                STOP_TIMER_N(range_random_remove,n);
                ENSURE(myRange.size() == 0);

           }
            getDashboard().cycle();
        }
        getDashboard().print(std::cerr);
        getDashboard().reset();

    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new PtrUnitTest<0,SingleThreaded>);
        addTest(new PtrLinkTest<0,SingleThreaded>);
        addTest(new PtrUnitTest2<0>);

// the windows release build fails when running under ant
#ifndef WIN32
        addTest(new RangeSetPerfTest<int>("int", 0));
        addTest(new RangeSetUnitTest<int>("int", 0));
        addTest(new RangeSetUnitTest<CollectablePtrBase<SingleThreaded, CollectablePtrAllocator<SingleThreaded> >*>("CollectablePtrBase<SingleThreaded, CollectablePtrAllocator<SingleThreaded> >*", 0));
        PtrUnitTest<0,SingleProcessor> * mySingleProcessorTest = new PtrUnitTest<0,SingleProcessor>();
#ifdef SINGLE_CPU
        addTest(mySingleProcessorTest);
#else
        delete mySingleProcessorTest;
#endif        
        addTest(new PtrUnitTest<0,MultiProcessor>);

// the windows release build fails when running under ant
#ifndef WIN32
        // TODO: adapt all these nifty allocators to support the WeakPtr stuff
        addTest(new AllocatorUnitTest<0,SingleThreaded,CollectablePtrAllocator<SingleThreaded> >("SingleThreaded"));
        addTest(new AllocatorUnitTest<0,SingleProcessor,CollectablePtrAllocator<SingleProcessor> >("SingleProcessor"));
        addTest(new AllocatorUnitTest<0,MultiProcessor,CollectablePtrAllocator<MultiProcessor> >("MultiProcessor"));
        //addTest(new AllocatorUnitTest<0,SingleThreaded,PtrFreeListChunkAllocator<SingleThreaded> >("PtrFreeListChunkAllocator"));

        // addTest(new AllocatorUnitTest<0,MultiProcessor,PtrHeapAllocator<MultiProcessor> >("PtrHeapAllocator"));
        //addTest(new AllocatorUnitTest<0,MultiProcessor,MutexPtrFreeListAllocator>("MutexPtrFreeListAllocator"));
        //addTest(new AllocatorUnitTest<0,MultiProcessor,PtrThreadSpecificFreeListAllocator>("PtrThreadSpecificFreeListAllocator"));
        //addTest(new AllocatorUnitTest<0,MultiProcessor,SpinLockedPtrFreeListAllocator>("SpinLockedPtrFreeListAllocator"));
        //addTest(new AllocatorUnitTest<0,MultiProcessor,PtrThreadSpecificFreeListChunkAllocator>("PtrThreadSpecificFreeListChunkAllocator"));
        //addTest(new PtrMultiThreadTest(get_argc(), get_argv()));
#endif

        addTest(new PtrUnitPerfTest<0,SingleThreaded>);
        PtrUnitPerfTest<0,SingleProcessor> * mySingleProcessorPrefTest = new PtrUnitPerfTest<0,SingleProcessor>();
#ifdef SINGLE_CPU
        addTest(mySingleProcessorPrefTest);
#else
        delete mySingleProcessorPrefTest;
#endif        
        addTest(new PtrUnitPerfTest<0,MultiProcessor>);
#endif
    }
};

#ifdef OSX
}
#endif


int main(int argc, char *argv[]) {

    asl::initialize();

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
