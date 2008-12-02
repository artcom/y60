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
//    $RCSfile: TestFilterTile.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description: 
//
//=============================================================================

#include "TestFilterTile.h"

#include <typeinfo>
#include <stdexcept>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plbitmap.h>
#include <paintlib/planybmp.h>
#include <paintlib/plpixel32.h>
#include <paintlib/Filter/plfilterfill.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include "FilterTile.h"

using namespace TexGen;
using namespace std;

void TestFilterTile::createTextures () {
    for (int i=0; i<2; ++i) {
        _texBmps.push_back(new PLAnyBmp());
        _texBmps[i]->Create (_texSize, _texSize, PLPixelFormat::X8R8G8B8);
    }
}

void TestFilterTile::setupSimpleTextures () {
    // Eine weiße und eine schwarze Textur generieren.
    createTextures();
    _texBmps[0]->ApplyFilter(PLFilterFill<PLPixel32> (PLPixel32(0,0,0,0)));
    _texBmps[1]->ApplyFilter(PLFilterFill<PLPixel32> (PLPixel32(255,255,255,0)));
}

void TestFilterTile::setupComplexTextures () {
    // Generate two textures with shades of blue and red.
    createTextures();
    for (int i=0; i<2; ++i) {
        PLBYTE ** texLines = _texBmps[i]->GetLineArray();
        for (int y=0; y<_texSize; y++) {
            PLPixel32 * texLine = (PLPixel32*)texLines[y];
            for (int x=0; x<_texSize; x++) {
                if (i==0) {
                    texLine[x] = PLPixel32 (0,0,static_cast<PLBYTE>(x+y),0);
                } else {
                    texLine[x] = PLPixel32 (static_cast<PLBYTE>(x+y),0,0,0);
                }
            }
        }
    }
}

void TestFilterTile::setup(int numSquares, int texSize, bool simple) {
    _numSquares = numSquares;
    _texSize = texSize;
    _indexBmp.Create (numSquares, numSquares, PLPixelFormat::I8);
    PLBYTE ** indexBmpLines = _indexBmp.GetLineArray();

    // Bitmap mit Schachbrettmuster füllen.
    for (int y=0; y<numSquares; ++y) {
        PLBYTE * curLine = indexBmpLines[y];
        for (int x=0; x<numSquares; ++x) {
          curLine[x] = (x+y) % 2;
        }       
    }
    
    if (simple) {
        setupSimpleTextures();
    } else {
        setupComplexTextures();
    }

    _myGenerator = new FilterTile (_texBmps);
}

void TestFilterTile::tearDown () {
    delete _myGenerator;
    delete _texBmps[0];
    delete _texBmps[1];
    _texBmps.clear();
}

void TestFilterTile::runGeneratorTest (int numSquares, int texSize, bool simple) {
    setup (numSquares, texSize, simple);
    cerr << getTracePrefix() << "Testing tile filter with " 
         << _numSquares << " x " << _numSquares << " patches of size "
         << _texSize << " x " << _texSize << "," << endl;
    if (simple) {
        cerr << getTracePrefix() << "simple textures." << endl;
    } else {
        cerr << getTracePrefix() << "complex textures." << endl;
    }

    _myGenerator->Apply (&_indexBmp, &_resultBmp);

    // We should have a large checkboard pattern now...
    const int resultSize = _texSize*_numSquares;
    ENSURE (_resultBmp.GetWidth() == resultSize);

    PLBYTE ** resultBmpLines = _resultBmp.GetLineArray();
    bool ok = true;

    if (simple) {
        PLPixel32 blackPixel = PLPixel32 (0,0,0,0);
        PLPixel32 whitePixel = PLPixel32 (255,255,255,0);
        for (int y=0; y<resultSize; ++y) {
            PLPixel32 * curLine = (PLPixel32*)resultBmpLines[y];
            for (int x=0; x<resultSize; ++x) {
                bool pixel_ok;
                if ((x/_texSize + y/_texSize) % 2 == 0) {
                    pixel_ok = (curLine[x] == blackPixel);
                } else {
                    pixel_ok = (curLine[x] == whitePixel);
                }
                if (!pixel_ok && ok) {
                    ok = false;
                    cerr << getTracePrefix() << "First error at ("
                         << x << ", " << y << ")" << endl;
                }
            }       
        }
    } else {
        for (int y=0; y<resultSize; ++y) {
            PLPixel32 * resultLine = (PLPixel32*)resultBmpLines[y];
            for (int x=0; x<resultSize; ++x) {
                bool pixel_ok;
                int texNum = (x/_texSize+y/_texSize) % 2;
                int patch_x = x % _texSize;
                int patch_y = y % _texSize;
                PLPixel32 * texLine = (PLPixel32*)(_texBmps[texNum]->GetLineArray()[patch_y]);
                pixel_ok = texLine[patch_x] == resultLine[x];
                if (!pixel_ok) {
                    ok = false;
                }
            }
        }
    }        
    ENSURE_MSG (ok, "FilterTile");
    tearDown ();
}

void TestFilterTile::runIndexExceptionTest() {
    PLBYTE ** indexBmpLines = _indexBmp.GetLineArray();
    indexBmpLines[0][0] = 2;
    bool ok = false;
    try {
        _myGenerator->Apply (&_indexBmp, &_resultBmp);
    } catch (const exception& e) {
        cerr << getTracePrefix() << e.what() << endl;
        if (typeid(e) == typeid (out_of_range)) {
            ok = true;
        }
    }
    ENSURE_MSG (ok, "FilterTile index exception handling");
}

void TestFilterTile::runTexSizeExceptionTest() {
    _texBmps[0]->Create (2, 5, PLPixelFormat::X8R8G8B8);
    bool ok = false;
    try {
        _myGenerator->Apply (&_indexBmp, &_resultBmp);
    } catch (const exception& e) {
        cerr << getTracePrefix () << e.what() << endl;
        if (typeid(e) == typeid (invalid_argument)) {
            ok = true;
        }
    }
    ENSURE_MSG (ok, "FilterTile texture size exception handling");
}

void TestFilterTile::run() {
    setup (4,10, true);
    runIndexExceptionTest();
    tearDown();

    setup (4,10, true);
    runTexSizeExceptionTest();
    tearDown();

    runGeneratorTest (4, 10, true);
    runGeneratorTest (10, 4, true);
    runGeneratorTest (4, 10, false);
    runGeneratorTest (10, 4, false);
    if (_run_extended_tests) {
        runGeneratorTest(128, 64, true);    
    }
}


