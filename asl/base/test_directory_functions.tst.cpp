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
//    $RCSfile: test_directory_functions.tst.cpp,v $
//
//   $Revision: 1.9 $
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "UnitTest.h"
#include "directory_functions.h"
#include "settings.h"
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace asl;

class DirectoryTest : public UnitTest {
public:
    DirectoryTest() : UnitTest("DirectoryTest") {  }
    void run() {
        DIR * myDirHandle = opendir(".");
        struct dirent *dir_entry;

        ENSURE(myDirHandle);
        bool myFoundUpDirFlag = false;
        bool myFoundCurDirFlag = false;

        while((dir_entry = readdir(myDirHandle)) != NULL) {
            if (std::string("..") == dir_entry->d_name) {
                myFoundUpDirFlag = true;
            }
            if (std::string(".") == dir_entry->d_name) {
                myFoundCurDirFlag = true;
            }

            DPRINT(dir_entry->d_name);
        }
        ENSURE_MSG(myFoundUpDirFlag, " found up dir entry");
        ENSURE_MSG(myFoundCurDirFlag, " found current dir entry");
        closedir(myDirHandle);

        // use getdir utility function instead

        vector<string> myDirEntries = getDirList(std::string("..") + theDirectorySeparator + ".." + theDirectorySeparator + "testdir");
        std::sort(myDirEntries.begin(), myDirEntries.end());

        ENSURE(myDirEntries.size() == 5);
//        ENSURE_MSG(myDirEntries[0] == ".svn "found dir .svn, ;-)");
        ENSURE_MSG(myDirEntries[1] == "a" , "found dir a");
        ENSURE_MSG(myDirEntries[2] == "b" , "found dir b");
        ENSURE_MSG(myDirEntries[3] == "c" , "found dir c");
        ENSURE_MSG(myDirEntries[4] == "d" , "found dir d");

        ENSURE_EXCEPTION(getDirList("../../testdir/a"), DirectoryException);
        ENSURE_EXCEPTION(getDirList("nonexistingdir"), DirectoryException);

        std::cerr << getTempDirectory() << std::endl;
        std::cerr << getAppDataDirectory("TEST") << std::endl;

        ENSURE(isDirectory("."));
        ENSURE(!isDirectory("nonexistingdir"));
        ENSURE(!isDirectory("../../testdir/a"));
//        ENSURE(isDirectory("../../testdir/.svn"));
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new DirectoryTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
