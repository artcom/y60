#include "ACIconFactory.h"

#include <gtkmm/stock.h>

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
