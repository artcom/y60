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
//   $RCSfile: GtkEventAdapter.h,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2004/11/19 15:00:20 $
//
//
//=============================================================================
#ifndef _Y60_INPUT_GTKEVENTADAPTER_INCLUDED_
#define _Y60_INPUT_GTKEVENTADAPTER_INCLUDED_

#include <y60/Event.h>
#include <y60/IEventSource.h>
#include <y60/KeyCodes.h>

#include <gdk/gdkevents.h>
#include <vector>

class GtkEventAdapter : public y60::IEventSource {
    public:
        // GtkEventAdapter();
        // virtual void init();
        virtual std::vector<y60::EventPtr> poll();
        void addMouseMotionEvent(GdkEventMotion * theEvent);
        void addMouseButtonEvent(GdkEventButton * theEvent);
        void addMouseScrollEvent(GdkEventScroll * theEvent);
        void addWindowEvent(GdkEventConfigure * theEvent);
    private:
        std::vector<y60::EventPtr> _myEventQueue;
};

#endif

