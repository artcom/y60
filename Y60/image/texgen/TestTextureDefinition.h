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
//    $RCSfile: TestTextureDefinition.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description: 
//
//=============================================================================

#if !defined(INCL_TESTTEXTUREDEFINITION)
#define INCL_TESTTEXTUREDEFINITION

#include <asl/base/UnitTest.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/planybmp.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include "TextureDefinition.h"

class TestTextureDefinition: public UnitTest {
public:
    explicit TestTextureDefinition (): UnitTest ("TestTextureDefinition") {}
    virtual void run();

private:
    PLAnyBmp* createBitmap ();
};


#endif


