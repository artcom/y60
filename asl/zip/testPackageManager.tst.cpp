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
#include <asl/base/UnitTest.h>
#include <asl/base/MappedBlock.h>
#include <asl/base/Ptr.h>

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
                asl::Ptr<ReadableBlockHandle> myFile = myPackageManager.readFile("not there");
                ENSURE(!myFile);
                string myFilename = myPackageManager.searchFile("not there");
                ENSURE(myFilename.empty());
            }
            {
                asl::Ptr<ReadableBlockHandle> myFile = myPackageManager.readFile("testfiles/test.zip");
                ENSURE(myFile);
                string myFilename = myPackageManager.searchFile("testfiles/test.zip");
                ENSURE(myFilename == "../../testfiles/test.zip");
            }
            {
                asl::Ptr<ReadableBlockHandle> myFile = myPackageManager.readFile("test.zip");
                ENSURE(myFile);
                string myFilename = myPackageManager.searchFile("test.zip");
                ENSURE(myFilename == "../../testfiles/test.zip");
            }
            {
                asl::Ptr<ReadableBlockHandle> myFile = myPackageManager.readFile("File One.txt");
                ENSURE(myFile);
                string myFilename = myPackageManager.searchFile("File One.txt");
                ENSURE(myFilename == "../../testfiles/test.zip/File One.txt");
            }
            {
                asl::Ptr<ReadableBlockHandle> myFile;
                myFile = myPackageManager.readFile("File One.txt","../../testfiles/test.zip");
                ENSURE(myFile);
                string myExpectedString = "Hello World One!";
                string myTestString;
                myFile->getBlock().readString(myTestString, myExpectedString.size(),0);
                DPRINT(myTestString);
                ENSURE(myExpectedString == myTestString);
            }
            {                
                asl::Ptr<ReadableBlockHandle> myFile = myPackageManager.readFile("File One.txt","test.zip");
                ENSURE( ! myFile);
            }
            { 
                myPackageManager.add(IPackagePtr(new ZipPackage("../../testfiles/test2.zip")));
                IPackage::FileList myEntries;

                myEntries = myPackageManager.findFiles(); //list all files
                for (unsigned i = 0;i < myEntries.size(); ++i) {
                    cerr << "entry " << myEntries[i] << endl;
                    cerr << "path " << myPackageManager.searchFile(myEntries[i]) << endl;
                }
                
                //list all files in relative dir. testfiles/
                myEntries = myPackageManager.findFiles("testfiles");                 
                for(IPackage::FileList::size_type i = 0;i < myEntries.size(); ++i) {
                    cerr << "listing '" << myEntries[i] << "'" << endl;
                }
                //list all files in package ../../testfiles
                // *this is a different thing than the above*
                myEntries = myPackageManager.findFiles("", "../../testfiles"); 
                ENSURE(myEntries.size() == 2);
                for (unsigned i = 0;i < myEntries.size(); ++i) {
                    cerr << "listing '" << myEntries[i] << "'" << endl;
                }
                //list all files in package ../../testfiles/test.zip
                myEntries = myPackageManager.findFiles("","../../testfiles/test.zip"); 
                for(IPackage::FileList::size_type i = 0;i < myEntries.size(); ++i) {
                    cerr << "listing '" << myEntries[i] << "'" << endl;
                }
                
                myEntries = myPackageManager.findFiles("d"); //d is dir in test2.zip
                ENSURE(myEntries.size() == 3);

                myPackageManager.add("../../testfiles/testdir");
                myEntries = myPackageManager.findFiles("subdir1", "../../testfiles/testdir");
                ENSURE(myEntries.size() == 1);
            }  
            ENSURE(!myPackageManager.searchFile("testfiles/test.zip").empty())
            ENSURE(myPackageManager.remove("../.."));
            ENSURE(myPackageManager.remove("../../testfiles"));
            ENSURE(myPackageManager.remove("../../testfiles/test.zip"));
            ENSURE(myPackageManager.remove(""));
            ENSURE(myPackageManager.searchFile("testfiles/test.zip").empty());
        }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new PackageManagerUnitTest);
    }

};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}
