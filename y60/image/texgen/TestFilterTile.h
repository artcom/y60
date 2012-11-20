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
//    $RCSfile: TestFilterTile.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description:
//
//=============================================================================

#if !defined(INCL_TESTFILTERTILE)
#define INCL_TESTFILTERTILE

#include <vector>

#include <asl/base/UnitTest.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/Filter/plfilter.h>
#include <paintlib/planybmp.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include "FilterTile.h"

class TestFilterTile: public UnitTest {
public:
    explicit TestFilterTile (bool run_extended_tests): UnitTest ("TestFilterTile"),
        _run_extended_tests(run_extended_tests) {}
    virtual void run();

private:
    void setup(int numSquares, int texSize, bool simple);
    void createTextures ();
    void setupSimpleTextures ();
    void setupComplexTextures ();
    void tearDown ();
    void runGeneratorTest (int numSquares, int texSize, bool simple);
    void runIndexExceptionTest();
    void runTexSizeExceptionTest();

    PLAnyBmp _indexBmp;
    PLAnyBmp _resultBmp;
    std::vector<PLBmp*> _texBmps;
    TexGen::FilterTile * _myGenerator;
    int _numSquares;
    int _texSize;
    bool _run_extended_tests;
};


#endif


