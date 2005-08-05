// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: TouchEvent.h,v $
//     $Author: ulrich $
//   $Revision: 1.5 $
//       $Date: 2004/10/27 13:52:33 $
//
//=============================================================================

#ifndef _Y60_INPUT_TOCUHEVENT_INCLUDED_
#define _Y60_INPUT_TOUCHEVENT_INCLUDED_

#include "Event.h"

#include <asl/Vector234.h>
#include <string>

namespace y60 {

    struct TouchEvent : public Event {
        TouchEvent(const std::string & theDevice,
                   const asl::Vector2f & thePosition,
                   const asl::Vector2i & theSize,
                   float theIntensity) :
            Event(Event::TOUCH),
            device(theDevice),
            position(thePosition),
            size(theSize),
            intensity(theIntensity)
        {}
        const std::string device;
        const asl::Vector2f position;
        const asl::Vector2i size;
        float intensity;
    };
    typedef asl::Ptr<TouchEvent> TouchEventPtr;
}

#endif

