//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef ACGTK_AC_ICON_FACTORY_INCLUDED
#define ACGTK_AC_ICON_FACTORY_INCLUDED

#include <gtkmm/iconfactory.h>

namespace acgtk {

/** This class hides some of the complexities of Gtk's icon handling
 * and works around some shortcomings of our approch to wrap Gtk.
 */
class ACIconFactory {
    public:
        ACIconFactory();

        void add(const Glib::ustring & theStockId, const Glib::ustring & theIconFile,
                 const Glib::ustring & theLabel);

    private:
        Glib::RefPtr<Gtk::IconFactory> _myIconFactory;


};

} // end of namespace

#endif // ACGTK_AC_ICON_FACTORY_INCLUDED


