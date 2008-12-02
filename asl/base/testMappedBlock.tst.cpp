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
//    $RCSfile: testMappedBlock.tst.cpp,v $
//
//   $Revision: 1.7 $
//
// Description: unit test for file functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "file_functions.h"
#include "MappedBlock.h"
#include "Time.h"

#include "UnitTest.h"
#include <iostream>
#include <fstream>

#define DB2(x) // x

using namespace std;
using namespace asl;

class MappedBlockUnitTest : public UnitTest {
public:
	MappedBlockUnitTest() : UnitTest("MappedBlockUnitTest") {  }
	void run() {
		const string testFileName = "MappedBlockUnitTest.testoutput";

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
		perform_putget(testFileName, 0);
	}  


	void perform_putget(const string & testFileName, int contentSize) {
		DPRINT2("Test with content of size", contentSize);
		Block largeTestContent;
		largeTestContent.resize(contentSize);
		for (int i = 0; i< contentSize; ++i) {
			largeTestContent[i] = ' ' + i%(255-' ');
		}
		perform_putget(testFileName, largeTestContent); 
	}
	void perform_putget(const string & testFileName, const asl::ReadableBlock & testContent) {
		ENSURE(writeFile(testFileName,testContent));
		asl::Block fromFile;
		ENSURE(readFile(testFileName,fromFile));
		ENSURE(fromFile == testContent);  
		ENSURE(fileExists(testFileName));
        ENSURE( static_cast<asl::AC_SIZE_TYPE>(getFileSize(testFileName)) == testContent.size());
        
        {
            ConstMappedBlock myBlock(testFileName);
		    ENSURE(fromFile == myBlock);  
        }
        {
		    ConstMappedBlock mySizedBlock(testFileName,fromFile.size());
		    ENSURE(fromFile == mySizedBlock); 
        }

		string otherTestFileName = std::string("WRB1-") + testFileName;
		asl::Block otherContent;
		{
			ENSURE(writeFile(otherTestFileName,testContent));
			WriteableMappedBlock myWriteableBlock(otherTestFileName);
			ENSURE(testContent == myWriteableBlock);
			otherContent.resize(testContent.size());
            for (asl::ReadableBlock::size_type i = 0; i< testContent.size(); ++i) {
				myWriteableBlock[i] = 'A' + i%26;
				otherContent[i] = myWriteableBlock[i];
			}
			ENSURE(testContent.size()==0 || testContent != myWriteableBlock); 
		}
		asl::Block fromMappedFile;
		ENSURE(readFile(otherTestFileName,fromMappedFile));
		ENSURE(fromMappedFile == otherContent);  

        asl::deleteFile(testFileName);
        asl::deleteFile(otherTestFileName);
	}
};
class ResizeableMappedBlockUnitTest : public UnitTest {
public:
	ResizeableMappedBlockUnitTest() : UnitTest("ResizeableMappedBlockUnitTest") {  }
	void run() {
		const string testFileName = "MappedBlockUnitTest.testoutput";

		perform_resize(testFileName, 0);
		perform_resize(testFileName, 1);
		perform_resize(testFileName, 2);
		perform_resize(testFileName, 5);
        
		perform_resize(testFileName, 65536-1);
		perform_resize(testFileName, 65536);
		perform_resize(testFileName, 65536+1);
		perform_resize(testFileName, 2*65536-1);
		perform_resize(testFileName, 2*65536);
		perform_resize(testFileName, 2*65536+1);

		perform_resize(testFileName, 20*65536-1);
		perform_resize(testFileName, 20*65536);
		perform_resize(testFileName, 20*65536+1);

        testResizePerformance(testFileName, 1024, 1024);
        testResizePerformance(testFileName, 1, 1024 * 1024);
        testResizePerformance(testFileName, 1024, 1);
        testResizePerformance(testFileName, 1024 * 10, 1024);
        testResizePerformance(testFileName, 4096, 1024);
        testResizePerformance(testFileName, 1024, 4096);
#ifdef BIG_TEST
        testResizePerformance(testFileName, 1024*1024, 10);
        testResizePerformance(testFileName, 1024*1024, 20);
        testResizePerformance(testFileName, 1024*1024, 50);
        testResizePerformance(testFileName, 1024*1024, 100);
        testResizePerformance(testFileName, 1024*1024, 200);
#endif
        /*
        testResizePerformance(testFileName, 1024*1024, 50);
        testResizePerformance(testFileName, 1024*1024, 300);
        */
    }  

	void perform_resize(const string & testFileName, int contentSize) {
		DPRINT2("Resize Test with content of size", contentSize);
        Block myTestContent;
        {
		    MappedBlock mappedTestContent(testFileName);
            ENSURE(mappedTestContent);
		    for (int i = 0; i< contentSize; ++i) {
                unsigned char myNextContentByte = ' ' + i%(255-' ');
			    myTestContent.appendUnsigned8(myNextContentByte);
                mappedTestContent.appendUnsigned8(myNextContentByte);
		    }
            DB2(
            cerr << "myTestContent = " << myTestContent << endl;
            cerr << "mappedTestContent = " << mappedTestContent << endl;
               );
            ENSURE(myTestContent == mappedTestContent);
        }
        {
		    ConstMappedBlock myMapWrittenBlock(testFileName);
            ENSURE(myTestContent == myMapWrittenBlock);
        }
        asl::deleteFile(testFileName);
	}
	void testResizePerformance(const string & testFileName, int chunkSize, int chunkCount) {
		DPRINT2("Resize Performance Test with content of size", chunkSize*chunkCount);
        Block myTestContent;
        {
		    for (int i = 0; i< chunkSize; ++i) {
                unsigned char myNextContentByte = ' ' + i%(255-' ');
			    myTestContent.appendUnsigned8(myNextContentByte);
		    }
        }
//#define SKIP_WRITE_COMPARE
#ifndef SKIP_WRITE_COMPARE
        {
            ofstream outFile(testFileName.c_str(), ios::binary);
            ENSURE(outFile);
            NoisyScopeTimer myTimer(string("write ") + as_string(chunkCount) + " chunks of size " + as_string(chunkSize), chunkSize*chunkCount);
            for (int i = 0; i < chunkCount; ++i) {
                outFile.write(myTestContent.strbegin(),myTestContent.size());
                if (!outFile) {
                    FAILURE("i/o error");
                    return;
                }
            }
        }
        {
            ENSURE(ConstMappedBlock(testFileName).size() == chunkSize*chunkCount);
        }
#endif
#if 0
        ENSURE(ConstMappedBlock("small.testfile").size() == asl::getFileSize("small.testfile"));
        ENSURE(NewMappedBlock("medium.testfile",1024*1024*50).size() == 1024*1024*50);
        ENSURE(ConstMappedBlock("medium.testfile").size() == asl::getFileSize("medium.testfile"));
        ENSURE(ConstMappedBlock("large.testfile").size() == asl::getFileSize("large.testfile"));
        ENSURE(NewMappedBlock("xlarge.testfile",1024*1024*257).size() == 1024*1024*257);
#endif     
        const string testMapFileName = string("xmapped-")+testFileName;
        {
		    MappedBlock mappedTestContent(testMapFileName, 0);
            ENSURE(mappedTestContent);
            NoisyScopeTimer myTimer(string("mapped write ") + as_string(chunkCount) + " chunks of size " + as_string(chunkSize), chunkSize*chunkCount);
            for (int i = 0; i < chunkCount; ++i) {
                mappedTestContent.append(myTestContent);
                if (!mappedTestContent) {
                    FAILURE("i/o error");
                    return;
                }
            }
        }
#ifndef SKIP_WRITE_COMPARE

        {
            NoisyScopeTimer myTimer(string("compare two blocks of size ") + as_string(chunkCount*chunkSize/1024.0/1024.0) + " MB", chunkSize*chunkCount);
		    ConstMappedBlock myMapWrittenBlock(testFileName);
		    ConstMappedBlock myFileWrittenBlock(testMapFileName);
            ENSURE(myMapWrittenBlock == myFileWrittenBlock);
        }
//        asl::deleteFile(testFileName);
#endif
        asl::deleteFile(testMapFileName);
	}
};

class AppendMappedBlockTest : public UnitTest {
public:
	AppendMappedBlockTest() : UnitTest("AppendMappedBlockTest") {  }

    string createData(int begin, int end) {
        string myData;
        for (int i = begin; i < end; ++i) {
            myData += as_string(i) + ",";
        }
        return myData;
    }
    
	void run() {
		const string testFileName = "AppendMappedBlockTest.testoutput";
        
        string myData1 = createData(1,1000);
        string myData2 = createData(1000,2000);
        DPRINT(myData1.size());
        DPRINT(myData2.size());
        int totalSize = myData1.size()+myData2.size();

        {
            MappedBlock myBlock(testFileName);
            myBlock.append(myData1.c_str(), myData1.size());
            //myBlock.reserve(myBlock.size()); // this should not be necessary
            
            // if we don't call reserve, the file size is wrong.
            // this is bad if e.g. while downloading the program is interrupted
            // with CTRL-C since there will then be garbage behind the data.
            // reserve seems to be very expensive on win32 with large files
            ENSURE(getFileSize(testFileName) == myData1.size());   
        }
        // after closing the size is ok.
        ENSURE(getFileSize(testFileName) == myData1.size());   
        {
            MappedBlock myBlock(testFileName); // this should work
            // MappedBlock myBlock(testFileName, myData1.size()); // but we need this
            DPRINT(myBlock.size());
            myBlock.append(myData2.c_str(), myData2.size());
            // myBlock.reserve(myBlock.size()); // this should not be necessary 
            ENSURE(getFileSize(testFileName) == totalSize);   
        }
        DPRINT(totalSize);
        DPRINT(getFileSize(testFileName));
        ENSURE(getFileSize(testFileName) == totalSize);   
        ENSURE(readFile(testFileName) == myData1+myData2);
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        // addTest(new AppendMappedBlockTest);
        addTest(new MappedBlockUnitTest, 2);
        addTest(new ResizeableMappedBlockUnitTest, 2);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
