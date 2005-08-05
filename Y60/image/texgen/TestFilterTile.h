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

#include "FilterTile.h"

#include <asl/UnitTest.h>

#include <paintlib/Filter/plfilter.h>
#include <paintlib/planybmp.h>

#include <vector>

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


