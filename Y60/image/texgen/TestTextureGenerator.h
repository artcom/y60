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
//    $RCSfile: TestTextureGenerator.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description: 
//
//=============================================================================

#if !defined(INCL_TESTTEXTUREGENERATOR)
#define INCL_TESTTEXTUREGENERATOR

#include <asl/base/UnitTest.h>

#include <string>

class TestTextureGenerator:public UnitTest
{
public:
    TestTextureGenerator() : UnitTest("TestTextureGenerator") {}
    ~TestTextureGenerator() {}
    void run();
private:
    TestTextureGenerator(const TestTextureGenerator&);
    const TestTextureGenerator& operator=(const TestTextureGenerator&);

    // Tests
    void light_withLightmap (const std::string& myLightMapName, 
                             const std::string& myExpectedResultName,
                             bool createPartialTexture); 
};
#endif

