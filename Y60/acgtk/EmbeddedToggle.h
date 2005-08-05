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

#ifndef ACGTK_EMBEDDED_TOGGLE_INCLUDED
#define ACGTK_EMBEDDED_TOGGLE_INCLUDED

#include "EmbeddedButton.h"

namespace acgtk {

class EmbeddedToggle : public EmbeddedButton {
    public:
        EmbeddedToggle();
        EmbeddedToggle(const std::string & theDefaultIcon,
                       const std::string & theActiveIcon);
        
        sigc::signal<void> signal_toggled() const { return _myToggledSignal; }

        bool is_active();
        void set_active(bool theFlag);
    protected:
        virtual void on_clicked();

    private:
        Glib::RefPtr<Gdk::Pixbuf> _myActiveIcon;
        bool                      _myActiveFlag;
        sigc::signal<void>        _myToggledSignal;
};

}

#endif // ACGTK_EMBEDDED_TOGGLE_INCLUDED


