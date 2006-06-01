#include "ConsoleView.h"
#include <asl/Logger.h>

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

        while ( myBuffer->get_line_count() > _myScrollback) {
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
