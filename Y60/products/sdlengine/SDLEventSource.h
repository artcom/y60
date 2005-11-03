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
//   $RCSfile: SDLEventSource.h,v $
//   $Author: janbo $
//   $Revision: 1.2 $
//   $Date: 2004/10/18 16:22:58 $
//
//
//=============================================================================
#ifndef _Y60_INPUT_SDLEVENTSOURCE_INCLUDED_
#define _Y60_INPUT_SDLEVENTSOURCE_INCLUDED_

#include <y60/Event.h>
#include <y60/IEventSource.h>
#include <y60/KeyCodes.h>

#include <SDL/SDL.h>
#include <asl/Ptr.h>

#include <vector>


class SDLEventSource : public y60::IEventSource {
    public:
        SDLEventSource();
        virtual void init();
        virtual std::vector<y60::EventPtr> poll();

    private:
        static std::vector<y60::KeyCode> myKeyCodeTranslationTable;
        y60::EventPtr createMouseMotionEvent(y60::Event::Type theType, const SDL_Event & theSDLEvent);
        y60::EventPtr createMouseButtonEvent(y60::Event::Type theType, const SDL_Event & theSDLEvent);
        y60::EventPtr createMouseWheelEvent(y60::Event::Type theType, const SDL_Event & theSDLEvent);
        y60::EventPtr createAxisEvent(const SDL_Event & theSDLEvent);
        y60::EventPtr createButtonEvent(y60::Event::Type theType, const SDL_Event & theSDLEvent);
        y60::EventPtr createKeyEvent(y60::Event::Type theType, const SDL_Event & theSDLEvent);
        void initJoysticks();
        void initTranslationTable();
};

#endif
