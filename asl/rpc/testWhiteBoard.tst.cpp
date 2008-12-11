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
//    $RCSfile: testWhiteBoard.tst.cpp,v $
//
//   $Revision: 1.1.1.1 $
//
// Description: unit test for MultiThreadUnitTest classes 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "WhiteBoard.h"
#include <asl/net/Station.h>

#include <asl/base/UnitTest.h>
#include <asl/base/string_functions.h>
#include <asl/Xml.h>
#include <asl/XmlTypes.h>

#include <asl/base/MultiThreadedUnitTest.h>
#include <asl/base/Time.h>

#include <unistd.h>

#include <string>
#include <iostream>
#include <vector>


using namespace std;  // automatically added!



using namespace asl;

// undefine to enable manual testing with remote partners
#define NO_REMOTE_TESTING

StationWhiteBoard ourBoard;
//WhiteBoard ourBoard;
//BufferedWhiteBoard ourBoard;

template <int N>
class WhiteBoardPrivateEntryTest : public TemplateUnitTest {
public:
    WhiteBoardPrivateEntryTest() : TemplateUnitTest("WhiteBoardPrivateEntryTest-",asl::as_string(N).c_str()) {  }
    void run() {

        try {
            const string myNID = string("entry-")+asl::as_string(N)+"-";
            const string mySetter = string("setter-")+asl::as_string(N);

            xml::Node myTestNode = *asNode(string("stringvalue"));
            xml::Node otherTestNode = *asNode(int(23));

            const int numSheets = 10;

            // create some empty sheets
            for (int n = 0; n < numSheets; ++n) {
                const string myId = myNID + asl::as_string(n);
                ourBoard.createSheet(myId,myNID);
            }
            ourBoard.flush();
            SUCCESS("Created empty sheets");

            // get them
            for (int n = 0; n < numSheets; ++n) {
                const string myId = myNID + asl::as_string(n);
                xml::Node myResult;
                ourBoard.getSheet(myId,myNID,myResult);
                ENSURE(myResult);
                ENSURE(!myResult.NumberOfChildren());
            }
            SUCCESS("Got empty sheets");

            // re-create them with some data 
            for (int n = 0; n < numSheets; ++n) {
                const string myId = myNID + asl::as_string(n);
                ourBoard.createSheet(myId,myNID,myTestNode.Clone());
            }
            ourBoard.flush();
            SUCCESS("Created non-empty sheets");

            // get them
            for (int n = 0; n < numSheets; ++n) {
                const string myId = myNID + asl::as_string(n);
                xml::Node myResult;
                ourBoard.getSheet(myId,myNID,myResult);
                //DPRINT(myResult);
                ENSURE(myResult.NumberOfChildren() == 1);
                string myString;
                ENSURE(fromNode(*myResult.GetChild(0),myString));
                ENSURE(myString == "stringvalue");
            }
            SUCCESS("Verified non-empty sheets");

            // set them to another value
            for (int n = 0; n < numSheets; ++n) {
                const string myId = myNID + asl::as_string(n);
                ourBoard.setSheet(myId,myNID,otherTestNode);
            }
            ourBoard.flush();
            SUCCESS("Changed sheets");

            // check the other value
            for (int n = 0; n < numSheets; ++n) {
                const string myId = myNID + asl::as_string(n);
                xml::Node myResult;
                ourBoard.getSheet(myId,myNID,myResult);
                ENSURE(myResult.NumberOfChildren() == 1);
                int myInt;
                ENSURE(fromNode(*myResult.GetChild(0),myInt));
                ENSURE(myInt == 23);
                ENSURE(myResult == otherTestNode);
            }
            SUCCESS("Verified Change");

            // destroy them
            for (int n = 0; n < numSheets; ++n) {
                const string myId = myNID + asl::as_string(n);
                ourBoard.destroySheet(myId,myNID);
            }
            SUCCESS("Destroyed Sheets");

            ourBoard.flush();
            SUCCESS("Passed without Exceptions");
            //DPRINT2("Passed one WhiteBoardPrivateEntryTest", N);
            cerr << N;
        } catch (const asl::Exception & e) {
            cerr << "asl exception: " << e << endl;
            FAILURE("");
        }
    }
};

template <int N>
class WhiteBoardSharedEntryTest : public TemplateUnitTest {
public:
    WhiteBoardSharedEntryTest() : TemplateUnitTest("WhiteBoardSharedEntryTest-",asl::as_string(N).c_str()) {  }
    void run() {

        const string myGetter = string("getter-")+asl::as_string(N)+"-";
        const string mySetter = string("shared-setter");
          
        xml::Node myTestNode = *asNode(string("shared-stringvalue"));
        xml::Node otherTestNode = *asNode(int(24));
       
        const int numSheets = 10;

        // re-create sheets with some data 
        for (int n = 0; n < numSheets; ++n) {
            const string myId = asl::as_string(n);
            ourBoard.createSheet(myId,mySetter,myTestNode.Clone());
        }
        ourBoard.flush();
        SUCCESS("Created non-empty sheets");

        // get them
        for (int n = 0; n < numSheets; ++n) {
            const string myId = asl::as_string(n);
            xml::Node myResult;
            ourBoard.getSheet(myId,mySetter,myResult);
            ourBoard.flush();
            //DPRINT(myResult);
            ENSURE(myResult.NumberOfChildren() == 1);
            string myString;
            int myInt;
            if (fromNode(*myResult.GetChild(0),myInt)) {
                ENSURE(myInt == 24);
            } else  if (fromNode(*myResult.GetChild(0),myString)) {
                ENSURE(myString == "shared-stringvalue");
            } else {
                DPRINT(myResult);
                FAILURE("bad value returned from sheet");
            }
        }
        SUCCESS("Verified non-empty sheets");

        // set them to another value
        for (int n = 0; n < numSheets; ++n) {
            const string myId = asl::as_string(n);
            ourBoard.setSheet(myId,mySetter,otherTestNode);
        }
        ourBoard.flush();
        SUCCESS("Changed sheets");
        //DPRINT2("Passed one WhiteBoardSharedEntryTest", N);
        cerr  << N;
    }
};

const int maxValue = 9;
const int numCounters = 32;

template <int N>
class SheetTestWriter : public TemplateUnitTest {
public:
    SheetTestWriter() : TemplateUnitTest("SheetTestWriter-",asl::as_string(N).c_str()) {  }
    void run() {

        const string mySetter = string("setter-")+asl::as_string(N);
        WhiteBoard & myBoard = ourBoard;

        const string & myCounterBaseName = "myCounter";

        vector<Ptr<NewSheet<int> > > _mySheets;
        for (int value = 0; value <= maxValue; ++value) {
            for (int i = 0; i < numCounters; ++i) {
                Ptr<NewSheet<int> > myCounter(new NewSheet<int>(myBoard,myCounterBaseName + asl::as_string(i)));
                myCounter->setValue(value);
                myCounter->writeToBoard();
                _mySheets.push_back(myCounter);
            }
            myBoard.flush();
        }
        SUCCESS("Cycled Values");
        cerr  << "W";
    }
};

template <int N>
class SheetTestReader : public TemplateUnitTest {
public:
    SheetTestReader() : TemplateUnitTest("SheetTestReader-",asl::as_string(N).c_str()) {  }
    void run() {

        WhiteBoard & myBoard = ourBoard;
        const string & myCounterBaseName = "myCounter";

        for (int value = 0; value <= maxValue; ++value) {
            for (int i = 0; i < numCounters; ++i) {
                ConstSheet<int> myCounter(myBoard,myCounterBaseName + asl::as_string(i));
                myCounter.readFromBoard();
                myBoard.flush();
                int retry = 10;
                do {
                    myCounter.readFromBoard();
                    myBoard.flush();
                } while (--retry && !myCounter.hasNewValue());
                if (!retry) {
                    cerr << 'e'+N;
                }
                int x = myCounter.getValue();
            }
        }
        SUCCESS("Read all Values");
        cerr << N;
    }
};

template <int N>
class SheetLister : public TemplateUnitTest {
public:
    SheetLister() : TemplateUnitTest("SheetLister-",asl::as_string(N).c_str()) {  }
    void run() {
        ourBoard.requestSheetIds();
        vector<string> mySheetIds;
        ourBoard.requestSheetIds();
        ourBoard.flush();
        ourBoard.getSheetIds(mySheetIds);
        int totalLen = 0;
        for (int i = 0; i < mySheetIds.size(); ++i) {
            totalLen += mySheetIds[i].size();

            if (N != 0) { 
                cerr << "'" << mySheetIds[i] <<"'" << endl;
                xml::Node myResult;
                ourBoard.getSheet(mySheetIds[i],"lister",myResult);
                ourBoard.flush();
                //cerr << myResult << endl; 
            }
        }
        ourBoard.flush();

        SUCCESS("Requested SheetsIds");
        usleep(200000);
        cerr << "L";
    }
};

template <int N>
class SheetCopier : public TemplateUnitTest {
public:
    SheetCopier() : TemplateUnitTest("SheetCopier-",asl::as_string(N).c_str()) {  }
    void run() {

        WhiteBoard myBoard;
        asl::Time start;
        ourBoard.copyInto(myBoard);
        asl::Time ready;
        
        ourBoard.requestSheetIds();
        vector<string> mySheetIds;
        ourBoard.requestSheetIds();
        ourBoard.flush();
        ourBoard.getSheetIds(mySheetIds);
        
        cerr << "SheetCopier: copying " << mySheetIds.size()
             << " Sheets took " << ready - start << "secs." << endl;

        for (int i = 0; i < mySheetIds.size(); ++i) {
            xml::Node myResult;
            ourBoard.getSheet(mySheetIds[i],"lister",myResult);
            ourBoard.flush();
            xml::Node otherResult;
            myBoard.getSheet(mySheetIds[i],"lister",otherResult);
            myBoard.flush();
         }
        ourBoard.flush();

        SUCCESS("Copied Sheet");
        usleep(200000);
        cerr << "L";
    }
};

template <int N>
class ModifiedSheetCopier : public TemplateUnitTest {
public:
    ModifiedSheetCopier() : TemplateUnitTest("SheetCopier-",asl::as_string(N).c_str()) {  }
    void run() {
        WhiteBoard myBoard;
        
        ourBoard.copyInto(myBoard,&IWhiteBoard::copyModifiedFilter);
        
        usleep(200000);

        cerr << "M";
    }
};

template <int N>
class SheetConversionTest : public TemplateUnitTest {
public:
    SheetConversionTest() : TemplateUnitTest("SheetConversionTest-",asl::as_string(N).c_str()) {  }
    void run() {
        DPRINT(ourBoard);
        xml::NodePtr myXmlBoard = asNode(ourBoard);

        string myBoardString = asl::as_string(*myXmlBoard);
        DPRINT(myBoardString);
        DPRINT(myBoardString.size());
        
        WhiteBoard myOtherBoard;
        ENSURE(fromNode(*myXmlBoard,myOtherBoard));
        xml::NodePtr myOtherXmlBoard = asNode(myOtherBoard);
        string myOtherBoardString = asl::as_string(*myOtherXmlBoard);
        DPRINT(myOtherBoard);
        cerr << "S";
    }
};

class SheetUpdater : public TemplateUnitTest {
    public:
        SheetUpdater() : TemplateUnitTest("SheetUpdater-",asl::as_string(0).c_str()) {  }
        void run() {

            try {
                asl::Time begin;
                while (asl::Time() - begin < 10.0) {
                    asl::Time upstart;
                    ourBoard.update();
                    asl::Time upready;
                    cerr << "Sheetupdater: updating took " << upready - upstart << " secs." << endl;
                    asl::Time start;
                    ourBoard.broadcast();
                    asl::Time ready;
                    cerr << "Sheetupdater: broadcasting took " << ready - start << " secs." << endl;
                    //usleep(30000);
                }
            }
            catch (asl::Exception & ex) {
                cerr << "SheetUpdater: " << ex << endl;
                throw;
            }

        }
};




class PrivateEntriesMultiThreadTest : public MultiThreadedTestSuite  {
public:
    PrivateEntriesMultiThreadTest() : MultiThreadedTestSuite("PrivateEntriesMultiThreadTest") { 
        cerr << "Created PrivateEntriesMultiThreadTest()" << endl;
    }

    void setup() {
        addTest(new ThreadedTemplateUnitTest< Repeat<400,WhiteBoardPrivateEntryTest<0> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<300,WhiteBoardPrivateEntryTest<1> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<200,WhiteBoardPrivateEntryTest<2> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<100,WhiteBoardPrivateEntryTest<3> > > );
    }
};

class SharedEntriesMultiThreadTest : public MultiThreadedTestSuite  {
public:
    SharedEntriesMultiThreadTest() : MultiThreadedTestSuite("SharedEntriesMultiThreadTest") { 
        cerr << "Created SharedEntriesMultiThreadTest()" << endl;
    }

    void setup() {
        addTest(new ThreadedTemplateUnitTest< Repeat<4000,WhiteBoardSharedEntryTest<0> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<3000,WhiteBoardSharedEntryTest<1> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<2000,WhiteBoardSharedEntryTest<2> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<1000,WhiteBoardSharedEntryTest<3> > > );
    }
};

class SharedSheetMultiThreadTest : public MultiThreadedTestSuite  {
public:
    SharedSheetMultiThreadTest() : MultiThreadedTestSuite("SharedSheetMultiThreadTest") { 
        cerr << "Created SharedSheetMultiThreadTest()" << endl;
    }

    void setup() {
        addTest(new ThreadedTemplateUnitTest< Repeat<40,SheetLister<0> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<30,SheetTestWriter<0> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<30,SheetTestReader<0> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<20,SheetTestReader<1> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<10,SheetTestReader<2> > > );
#ifdef NO_REMOTE_TESTING
        addTest(new ThreadedTemplateUnitTest< Repeat<40,ModifiedSheetCopier<6> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<10,SheetCopier<4> > > );
#endif
        addTest(new ThreadedTemplateUnitTest< Repeat<3,SheetConversionTest<5> > > );
        addTest(new ThreadedTemplateUnitTest< Repeat<3,SheetUpdater> > );
    }

};

Station ourStation;

class MyTestSuite : public CatchingUnitTestSuite {
public:
    MyTestSuite(const char * myName) : CatchingUnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message

        addTest(new WhiteBoardPrivateEntryTest<0>);  // make sure it works single threaded
        addTest(new WhiteBoardSharedEntryTest<0>);  // make sure it works single threaded
        addTest(new SheetTestWriter<0>); 
        addTest(new SheetLister<1>);
        addTest(new SheetTestReader<0>);

#ifdef NO_REMOTE_TESTING
        addTest(new SheetCopier<4>);
        addTest(new ModifiedSheetCopier<4>);
#endif

        addTest(new SheetConversionTest<5>);
        addTest(new SheetUpdater);
        
        addTest(new SharedSheetMultiThreadTest); // now run as five threads
#ifdef NO_REMOTE_TESTING
        addTest(new PrivateEntriesMultiThreadTest); // now run as five threads
        addTest(new SharedEntriesMultiThreadTest); // now run as five threads
#endif
    ourStation.openStationDefault(
                Station::defaultBroadcastAddress(),
                Station::defaultBroadcastPort() - 1,
                Station::defaultBroadcastPort() - 1,
#ifdef NO_REMOTE_TESTING
                0x0);
#else
                Station::defaultIgnoreAddress());
#endif
        ourBoard.bind(&ourStation);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();
    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
