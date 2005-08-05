/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testZipFile.tst.cpp,v $
//
//   $Revision: 1.2 $
//
// Description: unit test template file - change ZipFile to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "ZipFile.h"
#include <asl/MappedBlock.h>
#include <asl/UnitTest.h>

#include <string>
#include <iostream>

using namespace std; 
using namespace asl; 

class ZipFileUnitTest : public UnitTest {
    public:
        ZipFileUnitTest() : UnitTest("ZipFileUnitTest") {  }
        void run() {
            ZipFile myZipFile("../../testfiles/test.zip");
//            ConstMappedBlock myZippedBlock("../../testfiles/ALongFileName.zip");
//            ZipFile myZipFile(myZippedBlock);
            ZipFile::Directory myDirectory = myZipFile.getDirectory();
            ENSURE(myDirectory.size() == 4);
            DPRINT(myDirectory[0].filename);
            ENSURE(myDirectory[0].filename == "File One.txt");

            Ptr<ReadableBlock> myFile(myZipFile.getFile(myDirectory[0]));
            string myExpectedString = "Hello World One!";
            string myTestString;
            myFile->readString(myTestString, myExpectedString.size(),0);
            DPRINT(myTestString);
            ENSURE(myExpectedString == myTestString);

        }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new ZipFileUnitTest);
    }

};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

