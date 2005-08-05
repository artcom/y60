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
            ENSURE(ii == i);

            Unsigned32 iiul = 0;
            p = theInputStream.readUnsigned(iiul, p);             // 1
            ENSURE(p == k + 6);
            ENSURE(iiul == i);

            Signed64 iill;
            p = theInputStream.readSigned64(iill, p);              // 8
            ENSURE(iill == i);
            ENSURE(p == k + 14);

            Unsigned64 iiull;
            p = theInputStream.readUnsigned64(iiull, p);           // 8
            ENSURE(iiull == i);
            ENSURE(p == k + 22);

            double iid=0;
            p = theInputStream.readFloat64(iid, p);               // 8
            ENSURE(p == k + 30);
            ENSURE(iid == double(i)); 

            iiul=0;
            p = theInputStream.readSigned32(ii, p);              // 4
            ENSURE(p == k + 34);
            ENSURE(ii == i);
            
            Unsigned32 iiui=0;
            p = theInputStream.readUnsigned32(iiui, p);          // 4
            ENSURE(p == k + 38);
            ENSURE(iiui == i);

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
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new StreamUnitTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}
