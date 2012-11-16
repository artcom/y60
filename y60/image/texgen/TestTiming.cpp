/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//    $RCSfile: TestTiming.cpp,v $
//
//     $Author: janbo $
//
//   $Revision: 1.3 $
//
//
// Description:
//
//=============================================================================

#include "TestTiming.h"

#include <stdexcept>
#include <fstream>
#include <math.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plbitmap.h>
#include <paintlib/planybmp.h>
#include <paintlib/plpixel32.h>
#include <paintlib/plpoint.h>
#include <paintlib/plrect.h>
#include <paintlib/planydec.h>
#include <paintlib/plpngenc.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include <asl/dom/Nodes.h>
#include <asl/base/string_functions.h>
#include <asl/base/Time.h>

#include "TextureGenerator.h"

using namespace TexGen;
using namespace dom;
using namespace std;
using namespace asl;

//#define CREATE_TESTCASES

string myDirectory="../../TestImages/";

string myXMLFileName=myDirectory+"timingtest.xml";

void TestTiming::run() {
    try {

        Node theTexDef;
        ifstream theXMLFile (myXMLFileName.c_str());
        if (!theXMLFile) {
            ENSURE_MSG (false, ("Can't open " +myXMLFileName).c_str());
        }

        theXMLFile >> theTexDef;
        if (!theTexDef) {
            ENSURE_MSG (false, ("Can't parse " +myXMLFileName).c_str());
        }

#ifdef CREATE_TESTCASES
        // save indexMap
        PLPNGEncoder theEncoder;
        PLAnyBmp indexMap;

        indexMap.Create(8024,8024,8,false);
        int counter = 0;
        for (int y = 0;y<8024;y++) {
            for (int x = 0;x<8024;x++) {
                indexMap.GetLineArray()[y][x] = counter++;
                counter = counter % 4;
            }
        }
        theEncoder.MakeFileFromBmp((myDirectory+"timingTest_index.png").c_str(),&indexMap );
        exit(1);
#endif

        TextureGenerator myTexGen (&theTexDef);

        cout << "Generating textures (target size is 64x64)..." << endl;
        PLPoint myTargetSize (64, 64);
        double TotalTime = 0;
        for (int i=0; i<10; i++)  {
            int myIndexSize = int(pow(2.0,i));
            cout << "Testing index bmp size: " << myIndexSize << endl;
            myIndexSize *=8;
            Time StartTime;
            int myIndexOffset = 0;
//            if (i<9) {
//                myIndexOffset = 4096*8;
//            }
            for (int j=0; j<15; j++) {
//             for (int j=0; j<16; j++) {
                int topleft = myIndexOffset+myIndexSize*j;
                int bottomright = myIndexOffset+myIndexSize*(j+1);
                PLRect myIndexRect (topleft, topleft, bottomright, bottomright);
                PLAnyBmp myResultBmp;
                myTexGen.createTexture (myIndexRect, myTargetSize, myResultBmp);
            }
            Time EndTime;
            double TimeDiff = EndTime-StartTime;
            TotalTime += TimeDiff;
            cout << "  Time: " << TimeDiff << " seconds." << endl;
        }
        cout << "Total time: " << TotalTime << " seconds." << endl;
    } catch (const std::exception& e) {
        cout <<"Exception occurred :"<<e.what()<<endl;
    } catch (const PLTextException& e) {
        cout <<"Paintlib exception occurred :"<<(const char *)e<<endl;
    }
    ENSURE (true);

}


