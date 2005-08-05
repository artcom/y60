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
//    $RCSfile: testPackageManager.tst.cpp,v $
//
//   $Revision: 1.4 $
//
// Description: unit test template file - change PackageManager to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "PackageManager.h"
#include "DirectoryPackage.h"
#include "ZipPackage.h"
#include <asl/UnitTest.h>
#include <asl/MappedBlock.h>

#include <string>
#include <iostream>

using namespace std; 
using namespace asl; 

class PackageManagerUnitTest : public UnitTest {
    public:
        PackageManagerUnitTest() : UnitTest("PackageManagerUnitTest") {  }
        void run() {
            PackageManager myPackageManager;

            myPackageManager.add(IPackagePtr(new DirectoryPackage("../../")));
            myPackageManager.add(IPackagePtr(new DirectoryPackage("../../testfiles")));
            myPackageManager.add(IPackagePtr(new ZipPackage("../../testfiles/test.zip")));
            myPackageManager.add(IPackagePtr(new DirectoryPackage("")));

#ifdef WIN32
            ENSURE(myPackageManager.searchFile("testPackageManager.exe") == "testPackageManager.exe");
#else
            ENSURE(myPackageManager.searchFile("testPackageManager") == "testPackageManager");
#endif

            {
                Ptr<ReadableBlock> myFile = myPackageManager.openFile("not there");
                ENSURE(!myFile);
                string myFilename = myPackageManager.searchFile("not there");
                ENSURE(myFilename.empty());
            }
            {
                Ptr<ReadableBlock> myFile = myPackageManager.openFile("testfiles/test.zip");
                ENSURE(myFile);
                string myFilename = myPackageManager.searchFile("testfiles/test.zip");
                ENSURE(myFilename == "../../testfiles/test.zip");
            }
            {
                Ptr<ReadableBlock> myFile = myPackageManager.openFile("test.zip");
                ENSURE(myFile);
                string myFilename = myPackageManager.searchFile("test.zip");
                ENSURE(myFilename == "../../testfiles/test.zip");
            }
            {
                Ptr<ReadableBlock> myFile = myPackageManager.openFile("File One.txt");
                ENSURE(myFile);
                string myFilename = myPackageManager.searchFile("File One.txt");
                ENSURE(myFilename == "../../testfiles/test.zip/File One.txt");
            }
            {
                Ptr<ReadableBlock> myFile;
                myFile = myPackageManager.openFile("File One.txt","../../testfiles/test.zip");
                ENSURE(myFile);
                string myExpectedString = "Hello World One!";
                string myTestString;
                myFile->readString(myTestString, myExpectedString.size(),0);
                DPRINT(myTestString);
                ENSURE(myExpectedString == myTestString);
            }
            {                
                Ptr<ReadableBlock> myFile = myPackageManager.openFile("File One.txt","test.zip");
                ENSURE( ! myFile);
            }
            { 
                myPackageManager.add(IPackagePtr(new ZipPackage("../../testfiles/test2.zip")));
                IPackage::FileList myEntries;

                myEntries = myPackageManager.listFiles(); //list all files
                for (unsigned i = 0;i < myEntries.size(); ++i) {
                    cerr << "entry " << myEntries[i] << endl;
                    cerr << "path " << myPackageManager.searchFile(myEntries[i]) << endl;
                }
                
                //list all files in relative dir. testfiles/
                myEntries = myPackageManager.listFiles("testfiles");                 
                for(int i = 0;i < myEntries.size(); ++i) {
                    cerr << "listing '" << myEntries[i] << "'" << endl;
                }
                //list all files in package ../../testfiles
                // *this is a different thing than the above*
                myEntries = myPackageManager.listFiles("", "../../testfiles"); 
                ENSURE(myEntries.size() == 2);
                for (unsigned i = 0;i < myEntries.size(); ++i) {
                    cerr << "listing '" << myEntries[i] << "'" << endl;
                }
                //list all files in package ../../testfiles/test.zip
                myEntries = myPackageManager.listFiles("","../../testfiles/test.zip"); 
                for(int i = 0;i < myEntries.size(); ++i) {
                    cerr << "listing '" << myEntries[i] << "'" << endl;
                }
                
                myEntries = myPackageManager.listFiles("d"); //d is dir in test2.zip
                ENSURE(myEntries.size() == 3);

                myPackageManager.add("../../testfiles/testdir");
                myEntries = myPackageManager.listFiles("subdir1", "../../testfiles/testdir");
                ENSURE(myEntries.size() == 1);
            }   
        }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new PackageManagerUnitTest);
    }

};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}
