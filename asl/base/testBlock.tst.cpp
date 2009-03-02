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
//    $RCSfile: testBlock.tst.cpp,v $
//
//   $Revision: 1.8 $
//
// Description: unit test template file - change Block to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "UnitTest.h"

#include <string>
#include <iostream>

#include "begin_end.h"
#include "Block.h"
#include "string_functions.h"

using namespace std;
using namespace asl;

struct SomeStruct {
	int a;
	float f;
	char c;
	char x[99];
};



class BlockUnitTest : public UnitTest {
public:
    BlockUnitTest() : UnitTest("BlockUnitTest") {  }
	void testEmptyReadable(const ReadableBlock & theBlock) {
		ENSURE(theBlock.size() == 0);
		ENSURE(theBlock.end() == theBlock.begin());
		ENSURE(theBlock.strend() == theBlock.strbegin());
		ENSURE(theBlock.wend() == theBlock.wbegin());
		ENSURE(theBlock.wsize() == 0);
#ifdef goodNewCompiler
		try {
			theBlock.as<char>();
		}
		catch (BlockTooSmall & ex) {
			SUCCESS("correct exception thrown");
			DTITLE(ex);
			return;
		}
		FAILURE("theBlock.as<char> did not throw correct exception");
#endif
    }

	void testEmptyWriteable(WriteableBlock & theBlock) {
		ENSURE(theBlock.size() == 0);
		ENSURE(theBlock.end() == theBlock.begin());
		ENSURE(theBlock.strend() == theBlock.strbegin());
		ENSURE(theBlock.wend() == theBlock.wbegin());
		ENSURE(theBlock.wsize() == 0);
#ifdef goodNewCompiler
		try {
			theBlock.as<char>();
			FAILURE("theBlock.as<char> did not throw correct exception");
		}
		catch (BlockTooSmall & ex) {
			SUCCESS("correct exception thrown");
			DTITLE(ex);
		}
#endif
	}
	void testWriteable(WriteableBlock & theBlock, const ReadableBlock & theTestData) {
		ENSURE(theBlock.size());
		ENSURE(theBlock.end() != theBlock.begin());
		ENSURE(theBlock.strend() != theBlock.strbegin());
		ENSURE(theBlock.wend() != theBlock.wbegin());
		ENSURE(theBlock.wsize() == theBlock.size()/2);

		Block anotherBlock(asl::begin_ptr(theTestData),asl::end_ptr(theTestData));
		ENSURE(anotherBlock != theBlock);
		copy(anotherBlock,theBlock);
		ENSURE(anotherBlock == theBlock);

		anotherBlock.resize(1);
		try {
			copy(anotherBlock, theBlock);
			FAILURE("size mismatch not caught");
		}
		catch (BlockSizeMismatch & ex) {
			SUCCESS("caught size mismatch");
			DTITLE(ex);
		}
	}

	template <class RESIZEABLE_BLOCK>
 	void testResize(RESIZEABLE_BLOCK & theBlock) {
		theBlock.resize(1);
		ENSURE(theBlock.size() == 1);
		DDUMP(theBlock.size());
		ENSURE(theBlock.end() != theBlock.begin());
		ENSURE(theBlock.end() - theBlock.begin() == 1);
		theBlock[0] = 'x';
		ENSURE(theBlock[0] == 'x');
		ENSURE(*theBlock.begin() == 'x');
		ENSURE(*theBlock.strbegin() == 'x');
		ENSURE(theBlock.strend() - theBlock.strbegin() == 1);
#ifdef goodNewCompiler
		try {
			ENSURE(theBlock.as<char>() == 'x');
			ENSURE(theBlock.as<char>(0) == 'x');
		}
		catch (BlockTooSmall & ex) {
			FAILURE("exception thrown");
			DTITLE(ex);
		}
#endif
		char c = 'y';
		theBlock.append(&c, sizeof(c));
		ENSURE(theBlock.size() == 2);
		ENSURE(theBlock[0] == 'x');
		ENSURE(theBlock[1] == 'y');
		std::string myString(theBlock.strbegin(),theBlock.strend());
		ENSURE(myString == "xy");
		ENSURE(theBlock.wsize() == 1);
		ENSURE(theBlock.wend() - theBlock.wbegin() == 1);

		char d = 'a';
		theBlock.readData(d,0);
		ENSURE(d == 'x');
		theBlock.readData(d,1);
		ENSURE(d == 'y');
		try {
			theBlock.readData(d,2);
			FAILURE("no exception thrown");
		}
		catch (BlockTooSmall & ex) {
			SUCCESS("correct exception thrown");
			DTITLE(ex);
		}
		theBlock.appendUnsigned(23);
		theBlock.appendUnsigned16(42);
		theBlock.appendUnsigned8(5);
		AC_SIZE_TYPE myResult;
		ENSURE(theBlock.readUnsigned(myResult,2) == 2 + sizeof(unsigned char));
		ENSURE(myResult == 23);
		unsigned short myWord;
		ENSURE(theBlock.readUnsigned16(myWord,2+sizeof(unsigned char)) == 2 + sizeof(unsigned char)+sizeof(unsigned short));
		ENSURE(myWord == 42);

		unsigned char myByte;
		ENSURE(theBlock.readUnsigned8(myByte,2+sizeof(unsigned char)+sizeof(unsigned short))
			== 2 + sizeof(unsigned char)+sizeof(unsigned short)+sizeof(unsigned char));
		ENSURE(myByte == 5);

		int myLen = theBlock.size();
		theBlock.appendCountedString("blafasel");
		ENSURE(theBlock.size() == myLen + 8 + sizeof(unsigned char));
		string myString2;
		ENSURE(theBlock.readCountedString(myString2,myLen) == myLen+8+sizeof(unsigned char));
		ENSURE(myString2 == "blafasel");

        asl::Block myTestData;
		myTestData.appendCountedString("1234567890098761525125");
		myLen = theBlock.size();
        theBlock.append(myTestData);
        ENSURE(theBlock.size() == myLen+myTestData.size());
        asl::Block myBlock;
        myBlock.resize(myTestData.size());
        unsigned long myReadPos = theBlock.readBlock(myBlock,myLen);
        ENSURE(myBlock == myTestData);
        ENSURE(myReadPos == theBlock.size());

        // this test should better go to testStream
        theBlock.resize(0);
        ENSURE(theBlock.size() == 0);
        theBlock.appendUnsigned(0x23ULL);
        ENSURE(theBlock.size() == 1);
        theBlock.appendUnsigned(0xF0ULL);
        ENSURE(theBlock.size() == 3);
        theBlock.appendUnsigned(0x1234ULL);
        ENSURE(theBlock.size() == 6);
        theBlock.appendUnsigned(0x123456ULL);
        ENSURE(theBlock.size() == 10);
        theBlock.appendUnsigned(0x12345678ULL);
        ENSURE(theBlock.size() == 15);
        theBlock.appendUnsigned(0x123456789AULL);
        ENSURE(theBlock.size() == 21);
        theBlock.appendUnsigned(0x123456789ABCULL);
        ENSURE(theBlock.size() == 28);
        theBlock.appendUnsigned(0x123456789ABCDEULL);
        ENSURE(theBlock.size() == 36);
        theBlock.appendUnsigned(0x123456789ABCDEF0ULL);
        ENSURE(theBlock.size() == 45);
        
        myReadPos = 0;
        ENSURE(myReadPos == 0);
        Unsigned64 myValue;
        myReadPos = theBlock.readUnsigned(myValue, myReadPos);
        ENSURE(myReadPos == 1);
        ENSURE(myValue == 0x23ULL);
        myReadPos = theBlock.readUnsigned(myValue, myReadPos);
        ENSURE(myReadPos == 3);
        ENSURE(myValue == 0xF0ULL);
        myReadPos = theBlock.readUnsigned(myValue, myReadPos);
        ENSURE(myReadPos == 6);
        ENSURE(myValue == 0x1234ULL);
        myReadPos = theBlock.readUnsigned(myValue, myReadPos);
        ENSURE(myReadPos == 10);
        ENSURE(myValue == 0x123456ULL);
        myReadPos = theBlock.readUnsigned(myValue, myReadPos);
        ENSURE(myReadPos == 15);
        ENSURE(myValue == 0x12345678ULL);
        myReadPos = theBlock.readUnsigned(myValue, myReadPos);
        ENSURE(myReadPos == 21);
        ENSURE(myValue == 0x123456789AULL);
        myReadPos = theBlock.readUnsigned(myValue, myReadPos);
        ENSURE(myReadPos == 28);
        ENSURE(myValue == 0x123456789ABCULL);
        myReadPos = theBlock.readUnsigned(myValue, myReadPos);
        ENSURE(myReadPos == 36);
        ENSURE(myValue == 0x123456789ABCDEULL);
        myReadPos = theBlock.readUnsigned(myValue, myReadPos);
        ENSURE(myReadPos == 45);
        ENSURE(myValue == 0x123456789ABCDEF0ULL);
        
        theBlock.resize(0);
        theBlock.appendUnsigned8('x');
        theBlock.appendUnsigned8('y');
 		{
			Block myBlock = theBlock;
			ENSURE(myBlock[0] == 'x');
			ENSURE(myBlock[1] == 'y');
			ENSURE(myBlock.size() == 2);

			ENSURE(theBlock == myBlock);
			ENSURE(!(theBlock != myBlock));
			ENSURE(theBlock >= myBlock);
			ENSURE(theBlock <= myBlock);
			ENSURE(!(theBlock < myBlock));
			ENSURE(!(theBlock > myBlock));

			ENSURE(myBlock == theBlock);
			ENSURE(!(myBlock != theBlock));
			ENSURE(myBlock >= theBlock);
			ENSURE(myBlock <= theBlock);
			ENSURE(!(myBlock < theBlock));
			ENSURE(!(myBlock > theBlock));

			myBlock.resize(1);
			ENSURE(!(theBlock == myBlock));
			ENSURE(theBlock != myBlock);
			ENSURE(theBlock >= myBlock);
			ENSURE(!(theBlock <= myBlock));
			ENSURE(!(theBlock < myBlock));
			ENSURE(theBlock > myBlock);

			ENSURE(!(myBlock == theBlock));
			ENSURE(myBlock != theBlock);
			ENSURE(!(myBlock >= theBlock));
			ENSURE(myBlock <= theBlock);
			ENSURE(myBlock < theBlock);
			ENSURE(!(myBlock > theBlock));

			ENSURE(as_string(theBlock) == "7879");

		}

		{
			Block myBlock;
			copy(theBlock, myBlock);
			ENSURE(myBlock == theBlock);
		}

		myTestData.resize(9);
        for (asl::AC_SIZE_TYPE i = 0; i < 9; ++i) {
			myTestData[i] = static_cast<unsigned char>(i);
		}
		theBlock.resize(myTestData.size());
		testWriteable(theBlock, myTestData);

        {
            Block myIOBlock;
            myIOBlock = asl::as<Block>("123456");
            ENSURE(myIOBlock.size() == 3);
            ENSURE(myIOBlock[0]== 0x12);
            ENSURE(myIOBlock[1]== 0x34);
            ENSURE(myIOBlock[2]== 0x56);
        }
        {
            Block myIOBlock;
            myIOBlock = asl::as<Block>("123456 7890");
            ENSURE(myIOBlock.size() == 3);
            ENSURE(myIOBlock[0]== 0x12);
            ENSURE(myIOBlock[1]== 0x34);
            ENSURE(myIOBlock[2]== 0x56);
        }
        {
            ENSURE_EXCEPTION(asl::as<Block>("1234567 890"), ParseException);
        }
	}
	void testResizeAbstract(ResizeableBlock & theBlock) {
		testResize(theBlock);
	}
    void run() {

		Block myBlock;
		testEmptyReadable(myBlock);
		testEmptyWriteable(myBlock);
		testResize(myBlock);
		testResizeAbstract(myBlock);

		DTITLE("Starting FixedBlock Tests");
		FixedBlock<SomeStruct> myFixedBlock;
		testEmptyReadable(myFixedBlock);
		testEmptyWriteable(myFixedBlock);
		testResize(myFixedBlock);
		testResizeAbstract(myFixedBlock);

		myFixedBlock.resize(sizeof(SomeStruct));
		SUCCESS("Fixeblock resize ok")
		try {
			myFixedBlock.resize(sizeof(SomeStruct)+1);
			FAILURE("capacitycheck failed, no exception thrown");
		}
		catch (NotEnoughCapacity & ex) {
			SUCCESS("capacitycheck works, exception thrown");
			DTITLE(ex);
		}

		DTITLE("Starting SizeBlock Tests");
		SizeBlock mySizeBlock;
		testEmptyReadable(mySizeBlock);
		testEmptyWriteable(mySizeBlock);
		mySizeBlock.getSizeField() = 2;
		mySizeBlock[0] = 'a';
		mySizeBlock[1] = 'b';
		ENSURE(mySizeBlock.size() == 2);
		ENSURE(mySizeBlock.end() - mySizeBlock.begin()== 2);
		*mySizeBlock.getSizeFieldPtr() = 0;
		testEmptyReadable(mySizeBlock);
		testEmptyWriteable(mySizeBlock);

		testResize(mySizeBlock);
		testResizeAbstract(mySizeBlock);

		DTITLE("Starting Adapter Tests");

		Block anotherBlock(30);
		for (AC_SIZE_TYPE i=0; i< anotherBlock.size();++i) {
			anotherBlock[i] = static_cast<unsigned char>(i);
		};
		DTITLE("Starting Readable Block Adapter Tests");

		ReadableBlockAdapter emptyRBAdapter;
		testEmptyReadable(emptyRBAdapter);

		DTITLE("Starting Writeable Block Adapter Tests");

		ReadableBlockAdapter myRBAdapter(anotherBlock);
		Block anotherWriteableBlock;
		anotherWriteableBlock.resize(anotherBlock.size());
		testWriteable(anotherWriteableBlock, myRBAdapter);

		DTITLE("Starting both Readable/Writeable Block Adapter Tests");

		anotherBlock[0] = 1; // modify anotherblock
		WriteableBlockAdapter myWBAdapter(anotherWriteableBlock);
		testWriteable(myWBAdapter, myRBAdapter);

		WriteableBlockAdapter emptyWBAdapter;
		testEmptyWriteable(emptyWBAdapter);

		DTITLE("Starting FixedBlockAdapter Tests");

		anotherBlock[0] = 2; // modify anotherblock
		FixedBlockAdapter myFBAdapter(anotherWriteableBlock);
		testWriteable(myFBAdapter, myRBAdapter);

		FixedBlockAdapter emptyFBAdapter;
		testEmptyWriteable(emptyFBAdapter);
		testResize(myFBAdapter);
		testResizeAbstract(myFBAdapter);

		DTITLE("Starting CowBlock Tests");
        CowBlock myCowBlock;
		testEmptyReadable(myCowBlock);
		testEmptyWriteable(myCowBlock);
		testResize(myCowBlock);
		testResizeAbstract(myCowBlock);
    }
};

template <class T>
class BlockTemplateUnitTest : public TemplateUnitTest {
public:
    BlockTemplateUnitTest(const char * theTemplateArgument)
        : TemplateUnitTest("BlockTemplateUnitTest",theTemplateArgument) {}
    void run() {
        T someVariable = 1;
        ENSURE(someVariable != 0);
        DPRINT2(getMyName(),someVariable);
    }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new BlockUnitTest, 1000);
        addTest(new BlockTemplateUnitTest<bool>("<bool>"),1000);
        addTest(new BlockTemplateUnitTest<int>("<int>"),1000);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
