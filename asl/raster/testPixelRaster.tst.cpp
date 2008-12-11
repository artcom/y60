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
//    $RCSfile: testPixelRaster.tst.cpp,v $
//
//   $Revision: 1.9 $
//
// Description: unit test template file - change PixelRaster to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#define USE_FORMATTED_TEST_RASTER_OUTPUT_OPERATOR

#include "pixels.h"
#include "raster.h"
#include "subraster.h"
#include "palgo.h"
#include "piterator.h"
#include "standard_pixel_types.h"
#include "pixel_functions.h"

#include <asl/base/UnitTest.h>
#include <asl/base/Time.h>

using namespace std;  
using namespace asl;  

int mydata[] = {
          0,  10,  20,  30,  40,  50,  60,  70,  80,  90, 
		100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 
		200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 
		300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 
		400, 410, 420, 430, 440, 450, 460, 470, 480, 490, 
		500, 510, 520, 530, 540, 550, 560, 570, 580, 590, 
		600, 610, 620, 630, 640, 650, 660, 670, 680, 690, 
		700, 710, 720, 730, 740, 750, 760, 770, 780, 790, 
		800, 810, 820, 830, 840, 850, 860, 870, 880, 890, 
		900, 910, 920, 930, 940, 950, 960, 970, 980, 990
};


class Scale2dTest : public UnitTest {
public:
    Scale2dTest() : UnitTest("Scale2dTest") {  }
    void run() {
        DTITLE("---------------Scale2dTest -------------------");

        raster<int> mySrc(10, 10, mydata);
        raster<int> myDest(5, 5, -1);

        int myExpectedData[] = {
                0,  20,  40,  60,  80, 
		        200, 220, 240, 260, 280, 
		        400, 420, 440, 460, 480, 
		        600, 620, 640, 660, 680, 
		        800, 820, 840, 860, 880
        };
        raster<int> myExpected(5,5,myExpectedData);

        scale2d(mySrc, myDest);
 
        ENSURE(myDest == myExpected);
        DTITLE("expectedResult:");
        cerr << myExpected << endl;
        DTITLE("result:");
        cerr << myDest << endl;

    }
};

class FractionIteratorTest1 : public UnitTest {
public:
    FractionIteratorTest1() : UnitTest("FractionIteratorTest1") {  }
    void run() {
        DTITLE("---------------fraction_iterator 10/3 -------------------");

        fraction_iterator<int*, int> begin(mydata, AC_OFFSET_TYPE(10), AC_OFFSET_TYPE(3));
        fraction_iterator<int*, int> end = begin + 10;
        vector<int> result(11,-1);
        copy(begin, end, result.begin());
        int expectedResult[] = {0,30,70,100,130,170,200,230,270,300,-1};
        int expectedResultWin[] = {0,30,60,100,130,160,200,230,260,300,-1};

        ENSURE(equal(expectedResult,expectedResult+11,result.begin()) ||
               equal(expectedResultWin,expectedResultWin+11,result.begin()));
        DTITLE("expectedResult:");
        copy(expectedResult, expectedResult+11, ostream_iterator<int>(cerr, " "));cerr<<endl;
        DTITLE("expectedResultWin:");
        copy(expectedResultWin, expectedResultWin+11, ostream_iterator<int>(cerr, " "));cerr<<endl;
        DTITLE("result:");
        copy(result.begin(), result.end(), ostream_iterator<int>(cerr, " "));cerr<<endl;

    }
};

class FractionIteratorTest2 : public UnitTest {
public:
    FractionIteratorTest2() : UnitTest("FractionIteratorTest2") {  }
    void run() {
        DTITLE("---------------fraction_iterator 10/5 -------------------");

        fraction_iterator<int*, int> begin(mydata, AC_OFFSET_TYPE(10), AC_OFFSET_TYPE(5));
        fraction_iterator<int*, int> end = begin + 5;
        vector<int> result(6,-1);
        copy(begin, end, result.begin());
        int expectedResult[] = {0,20,40,60,80,-1};

        ENSURE(equal(expectedResult,expectedResult+6,result.begin()));
        DTITLE("expectedResult:");
        copy(expectedResult, expectedResult+6, ostream_iterator<int>(cerr, " "));cerr<<endl;
        DTITLE("result:");
        copy(result.begin(), result.end(), ostream_iterator<int>(cerr, " "));cerr<<endl;

    }
};

class FractionIteratorTest3 : public UnitTest {
public:
    FractionIteratorTest3() : UnitTest("FractionIteratorTest3") {  }
    void run() {
        DTITLE("---------------fraction_iterator 3/10 -------------------");

        fraction_iterator<int*, int> begin(mydata, AC_OFFSET_TYPE(3), AC_OFFSET_TYPE(10));
        fraction_iterator<int*, int> end = begin + 10;
        vector<int> result(11,-1);
        copy(begin, end, result.begin());
        int expectedResult[] = {0,0,0,10,10,10,20,20,20,20,-1};
        int expectedResultWin[] = {0,0,0,10,10,10,20,20, 20, 30, -1};

        ENSURE(equal(expectedResult,expectedResult+11,result.begin()) || 
            equal(expectedResultWin,expectedResultWin+11,result.begin()));
        DTITLE("expectedResult:");
        copy(expectedResult, expectedResult+11, ostream_iterator<int>(cerr, " "));cerr<<endl;
        DTITLE("expectedResultWin:");
        copy(expectedResultWin, expectedResultWin+11, ostream_iterator<int>(cerr, " "));cerr<<endl;
        DTITLE("result:");
        copy(result.begin(), result.end(), ostream_iterator<int>(cerr, " "));cerr<<endl;

    }
};

template <class MAT, class PIX>
class RasterTest1 : public UnitTest {
public:
    RasterTest1(string myTypeName) 
        : UnitTest(strdup(string(string("RasterTest1<")+myTypeName+">").c_str())) {  }
    void run() {

        MAT myMap(5,5, 
                "ABCDE"
                "FGHIJ"
                "KLMNO"
                "PQRST"
                "UVWXY");
        DDUMP(myMap);

        rect_iterator_test(myMap);

        MAT subMap(myMap,0,0,5,5);
        DDUMP(subMap);

        ENSURE(myMap == subMap);

        MAT nonSquareMap(7,4,  
                "ABCDEFG"
                "HIJKLMN"
                "OPQRSTU"
                "VWXYZab");
        DDUMP(subMap);

        MAT expectedResultMap(3,2,
                "RST"
                "YZa");
        DDUMP(expectedResultMap);

        MAT subResultMap(nonSquareMap,3,2,3,2);
        DDUMP(subResultMap);

        ENSURE(subResultMap == expectedResultMap);

        rect_iterator_test(subResultMap);
    }
    template <class MMAT>
    void rect_iterator_test(const MMAT & m) {
        for (int y = 0; y < m.ysize(); ++y) {
            for (int x = 0; x < m.xsize(); ++x) {
                for (int ys = 0; y + ys < m.ysize(); ++ys) {
                    for (int xs = 0; x + xs < m.xsize(); ++xs) {
                        rect_iterator_test(m,x,y,xs,ys);
                    }
                }
            }
        }
    }

    template <class MMAT>
    void rect_iterator_test(const MMAT & m, int x, int y, int xsize, int ysize) {
        cerr << endl;
        cerr << "rect_iterator_test: x = " << x << ", y = " << y 
            << ", xsize = " << xsize << ", ysize = " << ysize << ":" << endl;
        typename MMAT::const_sub_iterator bgn = m.begin(x,y,xsize,ysize);
        typename MMAT::const_sub_iterator end = m.end(x,y,xsize,ysize);
        //PIX px = *bgn;
        DPRINT((const void*)&(*bgn));
        DPRINT((const void*)&(*end));
        int subsize = end - bgn;
        DPRINT(subsize);
        DPRINT(xsize * ysize);
        ENSURE(subsize == xsize * ysize);
        int i = 0;
        while (bgn!=end) {
            ++bgn; ++i;
        }
        DPRINT(i);
        ENSURE( i == xsize * ysize);

        typename MMAT::const_sub_iterator sbgn = m.begin(x,y,xsize,ysize);
        typename MMAT::const_sub_iterator send = m.end(x,y,xsize,ysize);
        for (int iy = 0; iy < ysize; ++iy) {
            for (int ix = 0; ix < xsize; ++ix) {
                ENSURE(&m(x + ix, y + iy) == &(*sbgn));
                ENSURE(m(x + ix, y + iy) == *sbgn);
                ENSURE(sbgn != send);
                ++sbgn;
            }
        }
        ENSURE(sbgn == send);
    }
private:
};

class BresenhamIteratorTest1 : public UnitTest {
public:
    BresenhamIteratorTest1() : UnitTest("BresenhamIteratorTest1") {  }
    void run() {
        DTITLE("--------------- BresenhamIteratorTest1 -------------------");
		    
        raster<char> zm(17, 19);
        raster<char>::horizontal_line_iterator start = zm.hfind(6, 6);
        const char myDot = '.';
        fill(zm.begin(), zm.end(), myDot);

        bresenham_fill(start, zm.stride(), 5, 5, '1');
        zm.dump();
        bresenham_fill(start, zm.stride(), 5, 0, '2');
        zm.dump();
        bresenham_fill(start, zm.stride(), 5, -5, '3');
        zm.dump();
        bresenham_fill(start, zm.stride(), -5, 0, '4');
        zm.dump();
        bresenham_fill(start, zm.stride(), -5, 5, '5');
        zm.dump();
        bresenham_fill(start, zm.stride(), -5, -5, '6');
        zm.dump();
        bresenham_fill(start, zm.stride(), 0, -5, '7');
        zm.dump();
        bresenham_fill(start, zm.stride(), 0, 5, '8');
        zm.dump();
        bresenham_fill(start, zm.stride(), 0, 0, '9');
        zm.dump();


        char myExpectedChars[] = {
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','6','.','.','.','7','.','.','.','3','.','.','.','.','.','.',
'.','.','.','6','.','.','7','.','.','3','.','.','.','.','.','.','.',
'.','.','.','.','6','.','7','.','3','.','.','.','.','.','.','.','.',
'.','.','.','.','.','6','7','3','.','.','.','.','.','.','.','.','.',
'.','.','4','4','4','4','8','2','2','2','2','.','.','.','.','.','.',
'.','.','.','.','.','5','8','1','.','.','.','.','.','.','.','.','.',
'.','.','.','.','5','.','8','.','1','.','.','.','.','.','.','.','.',
'.','.','.','5','.','.','8','.','.','1','.','.','.','.','.','.','.',
'.','.','5','.','.','.','8','.','.','.','1','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'
        };
        const_subraster<char> myExpectedRaster(17, 19, myExpectedChars);
        myExpectedRaster.dump();
        ENSURE(myExpectedRaster.size() == zm.size() && std::equal(myExpectedRaster.begin(),myExpectedRaster.end(),zm.begin()));

        bresenham_fill(start, zm.stride(), 6, 5, '*');
        zm.dump();
        bresenham_fill(start, zm.stride(), 1, 9, '+');
        zm.dump();
        bresenham_fill(start, zm.stride(), 2, 9, '@');
        zm.dump();
        bresenham_fill(start, zm.stride(), 3, 9, '&');
        zm.dump();
        bresenham_fill(start, zm.stride(), 4, 9, '$');
        zm.dump();
        bresenham_fill(start, zm.stride(), 5, 6, '#');
        zm.dump();
        bresenham_fill(zm.find(10, 11), zm.stride(), -5, -6, 'X');
        zm.dump();

        char myExpectedChars2[] = {
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','6','.','.','.','7','.','.','.','3','.','.','.','.','.','.',
'.','.','.','6','.','.','7','.','.','3','.','.','.','.','.','.','.',
'.','.','.','.','6','.','7','.','3','.','.','.','.','.','.','.','.',
'.','.','.','.','.','6','7','3','.','.','.','.','.','.','.','.','.',
'.','.','4','4','4','4','X','2','2','2','2','.','.','.','.','.','.',
'.','.','.','.','.','5','$','X','.','.','.','.','.','.','.','.','.',
'.','.','.','.','5','.','@','$','X','*','.','.','.','.','.','.','.',
'.','.','.','5','.','.','+','$','X','1','*','.','.','.','.','.','.',
'.','.','5','.','.','.','+','&','$','X','1','*','.','.','.','.','.',
'.','.','.','.','.','.','.','@','$','.','X','.','.','.','.','.','.',
'.','.','.','.','.','.','.','@','&','$','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','+','&','$','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','+','@','&','$','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.',
'.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.','.'
        };
        const_subraster<char> myExpectedRaster2(17, 19, myExpectedChars2);
        myExpectedRaster2.dump();
        ENSURE(myExpectedRaster2.size() == zm.size() && std::equal(myExpectedRaster2.begin(),myExpectedRaster2.end(),zm.begin()));

    }
};

typedef asl::NanoTime ptime;

template <class mat>
struct iter_perf
{
    iter_perf(char* testname) 
    {
        cerr << "==== Testing type : " << testname << endl;
        typedef typename mat::iterator mat_iterator;
        typedef typename mat::value_type mat_value_type;
        typedef typename mat::reference mat_reference;

	    {
	        // Performance
	        int n = 5;
	        mat m(1280, 1024);
	        int i;
	        step_iterator<mat_iterator, mat_value_type,  mat_reference> bgn(m.begin(), 1);
	        step_iterator<mat_iterator, mat_value_type,  mat_reference> end(m.end(), 1);
	        ptime start;
	        for (i=0;i<n;i++)
	        {
		    fill(bgn,end, mat_value_type());
	        }
		    ptime stop;
            double duration = ptime(stop-start).seconds(); 

	        cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	        cerr << "fillrate step:" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	        cerr << "fillrate step:" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
	    }

  	    {
	        // Performance
	        int n = 5;
	        mat m(1280, 1024);
	        int i;
	        step_iterator<mat_iterator, mat_value_type,  mat_reference> bgn(m.begin(), 1);
	        step_iterator<mat_iterator, mat_value_type,  mat_reference> end(m.end(), 1);
	        ptime start;
	        for (i=0;i<n;i++)
	        {
		    pfill(bgn,end, mat_value_type());
	        }
	        ptime stop;
            double duration = ptime(stop-start).seconds(); 

	        cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	        cerr << "pfillrate step:" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	        cerr << "pfillrate step:" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
	    }
	    {
            // Performance
            int n = 1;
            mat m(1280, 1024);
            int i;
            fraction_iterator<mat_iterator, mat_value_type,  mat_reference> bgn(m.begin(), AC_OFFSET_TYPE(1), AC_OFFSET_TYPE(1));
            fraction_iterator<mat_iterator, mat_value_type,  mat_reference> end(m.end(), AC_OFFSET_TYPE(1), AC_OFFSET_TYPE(1));
            ptime start;
            for (i=0;i<n;i++)
            {
                fill(bgn,end, mat_value_type());
            }
            ptime stop;
            double duration = ptime(stop-start).seconds(); 

            cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
            cerr << "fillrate fraction:" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "fillrate fraction:" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
        }

        {
            // Performance
            int n = 1;
            mat m(1280, 1024);
            int i;
            fraction_iterator<mat_iterator, mat_value_type,  mat_reference> bgn(m.begin(), AC_OFFSET_TYPE(1), AC_OFFSET_TYPE(1));
            fraction_iterator<mat_iterator, mat_value_type,  mat_reference> end(m.end(), AC_OFFSET_TYPE(1), AC_OFFSET_TYPE(1));
            ptime start;
            for (i=0;i<n;i++)
            {
                pfill(bgn,end, mat_value_type());
            }
            ptime stop;
            double duration = ptime(stop-start).seconds(); 

            cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
            cerr << "pfillrate fraction:" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "pfillrate fraction:" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
        }
        {
            // Performance
            int n = 100;
            mat m(128, 128);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                fill(m.begin(), m.end(), mat_value_type());
            }
            ptime stop;
            double duration = ptime(stop-start).seconds(); 

            cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
            cerr << "fillrate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "fillrate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
        }

	    {
            // Performance
            int n = 100;
            mat m(128, 128);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                pfill(m.begin(), m.end(), mat_value_type());
            }
            ptime stop;
            double duration = ptime(stop-start).seconds(); 

            cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
            cerr << "pfillrate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "pfillrate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
        }

        {
            // Performance
            int n = 10;
            mat m(1280,1024);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                fill(m.begin(), m.end(), mat_value_type());
            }
            ptime stop;
            double duration = ptime(stop-start).seconds(); 

            cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
            cerr << "fillrate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "fillrate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
        }
        {
            // Performance
            int n = 10;
            mat m(1280,1024);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                pfill(m.begin(), m.end(), mat_value_type());
            }
            ptime stop;
            double duration = ptime(stop-start).seconds(); 

            cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
            cerr << "pfillrate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "pfillrate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
        }

	    {
            // Performance
            int n = 10;
            mat m(1280,1024);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                int x, y;
                mat_value_type v(0);
                for (y = 0; y < m.ysize(); y++) {
                    for (x = 0; x < m.xsize(); x++) {
                        m(x, y) = v;
                    }
                }
            }
            ptime stop;
            double duration = ptime(stop-start).seconds(); 

            cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
            cerr << "fillrate indexed:" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "fillrate indexed:" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
        }

	    {
	        // Performance
	        int n = 10;
	        mat m(1280,1024);
	        int i;
	        ptime start;
	        for (i=0;i<n;i++)
	        {
		    int x, y;
		    mat_value_type v(0);
		    for (y = 0; y < m.ysize(); y++) {
		        for (x = 0; x < m.xsize(); x++) {
			    m(x, y) = v;
		        }
		    }
	        }
	        ptime stop;
            double duration = ptime(stop-start).seconds(); 

	        cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	        cerr << "fillrate indexed():" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	        cerr << "fillrate indexed():" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
	    }

	    {
	        // Performance
	        int n = 10;
	        mat m(1280,1024);
	        int i;
	        ptime start;
	        for (i=0;i<n;i++)
	        {
		    int x, y;
		    int xx = m.xsize();
		    int yy = m.ysize();
		    mat_value_type v(0);
		    for (y = 0; y < yy; y++) {
		        for (x = 0; x < xx; x++) {
			    m(x, y) = v;
		        }
		    }
	        }
	        ptime stop;
            double duration = ptime(stop-start).seconds(); 

	        cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	        cerr << "fillrate indexed() optloop:" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	        cerr << "fillrate indexed() optloop:" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
	    }

	    {
	        // Performance
	        int n = 10;
	        mat m0(1280,1024);
	        subraster<mat_value_type> m(m0, AC_OFFSET_TYPE(1), AC_OFFSET_TYPE(1), m0.xsize() - 2, m0.ysize() - 2);
	        int i;
	        ptime start;
	        for (i=0;i<n;i++)
            {
                int x, y;
                mat_value_type v(0);
                for (y = 0; y < m.ysize(); y++) {
                    for (x = 0; x < m.xsize(); x++) {
                        m(x, y) = v;
                    }
                }
            }
	        ptime stop;
            double duration = ptime(stop-start).seconds(); 

	        cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	        cerr << "fillrate sub indexed:" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	        cerr << "fillrate sub indexed:" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
	    }

	    {
	        // Performance
	        int n = 10;
	        mat m0(1280,1024);
	        subraster<mat_value_type> m(m0, AC_OFFSET_TYPE(1), AC_OFFSET_TYPE(1), m0.xsize() - 2, m0.ysize() - 2);
	        int i;
	        ptime start;
	        for (i=0;i<n;i++)
	        {
		    fill(m.begin(), m.end(), mat_value_type());
	        }
	        ptime stop;
            double duration = ptime(stop-start).seconds(); 

	        cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	        cerr << "fillrate sub:" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	        cerr << "fillrate sub:" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
	    }

	    {
	        // Performance
	        int n = 10;
	        mat m(1280,1024);
	        int i;
	        ptime start;
	        for (i=0;i<n;i++)
	        {
		    fill(m.begin(1, 1, 1278, 1022), m.end(AC_OFFSET_TYPE(1), AC_OFFSET_TYPE(1), 1278, 1022), mat_value_type());
	        }
	        ptime stop;
            double duration = ptime(stop-start).seconds(); 

	        cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	        cerr << "fillrate sub sub:" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	        cerr << "fillrate sub sub:" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
	    }

	    {
	        // Performance
	        int n = 10;
	        mat m0(1280, 1024);
	        subraster<mat_value_type> m(m0, AC_OFFSET_TYPE(1), AC_OFFSET_TYPE(1), m0.xsize() - 2, m0.ysize() - 2);
	        int i;
	        ptime start;
	        for (i=0;i<n;i++)
	        {
		    int x, y;
		    mat_value_type v(0);
		    for (y = 0; y < m.ysize(); y++) {
		        for (x = 0; x < m.xsize(); x++) {
			    m(x, y) = v;
		        }
		    }
	        }
	        ptime stop;
            double duration = ptime(stop-start).seconds(); 

	        cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	        cerr << "fillrate sub indexed vertical:" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	        cerr << "fillrate sub indexed vertical:" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
	    }

	    {
	        // Performance
	        int n = 1000;
	        mat m0(1280, 1024);
	        subraster<mat_value_type> m(m0, AC_OFFSET_TYPE(1), AC_OFFSET_TYPE(1), AC_OFFSET_TYPE(1), m0.ysize() - 2);
	        int i;
	        ptime start;
	        for (i=0;i<n;i++)
	        {
		    fill(m.begin(), m.end(), mat_value_type());
	        }
	        ptime stop;
            double duration = ptime(stop-start).seconds(); 

	        cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	        cerr << "fillrate sub vertical:" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	        cerr << "fillrate sub vertical:" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
	    }

	    {
	        // Performance
	        int n = 10;
	        mat m(1280, 1024);
	        int i;
	        ptime start;
	        for (i=0;i<n;i++)
	        {
		    int j = m.size();
		    mat_iterator it = m.begin();
		    mat_value_type pix;
		    while (j--) *it++ = pix;
	        }
	        ptime stop;
            double duration = ptime(stop-start).seconds(); 

	        cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	        cerr << "fillrate explicit:" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	        cerr << "fillrate explicit:" << setprecision(4) << (m.size()*n/duration*sizeof(mat_value_type)/1024/1024) << " MB/sec" << endl; 
	    }

    }
};

void scale_perf(char* testname)
{
    cerr << "starting scale_perf" << endl; 
   {
	    typedef raster<RGB> mat;
	    //cerr << "starting scale2d RGB=" << endl; 
	    int n = 2;
	    raster<RGB> m(1280, 1024);
	    raster<RGB> zm(1280/2, 1024 / 2);
//	    int i;
	    ptime start;
	    scale2d(m, zm);
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "scale2d 1/2 explicit RGB rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "scale2d 1/2 explicit RGB rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
   {
	    typedef raster<RGB> mat;
	    //cerr << "starting halfsize RGB=" << endl; 
	    int n = 2;
	    raster<RGB> m(1280, 1024);
	    raster<RGB> zm(1280/2, 1024 / 2);
//	    int i;
	    ptime start;
	    halfsize(m, zm, RGB_t<int>());
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "halfsize RGB rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "halfsize RGB rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
    {
	    typedef raster<char> mat;
	    //cerr << "starting scale_down RGB=" << endl; 
	    int n = 2;
	    raster<char> m(1280, 1024);
	    raster<char> zm(1280/2, 1024 / 2);
//	    int i;
	    ptime start;
	    scale2d(m, zm);
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "scale2d 1/2 explicit char rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "scale2d 1/2 explicit char rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
	{
	    typedef raster<RGB> mat;
	    cerr << "starting scale2d RGB=" << endl; 
	    int n = 2;
	    raster<RGB> m(1280, 1024);
	    raster<RGB> zm(600, 500);
//	    int i;
	    ptime start;
	    scale2d(m, zm);
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "scale2d 600x500 explicit RGB rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "scale2d 600x500 explicit RGB rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
    {
	    typedef raster<RGB> mat;
	    //cerr << "starting scale_down RGB=" << endl; 
	    int n = 2;
	    raster<RGB> m(1280, 1024);
	    raster<RGB> zm(1280/2, 1024 / 2);
//	    int i;
	    ptime start;
	    scale_down(m, zm, RGB_t<int>(0));
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "scale_down 1/2 explicit RGB rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "scale_down 1/2 explicit RGB rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
	{
	    typedef raster<char> mat;
	    //cerr << "starting scale_down RGB=" << endl; 
	    int n = 2;
	    raster<char> m(1280, 1024);
	    raster<char> zm(1280/2, 1024 / 2);
//	    int i;
	    ptime start;
	    scale_down(m, zm, int(0));
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "scale_down 1/2 explicit char rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "scale_down 1/2 explicit char rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
	{
	    typedef raster<RGB> mat;
	    cerr << "starting scale_down RGB=" << endl; 
	    int n = 2;
	    raster<RGB> m(1280, 1024);
	    raster<RGB> zm(600, 500);
//	    int i;
	    ptime start;
	    scale_down(m, zm, RGB_t<int>(0));
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "scale_down 600x500 explicit RGB rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "scale_down 600x500 explicit RGB rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
	{
	    typedef raster<RGB> mat;
	    cerr << "starting resample RGB=" << endl; 
	    int n = 2;
	    raster<RGB> m(1280, 1024);
	    raster<RGB> zm(600, 500);
//	    int i;
	    ptime start;
//	    resample(zm, m, RGB_t<float>(0));
	    resample(zm, m, RGB_t<int>(0,0,0));
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "resample 600x500 explicit RGB rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "resample 600x500 explicit RGB rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
	{
	    typedef raster<int> mat;
	    cerr << "starting resample int" << endl; 
	    int n = 2;
	    raster<int> m(1280, 1024);
	    raster<int> zm(600, 500);
//	    int i;
	    ptime start;
	    resample(zm, m, int(0));
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "resample 600x500 explicit int rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "resample 600x500 explicit int rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
	{
	    typedef raster<char> mat;
	    cerr << "starting resample char" << endl; 
	    int n = 2;
	    mat m(1280, 1024);
	    mat zm(600, 500);
//	    int i;
	    ptime start;
	    resample(zm, m, char(0));
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "resample 600x500 explicit char rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "resample 600x500 explicit char rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
	{
	    typedef raster<float> mat;
	    cerr << "starting resample float" << endl; 
	    int n = 2;
	    raster<float> m(1280, 1024);
	    raster<float> zm(600, 500);
//	    int i;
	    ptime start;
	    resample(zm, m, float(0));
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "resample 600x500 explicit float rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "resample 600x500 explicit float rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
}

void algo_perf(char* testname)
{
    cerr << "**** algoperf: " << testname << endl;

	{
        RGB pix1(1,2,3);
        RGB pix2(2,3,4);
        RGB pix3 = pix1 * pix2;

	    typedef raster<RGB> mat;
	    // Performance
	    int n = 100;
	    mat m1(128, 128);
	    mat m2(128, 128);
	    mat m(128, 128);
	    int i;
	    ptime start;
	    for (i=0;i<n;i++)
	    {
		    transform(m1.begin(), m1.end(), m2.begin(), m.begin(), multiplies<RGB>());
	    }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "multrate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "multrate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
#ifdef ROUTE
	{
	    typedef raster<RGB> mat;
	    // Performance
	    int n = 100;
	    mat m1(128, 128);
	    mat m2(128, 128);
	    mat m(128, 128);
	    int i;
	    ptime start;
	    for (i=0;i<n;i++)
	    {
		transform(m1.begin(), m1.end(), m.begin(),
			    output3<RGB, RGB,   route<RED, GREEN>,
						route<BLUE, RED>,
						route<GREEN, BLUE> >());
	    }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "route rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "route rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
 #endif
	{
	    typedef raster<RGB> mat;
	    // Performance
	    int n = 3;
	    raster<RGB> m(1280, 1024);
	    raster<RGBA> m2(1280, 1024);
	    int i;
	    ptime start;
	    for (i=0;i<n;i++)
	    {
		    transform(m.begin(), m.end(), m2.begin(), make_rgb<RGB, RGBA>());
	    }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "RGB->RGBA transform rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "RGB->RGBA transform rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
#ifdef __sgi
	{
	    typedef raster<RGB> mat;
	    // Performance
	    int n = 10;
	    raster<RGB> m(1280, 1024);
	    raster<RGBA> m2(1280, 1024);
	    int i;
	    ptime start;
	    for (i=0;i<n;i++)
	    {
		RGB_to_RGBA_ii((char*)m.begin(), (char*)m.end(), (int*)m2.begin());
	    }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "RGB_to_RGBA_ii max rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "RGB_to_RGBA_ii max rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}

	{
	    typedef raster<RGB> mat;
	    // Performance
	    int n = 10;
	    raster<RGB> m(1280, 1024);
	    raster<RGBA> m2(1280, 1024);
	    int i;
	    ptime start;
	    for (i=0;i<n;i++)
	    {
		RGB_to_RGBA_li((char*)m.begin(), (char*)m.end(), (int*)m2.begin());
	    }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "RGB_to_RGBA_li max rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "RGB_to_RGBA_li max rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}

	{
	    typedef raster<RGB> mat;
	    // Performance
	    int n = 10;
	    raster<RGB> m(1280, 1024);
	    raster<RGBA> m2(1280, 1024);
	    int i;
	    ptime start;
	    for (i=0;i<n;i++)
	    {
		RGB_to_RGBA_ll((char*)m.begin(), (char*)m.end(), (int*)m2.begin());
	    }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "RGB_to_RGBA_ll max rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "RGB_to_RGBA_ll max rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
#endif
#ifdef __sgi
	{
	    typedef raster<RGB> mat;
	    // Performance
	    int n = 10;
	    raster<RGB> m(1280, 1024);
	    raster<RGBA> m2(1280, 1024);
	    int i;
	    ptime start;
	    for (i=0;i<n;i++)
	    {
		RGB_to_RGBA_bi((char*)m.begin(), (char*)m.end(), (int*)m2.begin());
	    }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "RGB_to_RGBA_bi max rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "RGB_to_RGBA_bi max rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
 #endif
 
 #ifdef __sgi
	{
	    typedef raster<RGB> mat;
	    // Performance
	    int n = 10;
	    raster<RGB> m(1280, 1024);
	    raster<RGBA> m2(1280, 1024);
	    int i;
	    ptime start;
	    for (i=0;i<n;i++)
	    {
		RGB_to_RGBA(m.begin(), m.end(), m2.begin());
	    }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "RGB_to_RGBA max rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "RGB_to_RGBA max rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
#endif

#if 0
	if (0) {
	    typedef raster<RGB> mat;
	    // Performance
	    int n = 2;
	    mat m2(1280, 1024);
	    mat m(1280*2, 1024 * 2);
	    int i;
	    ptime start;
	    for (i=0;i<n;i++)
	    {
		scale_up(m2.hbegin(), m2.vbegin(),
                 m2.hsize(),m2.vsize(), 
		  	     m.hbegin(), m.vbegin(),
                 m.hsize(), m.vsize());
	    }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "scale RGB rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "scale RGB rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}

	if (0) {
	    typedef raster<int> mat;
	    // Performance
	    int n = 2;
	    raster<int> m2(1280, 1024);
	    raster<int> m(1280*2, 1024 * 2);
	    int i;
	    ptime start;
	    for (i=0;i<n;i++)
	    {
		scale_up(m2.hbegin(), m2.vbegin(), m2.hsize(),m2.vsize(), 
			m.hbegin(), m.vbegin(), m.hsize(), m.vsize());
	    }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "scale int rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "scale int rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
#endif
 	{
	    typedef raster<int> mat;
	    // Performance
	    int n = 100000;
	    raster<int> m(1280, 1024);
	    int i;
	    raster<int>::iterator begin = m.find(60, 60);
	    ptime start;
	    for (i=0;i<n;i++)
	    {
		    bresenham_fill(begin, m.stride(), 10, 10, 1);
		    
		    bresenham_fill(begin, m.stride(), 10, 0, 2);
		    
		    bresenham_fill(begin, m.stride(), 10, -10, 3);
		    
		    bresenham_fill(begin, m.stride(), -10, 0, 4);
		    
		    bresenham_fill(begin, m.stride(), -10, 10, 5);
		    
		    bresenham_fill(begin, m.stride(), -10, -10, 6);
		    
		    bresenham_fill(begin, m.stride(), 0, -10, 7);
		    
		    bresenham_fill(begin, m.stride(), 0, 10, 8);
		    
	    }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "bresenham_fill int rate 1: " << setprecision(4) << (80*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "bresenham_fill int rate 1: " << setprecision(4) << (80*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	    cerr << "bresenham_fill int rate 1: " << setprecision(4) << (8*n/duration/1024) << " k 10-pix-lines/sec" << endl; 
	}
  	{
	    typedef raster<int> mat;
	    // Performance
//	    int n = 100000;
	    raster<int> m(1280, 1024);
	    subraster<int> s(m, AC_OFFSET_TYPE(10), AC_OFFSET_TYPE(10), m.xsize()-20,  m.ysize()-20);
//	    int i;
	    ptime start;
	    subraster<int>::iterator it = s.begin();
	    raster<int>::iterator begin;
	    
        while (it != s.end())
        {
            begin = it.ptr();
            bresenham_fill(begin, m.stride(), 10, 10, 1);

            bresenham_fill(begin, m.stride(), 10, 0, 2);

            bresenham_fill(begin, m.stride(), 10, -10, 3);

            bresenham_fill(begin, m.stride(), -10, 0, 4);

            bresenham_fill(begin, m.stride(), -10, 10, 5);

            bresenham_fill(begin, m.stride(), -10, -10, 6);

            bresenham_fill(begin, m.stride(), 0, -10, 7);

            bresenham_fill(begin, m.stride(), 0, 10, 8);
            ++it;
        }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "bresenham_fill int rate 2: " << setprecision(4) << (80*s.size()/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "bresenham_fill int rate 2: " << setprecision(4) << (80*s.size()/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	    cerr << "bresenham_fill int rate 2: " << setprecision(4) << (8*s.size()/duration/1024) << " k 10-pix-lines/sec" << endl; 
	}
   	{
	    typedef raster<int> mat;
	    // Performance
//	    int n = 100000;
	    raster<int> m(1280, 1024);
	    subraster<int> s(m, AC_OFFSET_TYPE(100), AC_OFFSET_TYPE(100), m.xsize()-700, m.ysize() - 700);
//	    int i;
	    ptime start;
	    subraster<int>::iterator it = s.begin();
	    raster<int>::iterator begin;
	    
        while (it != s.end())
        {
            begin = it.ptr();
            bresenham_fill(begin, m.stride(), 100, 100, 1);

            bresenham_fill(begin, m.stride(), 100, 0, 2);

            bresenham_fill(begin, m.stride(), 100, -100, 3);

            bresenham_fill(begin, m.stride(), -100, 0, 4);

            bresenham_fill(begin, m.stride(), -100, 100, 5);

            bresenham_fill(begin, m.stride(), -100, -100, 6);

            bresenham_fill(begin, m.stride(), 0, -100, 7);

            bresenham_fill(begin, m.stride(), 0, 100, 8);
            ++it;
        }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "bresenham_fill int rate 3: " << setprecision(4) << (800*s.size()/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "bresenham_fill int rate 3: " << setprecision(4) << (800*s.size()/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	    cerr << "bresenham_fill int rate 3: " << setprecision(4) << (8*s.size()/duration/1024) << " k 100-pix-lines/sec" << endl; 
	}
  	{
	    typedef raster<RGB> mat;
	    // Performance
//	    int n = 100000;
	    raster<RGB> m(1280, 1024);
	    subraster<RGB> s(m, AC_OFFSET_TYPE(10), AC_OFFSET_TYPE(10), m.xsize()-20, m.ysize()-20);
//	    int i;
	    ptime start;
	    subraster<RGB>::iterator it = s.begin();
	    raster<RGB>::iterator begin;
	    RGB val(0, 0, 0);
        while (it != s.end())
        {
            begin = it.ptr();
            bresenham_fill(begin, m.stride(), 10, 10, val);

            bresenham_fill(begin, m.stride(), 10, 0, val);

            bresenham_fill(begin, m.stride(), 10, -10, val);

            bresenham_fill(begin, m.stride(), -10, 0, val);

            bresenham_fill(begin, m.stride(), -10, 10, val);

            bresenham_fill(begin, m.stride(), -10, -10, val);

            bresenham_fill(begin, m.stride(), 0, -10, val);

            bresenham_fill(begin, m.stride(), 0, 10, val);
            ++it;
        }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "bresenham_fill RGB rate 4: " << setprecision(4) << (80*s.size()/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "bresenham_fill RGB rate 4: " << setprecision(4) << (80*s.size()/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	    cerr << "bresenham_fill RGB rate 4: " << setprecision(4) << (8*s.size()/duration/1024) << " k 10-pix-lines/sec" << endl; 
	}
 	{
		    // Performance
	    typedef raster<char> mat;
//	    int n = 100000;
	    raster<char> m(1280, 1024);
	    subraster<char> s(m, AC_OFFSET_TYPE(10), AC_OFFSET_TYPE(10), m.xsize()-20, m.ysize()-20);
//	    int i;
	    ptime start;
	    subraster<char>::iterator it = s.begin();
	    raster<char>::iterator begin;
	    char val(1);
        while (it != s.end())
        {
            begin = it.ptr();
            bresenham_fill(begin, m.stride(), 10, 10, val);

            bresenham_fill(begin, m.stride(), 10, 0, val);

            bresenham_fill(begin, m.stride(), 10, -10, val);

            bresenham_fill(begin, m.stride(), -10, 0, val);

            bresenham_fill(begin, m.stride(), -10, 10, val);

            bresenham_fill(begin, m.stride(), -10, -10, val);

            bresenham_fill(begin, m.stride(), 0, -10, val);

            bresenham_fill(begin, m.stride(), 0, 10, val);
            ++it;
        }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "bresenham_fill char rate 5: " << setprecision(4) << (80*s.size()/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "bresenham_fill char rate 5: " << setprecision(4) << (80*s.size()/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	    cerr << "bresenham_fill char rate 5: " << setprecision(4) << (8*s.size()/duration/1024) << " k 10-pix-lines/sec" << endl; 
	}
#ifdef __sgi 
	{
	    typedef raster<RGB> mat;
	    // Performance
	    int n = 10;
	    raster<RGB> m0(1280, 1024);
	    subraster<RGB> m(m0, 1, 1, m0.xsize() - 2, m0.ysize() - 2);
	    raster<RGBA> m2(m.xsize(), m.ysize());
	    int i;
	    ptime start;
	    for (i=0;i<n;i++)
	    {
		RGB_to_RGBA(m.begin(), m.end(), m2.begin());
	    }
	    ptime stop; double duration = ptime(stop-start).seconds();
	    
	    cerr << "Size = " << m.xsize() << ' ' << m.ysize() << endl;
	    cerr << "RGB_to_RGBA sub rate :" << setprecision(4) << (m.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
	    cerr << "RGB_to_RGBA sub rate :" << setprecision(4) << (m.size()*n/duration*sizeof(mat::value_type)/1024/1024) << " MB/sec" << endl; 
	}
#endif
}

template <class v>
class ref_perf {
public:
    typedef typename v::value_type v_value_type;
    ref_perf(char* testname)
    {
    
        {
            // Performance
            int n = 1000;
            v x(128, 128);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                fill(x.begin(), x.end(), v_value_type());
            }
    	    ptime stop; double duration = ptime(stop-start).seconds();

            cerr << "Size = " << x.size() << endl;
            cerr << "vec fillrate :" << setprecision(4) << (x.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "vec fillrate :" << setprecision(4) << (x.size()*n/duration*sizeof(v_value_type)/1024/1024) << " MB/sec" << endl; 
        }
        {
            // Performance
            int n = 1000;
            v x(128, 128);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                fill(x.begin(), x.end(), v_value_type());
            }
    	    ptime stop; double duration = ptime(stop-start).seconds();

            cerr << "Size = " << x.size() << endl;
            cerr << "vec mem fillrate :" << setprecision(4) << (x.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "vec mem fillrate :" << setprecision(4) << (x.size()*n/duration*sizeof(v_value_type)/1024/1024) << " MB/sec" << endl; 
        }
        {
            // Performance
            int n = 100;
            v x(1280, 1024);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                fill(x.begin(), x.end(), v_value_type());
            }
    	    ptime stop; double duration = ptime(stop-start).seconds();

            cerr << "Size = " << x.size() << endl;
            cerr << "vec fillrate :" << setprecision(4) << (x.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "vec fillrate :" << setprecision(4) << (x.size()*n/duration*sizeof(v_value_type)/1024/1024) << " MB/sec" << endl; 
        }
        {
            // Performance
            int n = 100;
            v x(1280, 1024);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                fill(x.begin(), x.end(), v_value_type());
            }
    	    ptime stop; double duration = ptime(stop-start).seconds();

            cerr << "Size = " << x.size() << endl;
            cerr << "vec mem fillrate :" << setprecision(4) << (x.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "vec mem fillrate :" << setprecision(4) << (x.size()*n/duration*sizeof(v_value_type)/1024/1024) << " MB/sec" << endl; 
        }
        {
            // Performance
            int n = 50;
            v x(1280, 1024);
            v x1(1280, 1024);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                copy(x.begin(), x.end(), x1.begin());
            }
    	    ptime stop; double duration = ptime(stop-start).seconds();

            cerr << "Size = " << x.size() << endl;
            cerr << "vec copy rate :" << setprecision(4) << (x.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "vec copy rate :" << setprecision(4) << (x.size()*n/duration*sizeof(v_value_type)/1024/1024) << " MB/sec" << endl; 
        }
        {
            // Performance
            int n = 50;
            v x(1280, 1024);
            v x1(1280, 1024);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                copy((char*)x.begin(), (char*)x.end(), (char*)x1.begin());
            }
    	    ptime stop; double duration = ptime(stop-start).seconds();

            cerr << "Size = " << x.size() << endl;
            cerr << "vec copy rate :" << setprecision(4) << (x.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "vec copy rate :" << setprecision(4) << (x.size()*n/duration*sizeof(v_value_type)/1024/1024) << " MB/sec" << endl; 
        }
        {
            // Performance
            int n = 10;
            v x(1280, 1024);
            v x1(1280, 1024);
            v x2(1280, 1024);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                transform(x.begin(), x.end(), x1.begin(), x2.begin(), multiplies<v_value_type>());
            }
    	    ptime stop; double duration = ptime(stop-start).seconds();

            cerr << "Size = " << x.size() << endl;
            cerr << "vec transform mult rate :" << setprecision(4) << (x.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "vec transform mult rate :" << setprecision(4) << (x.size()*n/duration*sizeof(v_value_type)/1024/1024) << " MB/sec" << endl; 
        }
        {
            // Performance
            int n = 20;
            v x(1280, 1024);
            v x1(1280, 1024);
            v x2(1280, 1024);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                transform(x.begin(), x.end(), x2.begin(), invert<v_value_type, v_value_type>());
            }
    	    ptime stop; double duration = ptime(stop-start).seconds();

            cerr << "Size = " << x.size() << endl;
            cerr << "vec transform invert rate :" << setprecision(4) << (x.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "vec transform invert rate :" << setprecision(4) << (x.size()*n/duration*sizeof(v_value_type)/1024/1024) << " MB/sec" << endl; 
        }
#ifdef __sgi
        {
            // Performance
            int n = 50;
            v x(1280, 1024);
            v x1(1280, 1024);
            v x2(1280, 1024);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                transform64((char*)(&(*(x.begin())))+1, (char*)(&(*(x.end()))), (char*)(&(*(x2.begin())))+1,
                    invert<char, char>(), invert<long long, long long>());
            }
    	    ptime stop; double duration = ptime(stop-start).seconds();

            cerr << "Size = " << x.size() << endl;
            cerr << "vec unaligned transform64 invert rate :" << setprecision(4) << (x.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "vec unaligned transform64 invert rate :" << setprecision(4) << (x.size()*n/duration*sizeof(v_value_type)/1024/1024) << " MB/sec" << endl; 
        }
        {
            // Performance
            int n = 50;
            v x(1280, 1024);
            v x1(1280, 1024);
            v x2(1280, 1024);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                transform64((char*)(&(*(x.begin()))), (char*)(&(*(x.end()))), (char*)(&(*(x2.begin()))),
                    invert<char, char>(), invert<long long, long long>());
            }
    	    ptime stop; double duration = ptime(stop-start).seconds();

            cerr << "Size = " << x.size() << endl;
            cerr << "vec transform64 invert rate :" << setprecision(4) << (x.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "vec transform64 invert rate :" << setprecision(4) << (x.size()*n/duration*sizeof(v_value_type)/1024/1024) << " MB/sec" << endl; 
        }
#endif
        {
            // Performance
            int n = 20;
            v x(1280, 1024);
            v x1(1280, 1024);
            v x2(1280, 1024);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                transform(x.begin(), x.end(), x1.begin(), x2.begin(), plus<v_value_type>());
            }
    	    ptime stop; double duration = ptime(stop-start).seconds();

            cerr << "Size = " << x.size() << endl;
            cerr << "vec transform add rate :" << setprecision(4) << (x.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "vec transform add rate :" << setprecision(4) << (x.size()*n/duration*sizeof(v_value_type)/1024/1024) << " MB/sec" << endl; 
        }
        {
            // Performance
            int n = 10;
            v x(1280, 1024);
            v x1(1280, 1024);
            v x2(1280, 1024);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                v_value_type* src1 = x.begin();
                v_value_type* src2 = x1.begin();
                v_value_type* dest = x2.begin();
                v_value_type* end = x.end();
                while (src1 < end) *dest++ = *src1++ * *src2++;
            }
    	    ptime stop; double duration = ptime(stop-start).seconds();

            cerr << "Size = " << x.size() << endl;
            cerr << "vec explicit mult rate :" << setprecision(4) << (x.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "vec explicit mult rate :" << setprecision(4) << (x.size()*n/duration*sizeof(v_value_type)/1024/1024) << " MB/sec" << endl; 
        }
        {
            // Performance
            int n = 10;
            v x(1280, 1024);
            v x1(1280, 1024);
            v x2(1280, 1024);
            int i;
            ptime start;
            for (i=0;i<n;i++)
            {
                v_value_type* src1 = x.begin();
                v_value_type* src2 = x1.begin();
                v_value_type* dest = x2.begin();
                v_value_type* end = x.end();
                while (src1 < end) *dest++ = *src1++ + *src2++;
            }
    	    ptime stop; double duration = ptime(stop-start).seconds();

            cerr << "Size = " << x.size() << endl;
            cerr << "vec explicit add rate :" << setprecision(4) << (x.size()*n/duration/1024/1024) << " Mpixels/sec" << endl; 
            cerr << "vec explicit add rate :" << setprecision(4) << (x.size()*n/duration*sizeof(v_value_type)/1024/1024) << " MB/sec" << endl; 
        }

    }
};


class PerformanceTest : public UnitTest {
public:
    PerformanceTest() : UnitTest("PerformanceTest") {  }
    void run() {
	    scale_perf("scale tests");
    	iter_perf<raster<RGB> > test1("raster<RGB>");
	    iter_perf<raster<int> > test2("raster<int>");
    	ref_perf<raster<RGB> > rtest1("raster<RGB>");
	    ref_perf<raster<int> > rtest2("raster<int>");
	    algo_perf("algo");
        SUCCESS("all performance test executed");
    }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        
        addTest(new Scale2dTest);
#define ALL_TESTS
#ifdef ALL_TESTS
        
#ifndef DEBUG_VARIANT        
        addTest(new PerformanceTest);
#endif        
        addTest(new FractionIteratorTest1);
        addTest(new FractionIteratorTest2);
        addTest(new FractionIteratorTest3);
        addTest(new BresenhamIteratorTest1);
        addTest(new RasterTest1<raster<char>,char>("raster<char>"));
        addTest(new RasterTest1<subraster<char>,char>("subraster<char>"));
        //addTest(new RasterTest1<raster,RGB> >("<raster>"));
#endif
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
