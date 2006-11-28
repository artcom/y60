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
//    $RCSfile: test_file_functions.tst.cpp,v $
//
//   $Revision: 1.6 $
//
// Description: unit test for file functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "file_functions.h"
#include "Time.h"

#include "UnitTest.h"
#include <iostream>

using namespace std;

using namespace asl;

class file_functions_UnitTest : public UnitTest {
    public:
        file_functions_UnitTest() : UnitTest("file_functions_UnitTest") {  }
        void run() {
            const string testFileName = "file_functions_UnitTest.testoutput";
            const string testContent = "blafasel";
            
            perform_putget(testFileName, testContent);
            
            perform_putget(testFileName, 0);
            perform_putget(testFileName, 1);
            perform_putget(testFileName, 65536-1);
            perform_putget(testFileName, 65536);
            perform_putget(testFileName, 65536+1);
            perform_putget(testFileName, 2*65536-1);
            perform_putget(testFileName, 2*65536);
            perform_putget(testFileName, 2*65536+1);
    
            perform_putget(testFileName, 20*65536-1);
            perform_putget(testFileName, 20*65536);
            perform_putget(testFileName, 20*65536+1);
            
            perform_filename_func();
            perform_uri_func();
         }
        
        void perform_uri_func() {
            ENSURE(getFilenamePart("http://www.artcom.de/foo/bar")=="bar");
            ENSURE(getFilenamePart("http://www.artcom.de/etc/")=="");
            ENSURE(getFilenamePart("http://www.artcom.de/passwd")=="passwd");

            ENSURE(getDirectoryPart("http://www.artcom.de/etc/passwd")=="/etc/");
            ENSURE(getDirectoryPart("http://www.artcom.de/etc/")=="/etc/");
            DPRINT(getDirectoryPart("http://www.artcom.de/passwd"));
            ENSURE(getDirectoryPart("http://www.artcom.de/passwd")=="/");
            
            ENSURE(getExtension("http://www.artcom.de/index.html")=="html");
            ENSURE(getExtension("http://root:pass:www.artcom.de/index.html")=="html");
            ENSURE(getExtension("http://www.artcom.de/index")=="");
            ENSURE(getExtension("http://www.artcom.de/index.")=="");

            ENSURE(getHostPortPart("http://root:path@www.artcom.de:80/foo/bar") == "www.artcom.de:80");
            ENSURE(getHostPortPart("http://root:path@www.artcom.de:80") == "www.artcom.de:80");

            DPRINT(getHostPart("http://root:path@www.artcom.de:80"));
            ENSURE(getHostPart("http://root:path@www.artcom.de:80") == "www.artcom.de");
            ENSURE(getHostPart("http://root:path@www.artcom.de:80/foo/bar") == "www.artcom.de");
            ENSURE(getHostPart("http://root:path@www.artcom.de:80/") == "www.artcom.de");

            DPRINT(getHostPart("/homes/martin/.visicore//undo/scene_0.b60"));
            ENSURE(getHostPart("/homes/martin/.visicore//undo/scene_0.b60") == "");
            ENSURE(getHostPart("foo.bar") == "");
        }

        void perform_filename_func() {
            ENSURE(getFilenamePart("/etc/passwd")=="passwd");
            ENSURE(getFilenamePart("/etc/")=="");
            ENSURE(getFilenamePart("passwd")=="passwd");
            ENSURE(getFilenamePart("C:/WinNT/sux")=="sux");

            ENSURE(getDirectoryPart("/etc/passwd")=="/etc/");
            ENSURE(getDirectoryPart("/homes/martin/.visicore//undo/scene_0.b60") == "/homes/martin/.visicore//undo/");
            DPRINT(getDirectoryPart("/etc//passwd"));
            ENSURE(getDirectoryPart("/etc//passwd")=="/etc/");
            ENSURE(getDirectoryPart("/etc/")=="/etc/");
            ENSURE(getDirectoryPart("/foo")=="/");
            ENSURE(getDirectoryPart("passwd")=="./");
            ENSURE(getDirectoryPart("C:/WinNT/sux")=="C:/WinNT/");


            ENSURE(getParentDirectory("C:/WinNT/sux/")=="C:/WinNT/");
            ENSURE(getParentDirectory("C:/WinNT/sux")=="C:/WinNT/");
            ENSURE(getParentDirectory("C:/WinNT")=="C:/");
            ENSURE(getParentDirectory("/")=="");
            ENSURE(getParentDirectory("")=="");
            ENSURE(getParentDirectory("X")=="");

            ENSURE(searchFile(getFilenamePart(__FILE__), "/;.;../..") == std::string("../../") + getFilenamePart(__FILE__));            
            ENSURE(searchFile(getFilenamePart(__FILE__), "") == "");            
            ENSURE(searchFile(getFilenamePart(__FILE__), "../..") == std::string("../../") + getFilenamePart(__FILE__));            
#ifdef LINUX
            ENSURE(searchFile(getFilenamePart(__FILE__), "/:.:../..") == std::string("../../") + getFilenamePart(__FILE__));
#endif            

            ENSURE(getExtension("myfile.mp3") == "mp3");
            ENSURE(getExtension("myfile.something.mp3") == "mp3");
            ENSURE(getExtension("somewhere\\sometime\\myfile.something.mp3") == "mp3");
            ENSURE(getExtension("somewhere.sometime/myfile.mp3") == "mp3");
            ENSURE(getExtension("somewhere.sometime\\myfile") == "");

            if (isDirectory("myTestSubDir")) {
                removeDirectory("myTestSubDir");
            }
            createDirectory("myTestSubDir");
            ENSURE(isDirectory("myTestSubDir"));

            createPath("myTestPath/myTestDir");
            ENSURE(isDirectory("myTestPath/myTestDir"));

            ENSURE(evaluateRelativePath("/usr/bin", "/bin") == "/bin");
            ENSURE(evaluateRelativePath("/usr/bin", "/usr/bin/laden") == "./laden");
            ENSURE(evaluateRelativePath("/usr/bin", "/usr/lib/bin") == "../lib/bin");
            ENSURE(evaluateRelativePath("/usr/bin", "/bin") == "/bin");
            ENSURE(evaluateRelativePath("/usr/bin", "/bin", true) == "../../bin");

            //FAILURE("test");
        }

        void perform_putget(const string & testFileName, int contentSize) {
            DPRINT2("Test with content of size", contentSize);
            string largeTestContent;
            largeTestContent.resize(contentSize);
            for (int i = 0; i< contentSize; ++i) {
                largeTestContent[i] = ' ' + i%63;
            }
            perform_putget(testFileName, largeTestContent); 
            perform_putget_binary(testFileName,contentSize);
        }
        void perform_putget(const string & testFileName, const string & testContent) {
            ENSURE(writeFile(testFileName,testContent));
            string fromFile;
            ENSURE(readFile(testFileName,fromFile));
            ENSURE(fromFile == testContent);   
            ENSURE(readFile(testFileName) == testContent);
            ENSURE(fileExists(testFileName));
            ENSURE(getFileSize(testFileName) == testContent.size());
        }
         void perform_putget_binary(const string & testFileName, int contentSize) {
            DPRINT2("Test with content of size", contentSize);
            Block largeTestContent;
            largeTestContent.resize(contentSize);
            for (int i = 0; i< contentSize; ++i) {
                largeTestContent[i] = ' ' + i%255;
            }
            perform_putget(testFileName, largeTestContent); 
        }
       void perform_putget(const string & testFileName, const asl::ResizeableBlock & testContent) {
            ENSURE(writeFile(testFileName,testContent));
            asl::Block fromFile;
            ENSURE(readFile(testFileName,fromFile));
            ENSURE(fromFile == testContent);   
            //ENSURE(readFile(testFileName) == testContent);
            ENSURE(fileExists(testFileName));
            ENSURE(getFileSize(testFileName) == testContent.size());
        }
};

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

        vector<string> myDirEntries = getDirectoryEntries(std::string("..") + theDirectorySeparator + ".." + theDirectorySeparator + "testdir");
        std::sort(myDirEntries.begin(), myDirEntries.end());

        ENSURE(myDirEntries.size() == 5);
//        ENSURE_MSG(myDirEntries[0] == ".svn "found dir .svn, ;-)");
        ENSURE_MSG(myDirEntries[1] == "a" , "found dir a");
        ENSURE_MSG(myDirEntries[2] == "b" , "found dir b");
        ENSURE_MSG(myDirEntries[3] == "c" , "found dir c");
        ENSURE_MSG(myDirEntries[4] == "d" , "found dir d");

        ENSURE_EXCEPTION(getDirectoryEntries("../../testdir/a"), OpenDirectoryFailed);
        ENSURE_EXCEPTION(getDirectoryEntries("nonexistingdir"), OpenDirectoryFailed);

        ENSURE(isDirectory("."));
        ENSURE(isDirectory("../../testdir/"));
        ENSURE(!isDirectory("nonexistingdir"));
        ENSURE(!isDirectory("../../testdir/a"));
//        ENSURE(isDirectory("../../testdir/.svn"));

        // last modified stuff
        std::string myFile = "../../dates.tst";
        writeFile(myFile, "foo");
        
        time_t myZeroTime = 0;
        tm myTimeStruct = *localtime(&myZeroTime);
        {
            myTimeStruct.tm_year = 100;
            myTimeStruct.tm_mon = 1;

            time_t myTime = mktime(&myTimeStruct);  
            setLastModified(myFile, myTime); 

            DPRINT(myTime);
            ENSURE_EQUAL(myTime, getLastModified(myFile));
        }
        {
            myTimeStruct.tm_year = 100;
            myTimeStruct.tm_mon = 7;

            time_t myTime = mktime(&myTimeStruct);  
            setLastModified(myFile, myTime); 

            DPRINT(myTime);
            ENSURE_EQUAL(myTime, getLastModified(myFile));
        }

        
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new file_functions_UnitTest);
        addTest(new DirectoryTest);
   }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
