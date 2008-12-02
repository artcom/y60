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
//    $RCSfile: TestTerrainTexGen.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description: 
//
//=============================================================================

#if !defined(INCL_TESTFILTERTEXGEN)
#define INCL_TESTFILTERTEXGEN

#include <vector>

#include <asl/base/UnitTest.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/planybmp.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include "TerrainTexGen.h"
#include "TextureDefinition.h"

class TestTerrainTexGen: public UnitTest {
public:
    explicit TestTerrainTexGen (): UnitTest ("TestTerrainTexGen") {}
    virtual void run();

private:
    TexGen::TextureDefinitionMap * createColorDefinitions ();
    TexGen::TextureDefinitionMap * createBmpDefinitions ();
    void runTestsWithOneSize 
        (const TexGen::TerrainTexGen& theTerrainTexGen,
         PLBmp * theIndexBmp,
         const std::string& theResultFileName,
         bool runPartialTest,
         const PLPoint& theResultSize);
    void runTestsWithIndexBmp
        (float myBlendRadius, 
         const std::string & theIndexFileName,
         const std::string & theBigResultFileName,
         const std::string & theSmallResultFileName,
         bool runPartialTest,
         bool useColorDefinition);

};


#endif

