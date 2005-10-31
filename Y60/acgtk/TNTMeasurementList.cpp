#include "TNTMeasurementList.h"
#include "CellRendererPixbufToggle.h"

#include <iostream>

using namespace std;

namespace acgtk {

TNTMeasurementList::TNTMeasurementList() :
    Gtk::TreeView()
{
    _myListModel = Gtk::ListStore::create(_myColumns);
    set_model(_myListModel);

    append_column( "Visible",  _myColumns.is_visible );
    append_column( "Type",     _myColumns.type_icon );
    append_column( "Name",     _myColumns.name );
    append_column( "Value",    _myColumns.value );
    append_column( "Editable", _myColumns.is_editable );
}


TNTMeasurementList::TNTMeasurementList( const std::string & theVisibleIcon,
                                        const std::string & theHiddenIcon,
                                        const std::string & theLockedIcon,
                                        const std::string & theEditableIcon ) :
    Gtk::TreeView()
{
    _myListModel = Gtk::ListStore::create(_myColumns);
    set_model(_myListModel);
    set_headers_clickable(true);

    //createDummyRows();

    //=== visibility ===================================
    acgtk::CellRendererPixbufToggle * myCellRenderer = Gtk::manage( new acgtk::CellRendererPixbufToggle);
    Glib::RefPtr<Gdk::Pixbuf> myVisibleIcon = Gdk::Pixbuf::create_from_file(theVisibleIcon);
    myCellRenderer->property_active_icon() = myVisibleIcon;
    myCellRenderer->property_inactive_icon() = Gdk::Pixbuf::create_from_file(theHiddenIcon);
    myCellRenderer->signal_toggled().connect(sigc::mem_fun(*this, & TNTMeasurementList::onVisibilityToggled));
    Gtk::TreeViewColumn * myColumn = Gtk::manage( new Gtk::TreeView::Column );
    myColumn->pack_start( * myCellRenderer, false);
    myColumn->add_attribute(myCellRenderer->property_active(), _myColumns.is_visible);
    myColumn->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
    myColumn->set_fixed_width(33);
    _myVisibilityHeaderIcon.set( myVisibleIcon );
    myColumn->set_widget(_myVisibilityHeaderIcon);
    _myVisibilityHeaderIcon.show();
    append_column( * myColumn);

    //=== type ===================================
    _myTypeColumn = Gtk::manage( new Gtk::TreeView::Column("T") );
    _myTypeColumn->pack_start(_myColumns.type_icon);
    _myTypeColumn->set_clickable(true);
    _myTypeColumn->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
    _myTypeColumn->set_fixed_width(33);
    _myTypeColumn->signal_clicked().connect( sigc::mem_fun( *this, & TNTMeasurementList::onSortByType ));
    append_column( * _myTypeColumn);

    //=== name ===================================
    int myColCount = append_column_editable("Name", _myColumns.name);
    _myNameColumn = get_column( myColCount - 1);
    _myNameColumn->set_expand(true);
    _myNameColumn->set_clickable(true);
    _myNameColumn->signal_clicked().connect( sigc::mem_fun( *this, & TNTMeasurementList::onSortByName ));

    //=== value ===================================
    append_column( "Value", _myColumns.value);

    //=== editability ===================================
    myCellRenderer = Gtk::manage( new acgtk::CellRendererPixbufToggle);
    myCellRenderer->property_active_icon() = Gdk::Pixbuf::create_from_file(theEditableIcon);
    Glib::RefPtr<Gdk::Pixbuf> myLockedIcon = Gdk::Pixbuf::create_from_file(theLockedIcon);
    myCellRenderer->property_inactive_icon() = myLockedIcon;
    myCellRenderer->signal_toggled().connect(sigc::mem_fun(*this, & TNTMeasurementList::onEditableToggled));
    myColumn = Gtk::manage( new Gtk::TreeView::Column );
    myColumn->pack_start( * myCellRenderer, false);
    myColumn->add_attribute(myCellRenderer->property_active(), _myColumns.is_editable);
    myColumn->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
    myColumn->set_fixed_width(33);
    _myEditableHeaderIcon.set( myLockedIcon );
    myColumn->set_widget( _myEditableHeaderIcon );
    _myEditableHeaderIcon.show();
    append_column( * myColumn);

    // setup default sorting
    onSortByName();
}

TNTMeasurementList::~TNTMeasurementList() {
}

void
TNTMeasurementList::registerTypeIcon(const std::string & theTypeName,
                                     const std::string & theIconFile)
{
    _myTypeIcons[theTypeName] = Gdk::Pixbuf::create_from_file(theIconFile);
}

Gtk::TreeIter
TNTMeasurementList::append(dom::NodePtr theMeasurementNode, const Glib::ustring & theDisplayValue) {
    Gtk::TreeIter myIterator = _myListModel->append();
    Gtk::TreeModel::Row myRow = * myIterator;
    myRow[_myColumns.is_visible] =  true;

    std::string myType = theMeasurementNode->getAttributeString("type");
    IconMap::iterator myIt = _myTypeIcons.find(myType);
    if (myIt == _myTypeIcons.end()) {
        throw asl::Exception(std::string("Could not find icon for type '") +
                             myType + "'.", PLUS_FILE_LINE);
    }
    
    myRow[_myColumns.is_visible] = theMeasurementNode->getAttribute("visible")->nodeValueRef<bool>();
    myRow[_myColumns.type_icon] = myIt->second;
    myRow[_myColumns.type] = myType;
    myRow[_myColumns.name] = theMeasurementNode->getAttributeString("name");
    myRow[_myColumns.value] = theDisplayValue;
    myRow[_myColumns.is_editable] = theMeasurementNode->getAttribute("editable")->nodeValueRef<bool>();
    myRow[_myColumns.xml_id] = theMeasurementNode->getAttributeString("id");
    return myIterator;
}

void
TNTMeasurementList::clear() {
    _myListModel->clear();
}


void 
TNTMeasurementList::createDummyRows() {
    cerr << "TNTMeasurementList::createDummyRows()" << endl;
    std::vector<Glib::RefPtr<Gdk::Pixbuf> > myTypeList;
    myTypeList.push_back( Gdk::Pixbuf::create_from_file("/tmp/smallicon_landmark.png"));
    myTypeList.push_back( Gdk::Pixbuf::create_from_file("/tmp/smallicon_distance.png"));
    myTypeList.push_back( Gdk::Pixbuf::create_from_file("/tmp/smallicon_angle.png"));
    myTypeList.push_back( Gdk::Pixbuf::create_from_file("/tmp/smallicon_area.png"));
    myTypeList.push_back( Gdk::Pixbuf::create_from_file("/tmp/smallicon_volume.png"));

    Gtk::TreeModel::Row myRow;
    for (unsigned i = 0; i < 15; ++i) {
        for (unsigned j = 0; j < myTypeList.size(); ++j) {
            myRow = *(_myListModel->append());
            myRow[_myColumns.is_visible] =  i % 2;
            myRow[_myColumns.type_icon] = myTypeList[j];
            myRow[_myColumns.name] = std::string("foo");
            myRow[_myColumns.value] = "23.0";
        }
    }
}

void
TNTMeasurementList::onVisibilityToggled(const Glib::ustring & thePathString) {
    Gtk::TreeModel::Row myRow = * (_myListModel->get_iter(Gtk::TreeModel::Path(thePathString)));
    _myVisibilityToggledSignal.emit(myRow[_myColumns.xml_id]);
}

void
TNTMeasurementList::onEditableToggled(const Glib::ustring & thePathString) {
    Gtk::TreeModel::Row myRow = * (_myListModel->get_iter(Gtk::TreeModel::Path(thePathString)));
    _myLockedToggledSignal.emit(myRow[_myColumns.xml_id]);
}

bool 
TNTMeasurementList::on_button_press_event(GdkEventButton* event) {
    //Call base class, to allow normal handling,
    //such as allowing the row to be selected by the right-click:
    bool return_value = TreeView::on_button_press_event(event);

    //Then do our custom stuff:
    if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) {
        _myRightClickSignal.emit(); 
    }
    return return_value;
}

void
TNTMeasurementList::onSortByName() {
    //AC_WARNING << "sorting by name";
    if ( ! _myNameColumn->get_sort_indicator()) {
        _myTypeColumn->set_sort_indicator(false);
        _myNameColumn->set_sort_indicator(true);
        _myNameColumn->set_sort_order(Gtk::SORT_ASCENDING);
        _myListModel->set_sort_column( _myColumns.name, Gtk::SORT_ASCENDING );
    } else {
        if (_myNameColumn->get_sort_order() == Gtk::SORT_ASCENDING) {
            _myNameColumn->set_sort_order(Gtk::SORT_DESCENDING);
            _myListModel->set_sort_column( _myColumns.name, Gtk::SORT_DESCENDING );
        } else {
            _myNameColumn->set_sort_order(Gtk::SORT_ASCENDING);
            _myListModel->set_sort_column( _myColumns.name, Gtk::SORT_ASCENDING );
        }
    }
}

void
TNTMeasurementList::onSortByType() {
    //AC_WARNING << "sorting by type";
    if ( ! _myTypeColumn->get_sort_indicator()) {
        _myNameColumn->set_sort_indicator(false);
        _myTypeColumn->set_sort_indicator(true);
        _myTypeColumn->set_sort_order(Gtk::SORT_ASCENDING);
        _myListModel->set_sort_column( _myColumns.type, Gtk::SORT_ASCENDING );
    } else {
        if (_myTypeColumn->get_sort_order() == Gtk::SORT_ASCENDING) {
            _myTypeColumn->set_sort_order(Gtk::SORT_DESCENDING);
            _myListModel->set_sort_column( _myColumns.type, Gtk::SORT_DESCENDING );
        } else {
            _myTypeColumn->set_sort_order(Gtk::SORT_ASCENDING);
            _myListModel->set_sort_column( _myColumns.type, Gtk::SORT_ASCENDING );
        }
    }
}

} // end of namespace
