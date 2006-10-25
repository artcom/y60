//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: jscpp.h,v $
//   $Author: martin $
//   $Revision: 1.2 $
//   $Date: 2004/10/04 13:36:42 $
//
//
//=============================================================================

#ifndef __Y60_ACXPSHELL_JSCPP_INCLUDED__
#define __Y60_ACXPSHELL_JSCPP_INCLUDED__

#include <js/jsapi.h>

namespace jslib {

extern bool initCppClasses(JSContext *cx, JSObject *theGlobalObject);

}

#endif
