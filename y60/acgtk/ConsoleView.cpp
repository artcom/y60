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

//own header
#include "ConsoleView.h"

#include <asl/base/Logger.h>

namespace acgtk {

ConsoleView::ConsoleView() :
    Gtk::TextView(),
    _myScrollback( 100 )
{
    /*
    Glib::RefPtr<Gtk::TextBuffer::TagTable> myTagTable = Gtk::TextBuffer::TagTable::create();

    Glib::RefPtr<Gtk::TextBuffer> myBuffer = Gtk::TextBuffer::create(myTagTable);

    set_buffer( myBuffer );
    */
}


ConsoleView::~ConsoleView() {

}

void
ConsoleView::addTag(const Glib::ustring & theTagName, const asl::Vector3f & theTextColor,
        const asl::Vector3f & theBackgroundColor)
{
    Glib::RefPtr<Gtk::TextBuffer::Tag> myTag = get_buffer()->create_tag( theTagName );

    Gdk::Color myGdkColor;
    myGdkColor.set_rgb_p( float(theTextColor[0]) / 1.0,
            float(theTextColor[1]) / 1.0,
            float(theTextColor[2]) / 1.0);
    myTag->property_foreground_gdk() = myGdkColor;
    myGdkColor.set_rgb_p( float(theBackgroundColor[0]) / 1.0,
            float(theBackgroundColor[1]) / 1.0,
            float(theBackgroundColor[2]) / 1.0);
    myTag->property_background_gdk() = myGdkColor;
}

void
ConsoleView::append(const Glib::ustring & theText, const Glib::ustring & theTagName) {
    Glib::RefPtr<Gtk::TextBuffer> myBuffer = get_buffer();
    if (myBuffer) {
        if ( theTagName.empty()) {
            myBuffer->insert(myBuffer->end(), theText);
        } else {
            myBuffer->insert_with_tag(myBuffer->end(), theText, theTagName);
        }
        Gtk::TextIter myEnd = myBuffer->end();
        scroll_to( myEnd, 0.0);

        while ( myBuffer->get_line_count() > static_cast<int>(_myScrollback) ) {
            myBuffer->erase( myBuffer->begin(), myBuffer->get_iter_at_line( 1 ));
        }
    }
}

void
ConsoleView::setScrollback(unsigned theLineCount) {
    _myScrollback = theLineCount;
}

unsigned
ConsoleView::getScrollback() const {
    return _myScrollback;
}

} // end of namespace
