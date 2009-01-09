//============================================================================
//
// Copyright (C) 2002, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: Text.cpp,v $
//
//     $Author: janbo $
//
//   $Revision: 1.2 $
//
// Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

//own header
#include "Text.h"

#include "TextRenderer.h"

#include <asl/math/Vector234.h>

using namespace std;
using namespace asl;

namespace y60 {

    Text::Text(TextRenderer *  theRenderer,
               const Vector2f & thePos,
               const asl::Vector4f & theTextColor,
               const string & theString,
               const string theFont)
            : _myPos(thePos),_myRenderer(theRenderer),
              _myString(theString), _myFont(theFont),
              _myTextColor(theTextColor)
    {
    }

    Text::~Text() {
    }
}
