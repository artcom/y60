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
//    $RCSfile: testStream.tst.cpp,v $
//
//   $Revision: 1.1 $
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

#include "Stream.h"
#include "Block.h"
#include "MappedBlock.h"
#include "string_functions.h"

using namespace std;
using namespace asl;

struct SomeStruct {
	int a;
	float f;
	char c;
	char x[99];
};



class StreamUnitTest : public UnitTest {
public:
    StreamUnitTest() : UnitTest("StreamUnitTest") {  };

    template <class AC_BYTE_ORDER, class SIZE_TYPE, class OFFSET_TYPE>
    void perform_put(asl::WriteableArrangedStream<AC_BYTE_ORDER, SIZE_TYPE, OFFSET_TYPE> & theOutputStream) {
        for (int i = 0; i< 9; ++i) {
            theOutputStream.append(&i, sizeof(i));                 // 4
            theOutputStream.appendSigned(i);                       // 1
            theOutputStream.appendUnsigned(i);                     // 1
            theOutputStream.appendSigned64(i);                     // 8
            theOutputStream.appendUnsigned64(i);                   // 8
            theOutputStream.appendFloat64(double(i));              // 8
            theOutputStream.appendSigned32(i);                     // 4
            theOutputStream.appendUnsigned32(i);                   // 4
            theOutputStream.appendFloat32(float(i));               // 4
            theOutputStream.appendSigned16((short)i);              // 2
            theOutputStream.appendUnsigned16((short)i);            // 2
            theOutputStream.appendSigned8((signed char)i);         // 1
            theOutputStream.appendUnsigned8((unsigned char)i);     // 1
            theOutputStream.appendString(as_string(i));            // 1
            theOutputStream.appendCountedString(as_string(i));     // 2
        }

        // write unsigned integers
        theOutputStream.appendUnsigned(0x23ULL);
        theOutputStream.appendUnsigned(0xF0ULL);
        theOutputStream.appendUnsigned(0x1234ULL);
        theOutputStream.appendUnsigned(0x123456ULL);
        theOutputStream.appendUnsigned(0x12345678ULL);
        theOutputStream.appendUnsigned(0x123456789AULL);
        theOutputStream.appendUnsigned(0x123456789ABCULL);
        theOutputStream.appendUnsigned(0x123456789ABCDEULL);
        theOutputStream.appendUnsigned(0x123456789ABCDEF0ULL);

        // write negative signed integers
        theOutputStream.appendSigned(-100LL);
        theOutputStream.appendSigned(-128LL);
        theOutputStream.appendSigned(-16000);
        theOutputStream.appendSigned(-4096000LL);
        theOutputStream.appendSigned(-1048576000LL);
        theOutputStream.appendSigned(-268435456000LL);
        theOutputStream.appendSigned(-68719476736000LL);
        theOutputStream.appendSigned(-17592186044416000LL);
        theOutputStream.appendSigned(-4503599627370496000LL);

        // write signed integers
        theOutputStream.appendSigned(100LL);
        theOutputStream.appendSigned(127LL);
        theOutputStream.appendSigned(16000);
        theOutputStream.appendSigned(4096000LL);
        theOutputStream.appendSigned(1048576000LL);
        theOutputStream.appendSigned(268435456000LL);
        theOutputStream.appendSigned(68719476736000LL);
        theOutputStream.appendSigned(17592186044416000LL);
        theOutputStream.appendSigned(4503599627370496000LL);
    }
    
    template <class AC_BYTE_ORDER, class SIZE_TYPE, class OFFSET_TYPE>
    void perform_get(const asl::ReadableArrangedStream<AC_BYTE_ORDER, SIZE_TYPE, OFFSET_TYPE> & theInputStream) {

        unsigned k = 0;
        AC_SIZE_TYPE p = 0;
        for (int i = 0; i< 9; ++i) {
            int ii = 0;
            p = theInputStream.readBytes(&ii, sizeof(i), p);    // 4
            DPRINT(p);
            ENSURE(p == k + 4);
            ENSURE(ii == i);

            Signed32 iil = 0;
            p = theInputStream.readSigned(iil, p);              // 1
            ENSURE(p == k + 5);
            ENSURE(ii == static_cast<Signed32>(i));

            Unsigned32 iiul = 0;
            p = theInputStream.readUnsigned(iiul, p);             // 1
            ENSURE(p == k + 6);
            ENSURE(iiul == static_cast<Unsigned32>(i));

            Signed64 iill;
            p = theInputStream.readSigned64(iill, p);              // 8
            ENSURE(iill == static_cast<Signed64>(i));
            ENSURE(p == k + 14);

            Unsigned64 iiull;
            p = theInputStream.readUnsigned64(iiull, p);           // 8
            ENSURE(iiull == static_cast<Unsigned64>(i));
            ENSURE(p == k + 22);

            double iid=0;
            p = theInputStream.readFloat64(iid, p);               // 8
            ENSURE(p == k + 30);
            ENSURE(iid == double(i)); 

            iiul=0;
            p = theInputStream.readSigned32(ii, p);              // 4
            ENSURE(p == k + 34);
            ENSURE(ii == static_cast<Signed32>(i));
            
            Unsigned32 iiui=0;
            p = theInputStream.readUnsigned32(iiui, p);          // 4
            ENSURE(p == k + 38);
            ENSURE(iiui == static_cast<Unsigned32>(i));

            float iif=0;
            p = theInputStream.readFloat32(iif, p);               // 4
            ENSURE(p == k + 42);
            ENSURE(iif == float(i)); 

            short iis=0;
            p = theInputStream.readSigned16(iis, p);               // 2
            ENSURE(p == k + 44);
            ENSURE(iis == i);

            unsigned short iius=0;
            p = theInputStream.readUnsigned16(iius, p);              // 2
            ENSURE(p == k + 46);
            ENSURE(iius == i);

            unsigned char iiuc = 0;
            p = theInputStream.readUnsigned8(iiuc, p);               // 1
            ENSURE(p == k + 47);
            ENSURE(iiuc == i); ii=0;

            signed char iisc = 0;
            p = theInputStream.readSigned8(iisc, p);         // 1
            ENSURE(p == k + 48);
            ENSURE(iisc == i); 

            string iistr;
            p = theInputStream.readString(iistr, as_string(i).size(), p);// 1
            ENSURE(p == k + 48 + as_string(i).size());
            ENSURE(iistr == as_string(i)); iistr="";

            p = theInputStream.readCountedString(iistr, p);     // 1
            ENSURE(p == k + 49 + 2 * as_string(i).size());
            ENSURE(iistr == as_string(i));
            k = p;
        }

        // verify unsigned integers
        AC_SIZE_TYPE mySize = p;
        Unsigned64 myResult;

        p = theInputStream.readUnsigned(myResult, p);
        ENSURE(myResult == 0x23ULL);
        ENSURE(p == mySize + 1);

        p = theInputStream.readUnsigned(myResult, p);
        ENSURE(p == mySize + 3);
        ENSURE(myResult == 0xF0ULL);

        p = theInputStream.readUnsigned(myResult, p);
        ENSURE(p == mySize + 6);
        ENSURE(myResult == 0x1234ULL);

        p = theInputStream.readUnsigned(myResult, p);
        ENSURE(p == mySize + 10);
        ENSURE(myResult == 0x123456ULL);

        p = theInputStream.readUnsigned(myResult, p);
        ENSURE(p == mySize + 15);
        ENSURE(myResult == 0x12345678ULL);

        p = theInputStream.readUnsigned(myResult, p);
        ENSURE(p == mySize + 21);
        ENSURE(myResult == 0x123456789AULL);

        p = theInputStream.readUnsigned(myResult, p);
        ENSURE(p == mySize + 28);
        ENSURE(myResult == 0x123456789ABCULL);

        p = theInputStream.readUnsigned(myResult, p);
        ENSURE(p == mySize + 36);
        ENSURE(myResult == 0x123456789ABCDEULL);

        p = theInputStream.readUnsigned(myResult, p);
        ENSURE(p == mySize + 45);
        ENSURE(myResult == 0x123456789ABCDEF0ULL);

        // test negative signed values
        mySize = p;
        Signed64 mySignedResult = 0;

        ENSURE(p == mySize + 0);
        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(mySignedResult == -100LL);
        ENSURE(p == mySize + 1);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(mySignedResult == -128LL);
        ENSURE(p == mySize + 3);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(mySignedResult == -16000);
        ENSURE(p == mySize + 6);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(mySignedResult == -4096000LL);
        ENSURE(p == mySize + 10);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(mySignedResult == -1048576000LL);
        ENSURE(p == mySize + 15);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(mySignedResult == -268435456000LL);
        ENSURE(p == mySize + 21);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(mySignedResult == -68719476736000LL);
        ENSURE(p == mySize + 28);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(mySignedResult == -17592186044416000LL);
        ENSURE(p == mySize + 36);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(mySignedResult == -4503599627370496000LL);
        ENSURE(p == mySize + 45);

        // test positive signed values
        mySize = p;

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(p == mySize + 1);
        ENSURE(mySignedResult == 100LL);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(p == mySize + 3);
        ENSURE(mySignedResult == 127LL);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(p == mySize + 6);
        ENSURE(mySignedResult == 16000);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(p == mySize + 10);
        ENSURE(mySignedResult == 4096000LL);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(p == mySize + 15);
        ENSURE(mySignedResult == 1048576000LL);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(p == mySize + 21);
        ENSURE(mySignedResult == 268435456000LL);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(p == mySize + 28);
        ENSURE(mySignedResult == 68719476736000LL);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(p == mySize + 36);
        ENSURE(mySignedResult == 17592186044416000LL);

        p = theInputStream.readSigned(mySignedResult, p);
        ENSURE(p == mySize + 45);    
        ENSURE(mySignedResult == 4503599627370496000LL);

    }

    void run() {
        asl::Block myBlock;
        perform_put(myBlock);
        perform_get(myBlock);

        std::string myMappedFileName="testStream-mapped-file.tmp";
        {
            asl::MappedBlock myMappedFile(myMappedFileName, 0);
            perform_put(myMappedFile);
        }
        {
            asl::ConstMappedBlock myMappedFile(myMappedFileName);
            ENSURE(myMappedFile == myBlock);
        }

        std::string myFileName="testStream-file.tmp";
        {
            WriteableFile myOutputFile(myFileName);
            perform_put(myOutputFile);
        }
        {
            ReadableFile myInputFile(myFileName);
            //ReadableFile myInputFile(myMappedFileName);
            perform_get(myInputFile);
        }
        {
            ReadableArrangedFile<PowerPCByteOrder> myInputFile(myFileName);
            ReadableStreamAdapter<X86ByteOrder, PowerPCByteOrder> myInputAdapter(myInputFile);
            perform_get(myInputAdapter);
        }
        
        ENSURE( asl::ConstMappedBlock(myFileName) == asl::ConstMappedBlock(myMappedFileName));
         
        {
            WriteableArrangedFile<PowerPCByteOrder> myOutputFile(myFileName);
            perform_put(myOutputFile);
        }
        {
            ReadableArrangedFile<PowerPCByteOrder> myInputFile(myFileName);
            perform_get(myInputFile);
        }
         ENSURE(asl::ConstMappedBlock(myFileName) != asl::ConstMappedBlock(myMappedFileName));
         asl::deleteFile(myMappedFileName);
         asl::deleteFile(myFileName);
    }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new StreamUnitTest, 100);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}
