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
//    $RCSfile: testZipReader.tst.cpp,v $
//
//   $Revision: 1.2 $
//
// Description: unit test template file - change ZipReader to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "ZipReader.h"
#include "ZipWriter.h"
#include <asl/MappedBlock.h>
#include <asl/UnitTest.h>

#include <string>
#include <iostream>

using namespace std; 
using namespace asl; 

class ZipReaderUnitTest : public UnitTest {
    public:
        ZipReaderUnitTest() : UnitTest("ZipReaderUnitTest") {  }
        void run() {
            ZipReader myZipReader("../../testfiles/test.zip");
            ZipReader::Directory myDirectory = myZipReader.getDirectory();
            ENSURE(myDirectory.size() == 4);
            DPRINT(myDirectory[0].filename);
            ENSURE(myDirectory[0].filename == "File One.txt");

            Ptr<ReadableBlock> myFile(myZipReader.getFile(myDirectory[0]));
            string myExpectedString = "Hello World One!";
            string myTestString;
            myFile->readString(myTestString, myExpectedString.size(),0);
            DPRINT(myTestString);
            ENSURE(myExpectedString == myTestString);

        }
};

class ZipWriterUnitTest : public UnitTest {
    public:
        ZipWriterUnitTest() : UnitTest("ZipWriterUnitTest") {  }
        void run() {
            const char * myTestZip = "testdata.zip.testoutput";
            const char * myTestFileInZip = "subdir/foo.txt";
            string myData("my test data string");
            Block myTestBlock;
            myTestBlock.append(myData.c_str(), myData.size());
             
            ZipWriter myZipWriter(myTestZip);
            myZipWriter.append(myTestBlock, myTestFileInZip);
            myZipWriter.close();
            
            ZipReader myReader(myTestZip);
            Ptr<ReadableBlock> myBlockRead = myReader.getFile(myTestFileInZip);

            ENSURE(*myBlockRead == myTestBlock);

        }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new ZipReaderUnitTest);
        addTest(new ZipWriterUnitTest);
    }

};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

