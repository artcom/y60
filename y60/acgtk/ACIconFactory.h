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

#ifndef ACGTK_AC_ICON_FACTORY_INCLUDED
#define ACGTK_AC_ICON_FACTORY_INCLUDED

#include "y60_acgtk_settings.h"

#if defined(_MSC_VER)
    #pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gtkmm/iconfactory.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

namespace acgtk {

/** This class hides some of the complexities of Gtk's icon handling
 * and works around some shortcomings of our approch to wrap Gtk.
 */
class Y60_ACGTK_DECL ACIconFactory {
    public:
        ACIconFactory();

        void add(const Glib::ustring & theStockId, const Glib::ustring & theIconFile,
                 const Glib::ustring & theLabel);

    private:
        Glib::RefPtr<Gtk::IconFactory> _myIconFactory;


};

} // end of namespace

#endif // ACGTK_AC_ICON_FACTORY_INCLUDED


