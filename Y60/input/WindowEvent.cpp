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
//   $RCSfile: WindowEvent.cpp,v $
//   $Author: janbo $
//   $Revision: 1.2 $
//   $Date: 2004/10/18 16:22:57 $
//
//
//=============================================================================

#include "WindowEvent.h"

namespace y60 {

    WindowEvent::WindowEvent(Type theEventType,
                      int theWidth,
                      int theHeight) 
        : Event(theEventType), isFullScreen(false),
          width(theWidth), height(theHeight)
    {}

    WindowEvent::WindowEvent(Type theEventType,
                      bool theFullScreenFlag) 
        : Event(theEventType), isFullScreen(theFullScreenFlag),
          width(-1), height(-1)
    {}

    WindowEvent::~WindowEvent() {
    }

}

