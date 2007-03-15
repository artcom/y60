//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


function SceneViewerDialog(theGladeHandle, theViewer) {
    this.Constructor(this, theGladeHandle, theViewer);
}

SceneViewerDialog.prototype.Constructor = function(self, theGladeHandle, theViewer) {
	
    var _myGladeHandle         = theGladeHandle;
    var _myViewer              = theViewer;
    var _myDialog              = _myGladeHandle.get_widget("dlgSceneViewer");
    var _myPreferenceDocument  = null;
    var _myListScheme 				 = new ACColumnRecord(5);
    var _myListStore 					 = new ListStore(_myListScheme);
    var _myTreeView						 = _myGladeHandle.get_widget("treeViewer");
    var _mySearchField				 = _myGladeHandle.get_widget("search_field");
    
    
    var _myBaseNode						 = null;
		
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