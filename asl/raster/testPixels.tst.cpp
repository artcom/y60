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
//    $RCSfile: testPixels.tst.cpp,v $
//
//   $Revision: 1.5 $
//
// Description: test pixel types
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#include "pixels.h"
#include "raster.h"
#include "standard_pixel_types.h"

#include <asl/UnitTest.h>
#include <asl/string_functions.h>

#include <string>
#include <iostream>
#include <algorithm>

#include <functional>

using namespace std;
using namespace asl;

template <class PIX>
class PixelUnitTest : public UnitTest {
public:
    PixelUnitTest(string myTypeName)
        : UnitTest(strdup(string(string("PixelUnitTest<")+myTypeName+">").c_str())) {  }
    void run() {
        PIX blackPixel;
        //DPRINT(&blackPixel);
        fill(begin(blackPixel),end(blackPixel),0);
        //DPRINT(blackPixel);
        PIX firstPixel = blackPixel;
        //DPRINT(firstPixel);
        ENSURE(firstPixel == blackPixel);

        PIX secondPixel = firstPixel + blackPixel;
        //DPRINT(secondPixel);
        ENSURE(secondPixel == blackPixel);

        PIX productPixel = firstPixel * blackPixel;
        //DPRINT(productPixel);
        ENSURE(productPixel == blackPixel);

        fill(begin(firstPixel),end(firstPixel),1);
        //DPRINT(firstPixel);
        ENSURE(firstPixel != blackPixel);
        secondPixel = firstPixel;
        //DPRINT(secondPixel);
        ENSURE(secondPixel == firstPixel);

        secondPixel *= firstPixel;
        //DPRINT(secondPixel);
        ENSURE(secondPixel == firstPixel);

        secondPixel /= firstPixel;
        //DPRINT(secondPixel);
        ENSURE(secondPixel == firstPixel);

        secondPixel += firstPixel;
        //DPRINT(secondPixel);
        fill(begin(firstPixel), end(firstPixel), 2);
        //DPRINT(firstPixel);
        ENSURE(secondPixel == firstPixel);

        std::multiplies<PIX> myMultiplier;
        PIX somePix = myMultiplier(firstPixel, secondPixel);

/*
        iota(begin(firstPixel),end(firstPixel),1);
        //DPRINT(firstPixel);
        iota(begin(secondPixel),end(secondPixel),1);
        //DPRINT(firstPixel);
        ENSURE(secondPixel == firstPixel);
  */
        sort(begin(firstPixel),end(firstPixel));
        ENSURE(secondPixel == firstPixel);
        //DPRINT(firstPixel);
    }
private:
};

class MiscPixelUnitTest : public UnitTest {
public:
    MiscPixelUnitTest() : UnitTest("MiscPixelUnitTest") {  }
    void run() {
        DPRINT(sizeof(red<unsigned char>));
        ENSURE(sizeof(red<unsigned char>) == 1);
        //DPRINT(sizeof(Pair<red<char>,green<char> >));
        //ENSURE(sizeof(Pair<red<char>,green<char> >) == 2);
        DPRINT(sizeof(RGB));
        ENSURE(sizeof(RGB) == 3);
        DPRINT(sizeof(RGB[10]));
        ENSURE(sizeof(RGB[10]) == 30);
        ENSURE(sizeof(BGR) == 3);
        ENSURE(sizeof(HSV) == 3);
        DPRINT(sizeof(RGBA));
        ENSURE(sizeof(RGBA) == 4);
        DPRINT(sizeof(RGBA[10]));
        ENSURE(sizeof(RGBA[10]) == 40);
        ENSURE(sizeof(ABGR) == 4);
        ENSURE(sizeof(BGRA) == 4);

        ENSURE(asl::as_string(RGB(1,2,3)) == "1 2 3");
        ENSURE(asl::as_string(RGBA(1,2,3,4)) == "1 2 3 4");
        ENSURE(asl::as<RGB>("1 2 3") == RGB(1,2,3));
        ENSURE(asl::as<RGBA>("1 2 3 4") == RGBA(1,2,3,4));

        ENSURE(sizeof(DXT1)  == 8);
        ENSURE(sizeof(DXT1a) == 8);
		ENSURE(sizeof(DXT3)  == 16);
		ENSURE(sizeof(DXT5)  == 16);

		{
			raster<DXT1> myRaster(4, 4);
			ENSURE(myRaster.getDataVector().size() == 1);
			ENSURE((char *)myRaster.end() - (char *)myRaster.begin() == 8);

			DXT1 myPixel;
			myPixel.value = 111;
			DXT1 myOtherPixel;
			myOtherPixel.value = 222;

			ENSURE(!(myPixel == myOtherPixel));
			ENSURE(myPixel + myPixel == myOtherPixel);
			ENSURE(myOtherPixel - myPixel == myPixel);
		}
		{
			raster<DXT1a> myRaster(4, 4);
			ENSURE(myRaster.getDataVector().size() == 1);
			ENSURE((char *)myRaster.end() - (char *)myRaster.begin() == 8);

			DXT1a myPixel;
			myPixel.value = 111;
			DXT1a myOtherPixel;
			myOtherPixel.value = 222;

			ENSURE(!(myPixel == myOtherPixel));
			ENSURE(myPixel + myPixel == myOtherPixel);
			ENSURE(myOtherPixel - myPixel == myPixel);
		}
		{
			raster<DXT3> myRaster(4, 4);
			ENSURE(myRaster.getDataVector().size() == 1);
			ENSURE((char *)myRaster.end() - (char *)myRaster.begin() == 16);

			DXT3 myPixel;
			myPixel.value1 = 111;
			myPixel.value2 = 333;
			DXT3 myOtherPixel;
			myOtherPixel.value1 = 222;
			myOtherPixel.value2 = 666;

			ENSURE(!(myPixel == myOtherPixel));
			ENSURE(myPixel + myPixel == myOtherPixel);
			ENSURE(myOtherPixel - myPixel == myPixel);
		}
		{
			raster<DXT5> myRaster(4, 4);
			ENSURE(myRaster.getDataVector().size() == 1);
			ENSURE((char *)myRaster.end() - (char *)myRaster.begin() == 16);

			DXT5 myPixel;
			myPixel.value1 = 111;
			myPixel.value2 = 333;
			DXT5 myOtherPixel;
			myOtherPixel.value1 = 222;
			myOtherPixel.value2 = 666;

			ENSURE(!(myPixel == myOtherPixel));
			ENSURE(myPixel + myPixel == myOtherPixel);
			ENSURE(myOtherPixel - myPixel == myPixel);
		}
		{
			raster<RGB> myRaster(2, 4);
			ENSURE(myRaster.getDataVector().size() == 8);
			ENSURE((char *)myRaster.end() - (char *)myRaster.begin() == 24);
		}
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new MiscPixelUnitTest);
        addTest(new PixelUnitTest<char>("char"));
        addTest(new PixelUnitTest<unsigned char>("unsigned char"));
        addTest(new PixelUnitTest<int>("int"));
        addTest(new PixelUnitTest<unsigned int>("unsigned int"));
        addTest(new PixelUnitTest<float>("float"));
        addTest(new PixelUnitTest<double>("double"));
        addTest(new PixelUnitTest<red<char> >("red<char> "));
        addTest(new PixelUnitTest<green<char> >("green<char> "));
        addTest(new PixelUnitTest<blue<char> >("blue<char> "));
        addTest(new PixelUnitTest<saturation<char> >("saturation<char> "));
        addTest(new PixelUnitTest<saturation<unsigned char> >("saturation<unsigned char> "));
        addTest(new PixelUnitTest<red<int> >("red<int> "));
        addTest(new PixelUnitTest<red<float> >("red<int> "));

        addTest(new PixelUnitTest<RGB>("RGB"));
        addTest(new PixelUnitTest<BGR>("BGR"));
        addTest(new PixelUnitTest<HSV>("HSV"));
        addTest(new PixelUnitTest<RGBA>("RGBA"));
        addTest(new PixelUnitTest<ABGR>("ABGR"));
        addTest(new PixelUnitTest<BGRA>("BGRA"));
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
