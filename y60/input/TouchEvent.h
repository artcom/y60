/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//    $RCSfile: TouchEvent.h,v $
//     $Author: ulrich $
//   $Revision: 1.5 $
//       $Date: 2004/10/27 13:52:33 $
//
//=============================================================================

#ifndef _Y60_INPUT_TOCUHEVENT_INCLUDED_
#define _Y60_INPUT_TOUCHEVENT_INCLUDED_

#include "y60_input_settings.h"

#include "Event.h"

#include <asl/math/Vector234.h>
#include <string>

namespace y60 {

    struct Y60_INPUT_DECL TouchEvent : public Event {
        TouchEvent(const std::string & theDevice,
                   const asl::Vector2f & thePosition,
                   const asl::Vector2i & theSize,
                   float theIntensity);
        TouchEvent(const dom::NodePtr & theNode);

        const std::string device;
        const asl::Vector2f position;
        const asl::Vector2i size;
        float intensity;

        virtual EventPtr copy() const {
            return EventPtr(new TouchEvent(*this));
        }

        virtual dom::NodePtr asNode() const;
    };
    typedef asl::Ptr<TouchEvent> TouchEventPtr;
}

#endif

