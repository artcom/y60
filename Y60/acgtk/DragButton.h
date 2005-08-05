//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: SliceViewer.js,v $
//   $Author: danielk $
//   $Revision: 1.97 $
//   $Date: 2005/04/29 13:49:59 $
//
//=============================================================================

#ifndef ACGTK_DRAG_BUTTON_INCLUDED
#define ACGTK_DRAG_BUTTON_INCLUDED

#include "EmbeddedButton.h"

namespace acgtk {

class DragButton : public EmbeddedButton {
    public:
        DragButton();
        DragButton(const std::string & theIconFile);

        sigc::signal<void, double, double> signal_drag_start() const { return _myDragStartSignal; }
        sigc::signal<void, double, double> signal_drag() const { return _myDragSignal; }
        sigc::signal<void> signal_drag_done() const { return _myDragDoneSignal; }

    protected:
        virtual bool on_button_press_event(GdkEventButton * theEvent);
        virtual bool on_button_release_event(GdkEventButton * theEvent);
        virtual bool on_motion_notify_event(GdkEventMotion * theEvent);

        bool on_leave_notify_event(GdkEventCrossing * theEvent);
    private:
        bool _myDragInProgressFlag;
        double _myLastX;
        double _myLastY;
    
        sigc::signal<void, double, double> _myDragStartSignal;
        sigc::signal<void, double, double> _myDragSignal;
        sigc::signal<void>                 _myDragDoneSignal;
        
};

};

#endif // ACGTK_DRAG_BUTTON_INCLUDED


