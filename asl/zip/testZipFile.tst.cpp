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
#include <asl/base/MappedBlock.h>
#include <asl/base/UnitTest.h>
#include <asl/base/file_functions.h>

#include <string>
#include <iostream>

using namespace std; 
using namespace asl; 

class ZipReaderUnitTest : public UnitTest {
    public:
        ZipReaderUnitTest() : UnitTest("ZipReaderUnitTest") {  }
        void run() {
            const std::string myRootDir = asl::getDirectoryPart( __FILE__ );
            ZipReader myZipReader((myRootDir + "/testfiles/test.zip").c_str());
            ZipReader::Directory myDirectory = myZipReader.getDirectory();
            ENSURE(myDirectory.size() == 4);
            DPRINT(myDirectory[0].filename);
            ENSURE(myDirectory[0].filename.toUTF8() == "File One.txt");

            asl::Ptr<ReadableBlockHandle> myFile(myZipReader.getFile(myDirectory[0]));
            string myExpectedString = "Hello World One!";
            string myTestString;
            myFile->getBlock().readString(myTestString, myExpectedString.size(),0);
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
            asl::Ptr<ReadableBlockHandle> myBlockRead = myReader.getFile(myTestFileInZip);

            ENSURE(myBlockRead->getBlock() == myTestBlock);

        }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new ZipReaderUnitTest);
        addTest(new ZipWriterUnitTest);
    }

};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

