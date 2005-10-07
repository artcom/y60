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
//   $RCSfile: JSdirectory_functions.h,v $
//   $Author: david $
//   $Revision: 1.1 $
//   $Date: 2005/01/27 14:29:04 $
//
//   Description: Base class wrapper for AbstractRenderWindow
//   TODO: this is not (yet) used by the SDL-based app
//
//=============================================================================

#ifndef __y60_jslib_jsfile_functions_h_included_
#define __y60_jslib_jsfile_functions_h_included_

#include "JScppUtils.h"

namespace jslib {

struct JSFileFunctions {
    static JSFunctionSpec * Functions();
};

}

#endif
