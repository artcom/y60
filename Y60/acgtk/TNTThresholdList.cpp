#include "TNTThresholdList.h"
#include "CellRendererPixbufToggle.h"

#include <asl/Vector234.h>
#include <iostream>

using namespace std;
using namespace asl;

namespace acgtk {

TNTThresholdList::TNTThresholdList() :
    Gtk::TreeView()
{
    _myListModel = Gtk::ListStore::create(_myColumns);
    set_model(_myListModel);

    Gtk::CellRendererText * myCellRenderer = Gtk::manage( new Gtk::CellRendererText());
    myCellRenderer->property_cell_background_set() = true;
    int myColumnCount = append_column("Color", * myCellRenderer );
    Gtk::TreeViewColumn * myColumn = get_column( myColumnCount - 1);
    myColumn->add_attribute(myCellRenderer->property_cell_background_gdk(), _myColumns.color);

    append_column( "Name",  _myColumns.name );
    append_column( "Lower", _myColumns.lowerThreshold );
    append_column( "Upper", _myColumns.upperThreshold );
    
}


TNTThresholdList::~TNTThresholdList() {
}

void
TNTThresholdList::clear() {
    _myListModel->clear();
}

void 
TNTThresholdList::refresh(dom::NodePtr thePaletteNode) {
    clear();
    Gtk::TreeModel::Row myRow;
    Vector2f myThresholds;
    int myIndex;
    Vector3i myColor;
    Gdk::Color myGdkColor;
    for (unsigned i = 0; i < thePaletteNode->childNodesLength(); ++i) {
        myThresholds = thePaletteNode->childNode(i)->getAttributeValue<Vector2f>("threshold");
        myIndex = thePaletteNode->childNode(i)->getAttributeValue<int>("index");
        myRow = * (_myListModel->append());
        myRow[_myColumns.name] = thePaletteNode->childNode(i)->getAttributeString("name");
        myRow[_myColumns.lowerThreshold] = int( myThresholds[0] );
        myRow[_myColumns.upperThreshold] = int( myThresholds[1] ); 
        myRow[_myColumns.index] = myIndex;
        myColor = thePaletteNode->childNode(i)->getAttributeValue<Vector3i>("color");
        myGdkColor.set_rgb_p( float(myColor[0]) / 255,
                              float(myColor[1]) / 255,
                              float(myColor[2]) / 255);
        myRow[_myColumns.color] = myGdkColor;
    }
}

} // end of namespace
