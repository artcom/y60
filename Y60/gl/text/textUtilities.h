//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: textUtilities.h,v $
//   $Author: janbo $
//   $Revision: 1.2 $
//   $Date: 2004/10/18 16:22:56 $
//
//  Description: This class performs texture loading and management.
//
//=============================================================================

#ifndef _ac_render_textUtils_h_
#define _ac_render_textUtils_h_

#include "BitFont.h"
#include "Screen15.h"

#include <string>

namespace y60 {

    void glDrawString(const char * myText, const BitFont & font = Screen15);

}

#endif // _ac_render_textUtils_h_

