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
//    $RCSfile: testRunLengthEncoder.tst.cpp,v $
//
//    $Revision: 1.5 $
//
//    Description:
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "RunLengthEncoder.h"

#include "UnitTest.h"
#include "Time.h"
#include "directory_functions.h"
#include "file_functions.h"

#include <string>
#include <iostream>

using namespace std;
using namespace asl;

double        ourTotalCompressTime;
double        ourTotalUncompressTime;
double        ourTotalBaselineTime;
unsigned long ourTotalSize;
unsigned long ourTotalCompressedSize;

class RunLengthEncoderUnitTest : public UnitTest {
    public:
        RunLengthEncoderUnitTest() : UnitTest("RunLengthEncoderUnitTest") {}

        void run() {
            testBasics();
        }

    private:
        void testBasics();
        void testDataCompression(unsigned char * theData, unsigned theSize);
        void testNumberCoder(unsigned long theNumber);
        template <class T>
        void testRunLengthEncoder(const asl::ReadableBlock & theBlock);

        void testRunLengthEncoder(const asl::ReadableBlock & theBlock, unsigned thePixelSize);
        void testStringCompression(const string & theData);
};

void
RunLengthEncoderUnitTest::testBasics() {
    testNumberCoder(0);
    testNumberCoder(1);
    testNumberCoder(2);
    testNumberCoder(10000);
    testNumberCoder(128);
    testNumberCoder(127);
    testNumberCoder(129);
    testNumberCoder(1000000000);

    testStringCompression(string("a"));
    testStringCompression(string("aa"));
    testStringCompression(string("ab"));
    testStringCompression(string("abcdeffffg"));
    testStringCompression(string("abcdefghijk"));
    testStringCompression(string("aaaaaaaaaaaaaa"));
    testStringCompression(string("aabbccddeeffgghhiijj"));
    testStringCompression(string("abcdefgjj"));
    testStringCompression(string("aaaaabcdefgggggggghijklmmmnopqqrsssssssssssssssttuuuuvwxyyyz"));
    string myData;
    myData.assign(63, 'a');
    testStringCompression(myData);
    myData.assign(64, 'a');
    testStringCompression(myData);
    myData.assign(65, 'a');
    testStringCompression(myData);
    myData.assign(255, 'a');
    myData.assign(256, 'a');
    myData.assign(257, 'a');
    testStringCompression(myData);
    myData.assign(8192, 'a');
    testStringCompression(myData);

    ENSURE_EXCEPTION(testDataCompression((unsigned char *)"abcd\0", 3), RunLengthEncodingException);

    testDataCompression((unsigned char *)"abcd\0", 4);
    testDataCompression((unsigned char *)"abcdefgh\0", 8);

    unsigned char myTestImage[401];
    myTestImage[400] = 0;
    for (unsigned i = 0; i < 400; i += 4) {
        myTestImage[i]   = 97;
        myTestImage[i+1] = 98;
        myTestImage[i+2] = 99;
        myTestImage[i+3] = 100;
    }
    testDataCompression((unsigned char *)&myTestImage, 400);

    for (unsigned i = 0; i < 400; ++i) {
        myTestImage[i] = 99;
    }
    testDataCompression((unsigned char *)&myTestImage, 400);
    for (unsigned i = 0; i < 400; ++i) {
        myTestImage[i] = (i % 10) + 48;
    }
    testDataCompression((unsigned char *)&myTestImage, 400);
}

template <class T>
void
RunLengthEncoderUnitTest::testRunLengthEncoder(const asl::ReadableBlock & theBlock) {
    {
        asl::Block myCompressed;
        RLE::compress<T>(theBlock, myCompressed);
        cerr << "  Original size: " << theBlock.size() << ", Compressed size: " << myCompressed.size() <<
                " Ratio: " << float(myCompressed.size()) / theBlock.size() << endl;

        asl::Block myUncompressed;
        RLE::uncompress(myCompressed, myUncompressed);
        ENSURE(theBlock == myUncompressed);
    }
    {
        asl::Block myCompressed;
        RLE::compress(theBlock, myCompressed, sizeof(T));
        asl::Block myUncompressed;
        RLE::uncompress(myCompressed, myUncompressed);
        ENSURE(theBlock == myUncompressed);
    }
}

void
RunLengthEncoderUnitTest::testRunLengthEncoder(const asl::ReadableBlock & theBlock, unsigned thePixelSize) {
    asl::Time myCompressStart;

    asl::Block myCompressed;
    RLE::compress(theBlock, myCompressed, thePixelSize);

    asl::Time myCompressStop;

    asl::Block myUncompressed;
    asl::Time myUncompressStart;
    RLE::uncompress(myCompressed, myUncompressed);
    asl::Time myUncompressStop;

    ourTotalCompressTime   += (myCompressStop - myCompressStart);
    ourTotalUncompressTime += (myUncompressStop - myUncompressStart);
    ourTotalSize           += theBlock.size();
    ourTotalCompressedSize += myCompressed.size();

    ENSURE(theBlock == myUncompressed);

    cerr << "  Uncompressed: " << theBlock.size() << " Compressed: " << myCompressed.size()
         << " Ratio: " << float(myCompressed.size()) / theBlock.size() << endl;
    cerr << "  Compress time:   " << myCompressStop - myCompressStart << endl;
    cerr << "  Uncompress time: " << myUncompressStop - myUncompressStart << endl;
}

void
RunLengthEncoderUnitTest::testNumberCoder(unsigned long theNumber) {
    cerr << "Coding: " << theNumber << " -> ";
    asl::Block myCodedNumber;
    RLE::writeNumber(myCodedNumber, theNumber);

    for (unsigned i = 0; i < myCodedNumber.size(); ++i) {
        cerr << myCodedNumber[i] << "(" << (int)(myCodedNumber[i]) << ") ";
    }

    const unsigned char * myCodeNumberIt = myCodedNumber.begin();
    unsigned long myResult = RLE::readNumber(myCodeNumberIt);
    cerr << " -> " << myResult << endl;

    ENSURE(myResult == theNumber)
}

void
RunLengthEncoderUnitTest::testStringCompression(const string & theData) {
    cerr << "Compressing: " << theData << endl;
    asl::Block myBlock;
    myBlock.appendString(theData);

    testRunLengthEncoder<unsigned char>(myBlock);
}

void
RunLengthEncoderUnitTest::testDataCompression(unsigned char * theData, unsigned theSize) {
    cerr << "Compressing: " << theData << endl;
    asl::Block myBlock;
    myBlock.append(theData, theSize);

    testRunLengthEncoder<unsigned>(myBlock);
}

class MyTestSuite : public UnitTestSuite {
    public:
        MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
        void setup() {
            UnitTestSuite::setup(); // called to print a launch message
            addTest(new RunLengthEncoderUnitTest);
        }
};

void printUsage() {
    cerr << "Usage:" << endl;
    cerr << "--imgdir [Directory]      Run performance test on a directory with images" << endl;
    cerr << "--dxtdir [Directory]      Run performance test on a directory with DirectX compressed textures" << endl;
    exit(0);
}

int main(int argc, char *argv[]) {
    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    cerr << endl << "Total Size:            " << (ourTotalSize) << endl;
    cerr << "Total compressed size: " << (ourTotalCompressedSize) << endl;
    cerr << "Total ratio:           " << (double(ourTotalCompressedSize) / ourTotalSize) << endl;
    cerr << "Total baseline time:   " << ourTotalBaselineTime << endl;
    cerr << "Total compress time:   " << ourTotalCompressTime << "s ("
         << (ourTotalSize / (1000 * 1000 * ourTotalCompressTime)) << " MB/s)" << endl;
    cerr << "Total uncompress time: " << ourTotalUncompressTime << "s ("
         << (ourTotalCompressedSize / (1000 * 1000 * ourTotalUncompressTime)) << " MB/s)" << endl;

    return mySuite.returnStatus();
}
