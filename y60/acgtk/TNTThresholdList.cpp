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
#include "TNTThresholdList.h"

#include "CellRendererPixbufToggle.h"

#include <asl/math/Vector234.h>
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

void
TNTThresholdList::select(dom::NodePtr theNode) {
    unsigned int myIndex = theNode->getAttributeValue<unsigned int>("index");
    typedef Gtk::TreeModel::Children ChildrenT;
    ChildrenT myChildren = _myListModel->children();
    for(ChildrenT::iterator it = myChildren.begin(); it != myChildren.end(); ++it) {
        Gtk::TreeModel::Row myRow = *it;
        if (myRow[_myColumns.index] == myIndex) {
            get_selection()->select(myRow);
            Gtk::TreePath myPath( it );
            set_cursor( myPath ); // triggers cursor changed
            return;
        }
    }
}

} // end of namespace
