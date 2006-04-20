#include "ConsoleView.h"

namespace acgtk {

ConsoleView::ConsoleView() :
    Gtk::TextView(),
    _myScrollback( 100 )
{
}

ConsoleView::~ConsoleView() {

}

void
ConsoleView::append(const Glib::ustring & theText) {
    Glib::RefPtr<Gtk::TextBuffer> myBuffer = get_buffer();
    if (myBuffer) {
        myBuffer->insert(myBuffer->end(), theText);
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
