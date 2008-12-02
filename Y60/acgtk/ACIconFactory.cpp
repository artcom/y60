#include "ACIconFactory.h"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4413 4244 4512)
#endif //defined(_MSC_VER)
#include <gtkmm/stock.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

namespace acgtk {

ACIconFactory::ACIconFactory() :
    _myIconFactory( Gtk::IconFactory::create())
{
    _myIconFactory->add_default();
}

void 
ACIconFactory::add(const Glib::ustring & theStockId, const Glib::ustring & theIconFile,
                   const Glib::ustring & theLabel)
{
    Glib::RefPtr<Gdk::Pixbuf> myPixbuf = Gdk::Pixbuf::create_from_file(theIconFile);
    Gtk::IconSource myIconSource;
    myIconSource.set_pixbuf(myPixbuf);
    Gtk::IconSet myIconSet;
    myIconSet.add_source( myIconSource );
    Gtk::StockID myStockId(theStockId);
    _myIconFactory->add(myStockId, myIconSet);
    Gtk::Stock::add(Gtk::StockItem(myStockId, theLabel));
}

} // end of namespace
