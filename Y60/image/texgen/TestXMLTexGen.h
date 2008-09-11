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
//    $RCSfile: TestXMLTexGen.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description: 
//
//=============================================================================

#if !defined(INCL_TESTXMLTEXGEN)
#define INCL_TESTXMLTEXGEN

#include "TerrainTexGen.h"
#include "TextureDefinition.h"

#include <asl/base/UnitTest.h>

class TestXMLTexGen: public UnitTest {
public:
    explicit TestXMLTexGen ();
    virtual ~TestXMLTexGen();
    virtual void run();

private:
    TexGen::TextureDefinitionMap * createColorDefinitions ();
    std::string makeXMLTileDef (const std::string & theFileName);
    void runFileNotFoundTest ();
    void runTypeErrorTest ();
    void runArgMissingErrorTest ();
    void runGenerator (const TexGen::TerrainTexGen & theFilter, 
                       const std::string & expectedResultFileName);
    void runNoBlendTest ();
    void runBlendTest ();

    TexGen::TextureDefinitionMap * _myTextureDefinitionMap;
};


#endif


