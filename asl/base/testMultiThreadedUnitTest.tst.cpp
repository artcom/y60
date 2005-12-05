/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testMultiThreadedUnitTest.tst.cpp,v $
//
//   $Revision: 1.2 $
//
// Description: unit test for MultiThreadUnitTest classes 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#include "UnitTest.h"
#include "string_functions.h"

#include "MultiThreadedUnitTest.h"
#ifdef LINUX
#include <unistd.h>
#endif
#include <string>
#include <iostream>
#include <vector>


using namespace std;  // automatically added!



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

template <int N>
class DummyUnitTest2 : public TemplateUnitTest {
public:
    DummyUnitTest2() : TemplateUnitTest("DummyUnitTest2:",asl::as_string(N).c_str()) {  }
    void run() {
        ENSURE(TestClassBase<N>::_theInstanceCount_ == 0);
        std::vector<TestClassDerived<N> *> myVec;
        for (int i = 0; i < 1000; ++i) {
            myVec.push_back(new TestClassDerived<N>);
        }
        ENSURE(TestClassBase<N>::_theInstanceCount_ == 1000);
        
        std::vector<TestClassDerived<N> * > otherBaseVec;
        otherBaseVec = myVec;

        std::vector<TestClassBase<N> * > myBaseVec(1000);

        std::copy(myVec.begin(),myVec.end(),myBaseVec.begin());
        ENSURE(equal(myBaseVec.begin(), myBaseVec.end(), myVec.begin()));

        ENSURE(TestClassBase<N>::_theInstanceCount_ == 1000);
        for (int i=0;i<1000;++i) {
            delete myVec[i];
        }

        myBaseVec.resize(0);
        otherBaseVec.resize(0);
        myVec.resize(0);
        
        ENSURE(TestClassBase<N> ::_theInstanceCount_ == 0);
        
    }
};
template <int N>
class DummyUnitTest : public TemplateUnitTest {
public:
    DummyUnitTest() : TemplateUnitTest("DummyUnitTest-",asl::as_string(N).c_str()) {  }
    void run() {

        {
            // Test empty pointer behavior
            ENSURE(TestClassBase<N>::_theInstanceCount_ == 0);
            TestClassBase<N> * emptyBaseDummy = 0;
            ENSURE(!emptyBaseDummy);

            // Test single non-empty pointer behaviour 
            TestClassBase<N> * myBaseDummy(new TestClassBase<N>);
            ENSURE(myBaseDummy);
            ENSURE(TestClassBase<N>::_theInstanceCount_ == 1);
            ENSURE(myBaseDummy != 0);

            ENSURE(myBaseDummy != emptyBaseDummy);

            ENSURE(strcmp("TestClassBase",myBaseDummy->getName()) == 0);
            ENSURE(strcmp("TestClassBase",(*myBaseDummy).getName()) == 0);

            // Test assignment behaviour
            {
                TestClassBase<N> * anotherDummy = myBaseDummy;
                ENSURE(anotherDummy);
                ENSURE(TestClassBase<N>::_theInstanceCount_ == 1);
                ENSURE(anotherDummy == myBaseDummy);
                ENSURE(strcmp("TestClassBase",anotherDummy->getName()) == 0);
                ENSURE(strcmp("TestClassBase",(*anotherDummy).getName()) == 0);
            }        

            // Test polymorph behaviour
            TestClassDerived<N> * derivedDummy(new TestClassDerived<N>);
            ENSURE(derivedDummy);
            ENSURE(TestClassBase<N>::_theInstanceCount_ == 2);
            ENSURE(strcmp("TestClassDerived",(*derivedDummy).getName()) == 0);

            TestClassBase<N> * anotherBase = derivedDummy;
            ENSURE(anotherBase);
            ENSURE(anotherBase == derivedDummy);

            TestClassDerived<N> * anotherDerived = 0;
            ENSURE(!anotherDerived);
            anotherDerived = dynamic_cast<TestClassDerived<N>*>(anotherBase);
            ENSURE(anotherDerived);
            delete derivedDummy;
            delete myBaseDummy;
        }
        ENSURE(TestClassBase<N>::_theInstanceCount_ == 0);
    }
};

class MyDummyMultiThreadTest : public MultiThreadedTestSuite  {
public:
    MyDummyMultiThreadTest(int argc, char *argv[]) : MultiThreadedTestSuite("PtrMultiThreadTest", argc, argv) { 
        cerr << "Created PtrMultiThreadTest()" << endl;
    }

    void setup() {
        addTest(new ThreadedTemplateUnitTest< Repeat<1000,DummyUnitTest<0> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<1000,DummyUnitTest<1> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<1000,DummyUnitTest2<2> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<1000,DummyUnitTest2<3> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<1000,DummyUnitTest2<4> > > );
    }
};


class MyTestSuite : public CatchingUnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : CatchingUnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new DummyUnitTest<0>,100);  // make sure it works single threaded
        addTest(new DummyUnitTest2<0>,100); // make sure it works single threaded
        addTest(new MyDummyMultiThreadTest(get_argc(), get_argv()),3); // now run as five threads
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
