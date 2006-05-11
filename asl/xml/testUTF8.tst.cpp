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
//    $RCSfile: testAuto.tst.cpp,v $
//
//   $Revision: 1.1.1.1 $
//
// Description: unit test template file - change Auto to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Nodes.h"
#include <asl/UnitTest.h>
#ifdef OSX
#else
#include <glib.h>
#endif
#include <sstream>

using namespace std; 
using namespace asl; 
using namespace dom; 

class XMLUTF_8Test : public UnitTest {
    public:
        XMLUTF_8Test() : UnitTest("XMLUTF_8Test") {  }
        void run() {
            Document myDoc;
            myDoc.parseFile("../../testdata/UTF8.xml");
            {
                string myString = myDoc.childNode("utf8-test")->getAttributeString("auml");
                ENSURE(myString.size() == 2); // 2 bytes
                ENSURE(myString[0] == char(0xC3)); 
                ENSURE(myString[1] == char(0xA4)); 
                //ENSURE(g_utf8_strlen(myString.c_str(), -1) == 1); // one utf8 char
            }

            ostringstream myBuffer;
            myBuffer << myDoc;
            DPRINT(myBuffer.str());
            Document myOtherDoc(myBuffer.str());
            {
                string myString = myOtherDoc.childNode("utf8-test")->getAttributeString("auml");
                ENSURE(myString.size() == 2); // 2 bytes
                ENSURE(myString[0] == char(0xC3)); 
                ENSURE(myString[1] == char(0xA4)); 
                //ENSURE(g_utf8_strlen(myString.c_str(), -1) == 1); // one utf8 char
            }
            
        }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new XMLUTF_8Test());
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

