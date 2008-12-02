//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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


