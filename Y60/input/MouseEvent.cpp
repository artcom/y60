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
//   $RCSfile: MouseEvent.cpp,v $
//   $Author: janbo $
//   $Revision: 1.3 $
//   $Date: 2004/10/18 16:22:57 $
//
//
//=============================================================================

#include "MouseEvent.h"

namespace y60 {

    MouseEvent::MouseEvent(Type theEventType,
                            bool theLeftButtonState,
                            bool theMiddleButtonState,
                            bool theRightButtonState,
                            int theXPosition,
                            int theYPosition,
                            int theXDelta,
                            int theYDelta,
                            Button theButton)
        : Event(theEventType), leftButtonState(theLeftButtonState),
          middleButtonState(theMiddleButtonState), rightButtonState(theRightButtonState),
          xPosition(theXPosition),yPosition(theYPosition),
          xDelta(theXDelta), yDelta(theYDelta), button(theButton)
    {}

    MouseEvent::~MouseEvent() {
    }

}

