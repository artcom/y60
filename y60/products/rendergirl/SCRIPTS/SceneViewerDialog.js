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


function SceneViewerDialog(theGladeHandle, theViewer) {
    this.Constructor(this, theGladeHandle, theViewer);
}

SceneViewerDialog.prototype.Constructor = function(self, theGladeHandle, theViewer) {

    var _myGladeHandle          = theGladeHandle;
    var _myViewer               = theViewer;
    var _myDialog               = _myGladeHandle.get_widget("dlgSceneViewer");
    var _myPreferenceDocument   = null;
    var _myListScheme           = new ACColumnRecord(5);
    var _myListStore            = new ListStore(_myListScheme);
    var _myTreeView             = _myGladeHandle.get_widget("treeViewer");
    var _mySearchField          = _myGladeHandle.get_widget("search_field");



    var _myBaseNode             = null;

    function setup() {
        _myTreeView.set_model(_myListStore);
        _myTreeView.append_column("", _myListScheme, 1)
        _myTreeView.append_column("Name", _myListScheme, 2);
        _myTreeView.append_column("Child count", _myListScheme, 3);
        _myTreeView.append_column("Visibility", _myListScheme, 4);

        _myBaseNode = window.scene.world;
        _myDialog.signal_response.connect(self, "onResponse");
    }

    setup();

    //=================================================
    //
    //  Gtk Signal Handlers
    //
    //=================================================

    ourHandler.on_go_button = function() {
         var myRow = _myTreeView.selected_row;
         if(myRow) {
            var myChildNode = getDescendantByName(_myBaseNode,myRow.get_value(2),false);
            if(myChildNode) {
                if(myChildNode.childNodes.length>0) {
                    _myGladeHandle.get_widget("main_label").text = myChildNode.name;
                    _myBaseNode = myChildNode;
                    parseChildNodes();
                }
            }
         }
    }

    ourHandler.on_back_button = function() {
         if (_myBaseNode.id == window.scene.world.id) {
            return;
         }
         var parentNode = _myBaseNode.parentNode;
         if(parentNode) {
            self.setBaseNode(parentNode);
         }
    }

    ourHandler.on_visibility_button = function() {

        var myRow = _myTreeView.selected_row;
        if(myRow) {
            var myChildNode = getDescendantByName(_myBaseNode,myRow.get_value(2),false);
            if(myChildNode) {
                myChildNode.visible = !myChildNode.visible;
                myRow.set_value(4, myChildNode.visible ? "visible" : "invisible");
            }
         }
    }


    ourHandler.on_search_button = function() {
        var myNode = getDescendantByName(window.scene.dom, _mySearchField.text, true);
        if(myNode) {
            var myParentNode = myNode.parentNode;
            self.setBaseNode(myParentNode);
            var myParentNodeName = (myParentNode.id == window.scene.world.id) ? "World" : myParentNode.name;
            _myGladeHandle.get_widget("main_label").text = myNode.name + " found in " + myParentNodeName;


        } else {
            _myGladeHandle.get_widget("main_label").text = "not found!";
        }
    }

    //=================================================

    function parseChildNodes() {
        _myListStore.clear();
        for(var i=0; i < _myBaseNode.childNodes.length; i++) {
            var myNode = _myBaseNode.childNodes[i];
            var myChildrenCount = myNode.childNodes.length;
            var myNewRow = _myListStore.append();
            myNewRow.set_value(1, myChildrenCount > 0 ? "+" : "");
            myNewRow.set_value(2, myNode.name);
            myNewRow.set_value(3, myChildrenCount > 0 ? myChildrenCount : "");
            myNewRow.set_value(4, myNode.visible ? "visible" : "invisible");
        }
    }

    self.setBaseNode = function(theBaseNode) {
        _myBaseNode = theBaseNode;
        _myGladeHandle.get_widget("main_label").text = (theBaseNode.id == window.scene.world.id) ? "World" : theBaseNode.name;
        parseChildNodes();


    }


    self.show = function() {
        _myDialog.show();
    }

    self.onResponse = function(theResponse) {
        _myDialog.hide();
    }
}
