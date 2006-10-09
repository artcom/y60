//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


function GtkSwitchNodeGroupHandler( theSwitchHandler, theParentMenu ) {
    this.Constructor( this, theSwitchHandler, theParentMenu );
}


GtkSwitchNodeGroupHandler.prototype.Constructor = function( obj, theSwitchHandler, theParentMenu ) {
    
    function setup() {
        buildMenuItem(theSwitchHandler, theParentMenu);
        _myHandlers.push(theSwitchHandler);
    }
    
    function removeMenuItem() {
        for (var h in _mySignalHandlers) {
            _mySignalHandlers[h].disconnect();
        }
        _myItem.submenu = null;
        _myItem = null;
        _mySubMenu = null;
        gc();
    }

    function buildMenuItem(theHandler, theMenu, theLabel) {
        if (_myItem && _mySubMenu) {
            removeMenuItem();
        }
        
        var myLabel = theHandler.getSwitchName();
        if (theLabel != undefined) {
            myLabel = theLabel;
        }
        _myItem = new MenuItem( myLabel );
        _myItem.show();
        theMenu.append( _myItem );
        _mySubMenu = new Menu();
        _myItem.submenu = _mySubMenu;
        collectSwitchNodeChildren( theHandler, _mySubMenu );
    }

    function collectSwitchNodeChildren( theHandler, theSubMenu ) {
        var myNode = theHandler.getNode();
        for (var i = 0; i < myNode.childNodesLength(); ++i) {
            var myChild = myNode.childNode( i );
            var myItem = new CheckMenuItem( myChild.name, myChild.visible );
            if (String(myNode.name).match(/^mswitch_.*/) && i == 0) {
                // switch to first material switch there is
                myItem.active = true;
            } else {
                myItem.active = myChild.visible;
            }
            myItem.show();

            _myChildren[ myChild.name ] = myItem;
            theSubMenu.append( myItem );

            var myFunctionString = 'this.onSwitchNodeSwitched(\'' + myChild.name + '\');';
            obj['_myGtkSwitchNodeHandler_'+myChild.name] = new Function (myFunctionString);

            _mySignalHandlers[ myChild.name ] =
                    myItem.signal_activate.connect( obj, "_myGtkSwitchNodeHandler_" + myChild.name);
        }
    }

    obj.onSwitchNodeSwitched = function( theName ) {
        if ( _myChildren[ theName ].active ) {
            // get previous item and disable it.
            _myChildren[ _myHandlers[0].getActiveName() ].active = false;
            // activate new one.
            for (var i=0; i < _myHandlers.length; ++i) {
                _myHandlers[i].setActiveChildByName( theName );
            }
        }
    }

    obj.finalize = function() {
        removeMenuItem();
        _myChildren = {};
        _myHandlers = [];
        gc();
    }

    obj.addSwitchNode = function(theSwitchHandler) {
        _myHandlers.push(theSwitchHandler);

        // unfortunately there is no other way of changing the menuitem label
        // than to destroy it and make new one [jb]
        buildMenuItem(theSwitchHandler, theParentMenu, 
                _myHandlers[0].getSwitchName()+ " (" + String(_myHandlers.length) + ")");

        // make sure all similar nodes added to this menu item are in the same condition as the first one
        _myHandlers[_myHandlers.length - 1].setActiveChild(_myHandlers[0].getActiveIndex());
    }

    var _myHandlers = [];
    var _myItem = null;
    var _mySubMenu = null;
    var _myChildren = {};
    var _mySignalHandlers = {};

    setup();
}



function SwitchNodeMenu( ) {
    this.Constructor(this);
}

SwitchNodeMenu.prototype.Constructor = function( obj ) {

    function sortSwitchNodesByName(a,b) {
        if (a.getSwitchName() > b.getSwitchName()) {
            return 1;
        } else if (a.getSwitchName() < b.getSwitchName()) {
            return -1;
        } else {
            return 0;
        }
    }

    function areEqualSwitchNodes(a,b) {
        if (a.getSwitchName() == b.getSwitchName()) {
            var i = 0;
            while (i < a.getChildCount()) {
                var myName = a.getNode().childNode(i).name;
                var myMatch = getDescendantByName(b.getNode(), myName);  //b.getNode().childNode(myName);
                if ( !(myMatch) ) {
                    return false;
                }
                ++i;
            }
            
            return true;
        }
        return false;
    }
    
    obj.setup = function( theViewer ) {

        _mySwitchNodeMenuItem = ourGlade.get_widget("switchnode_menu");
        _mySwitchNodeMenu = new Menu();
        _mySwitchNodeMenu.show();
        _mySwitchNodeMenuItem.submenu = _mySwitchNodeMenu;

        var mySwitchNodes  = theViewer.getSwitchNodes();
        var myMSwitchNodes = theViewer.getMaterialSwitchNodes();
        var myTSwitchNodes = theViewer.getTextureSwitchNodes();
        Logger.info("Geometry switches found: " + mySwitchNodes.length);
        Logger.info("Material switches found: " + myMSwitchNodes.length);
        Logger.info("Texture switches found: " + myTSwitchNodes.length);
        
        //material switches
        if ( myMSwitchNodes.length > 0) {
            myMSwitchNodes.sort(sortSwitchNodesByName);
            for (var i = 0; i < myMSwitchNodes.length; ++i) {
                _myHandlers.push( new GtkSwitchNodeGroupHandler( myMSwitchNodes[i],
                                            _mySwitchNodeMenu ) );
            }            
            _mySeparatorItems.push( new SeparatorMenuItem() );
            _mySwitchNodeMenu.append( _mySeparatorItems[_mySeparatorItems.length - 1] );
            _mySeparatorItems[_mySeparatorItems.length - 1].show();
        }

        //geometry switches
        if ( mySwitchNodes.length > 0) {
            mySwitchNodes.sort(sortSwitchNodesByName);
            for (var i = 0; i < mySwitchNodes.length; ++i) {
                if (i > 0 && areEqualSwitchNodes(mySwitchNodes[i], mySwitchNodes[i-1])) {
                    _myHandlers[_myHandlers.length - 1].addSwitchNode(mySwitchNodes[i]);
                } else {
                    _myHandlers.push( new GtkSwitchNodeGroupHandler( mySwitchNodes[i],
                                                _mySwitchNodeMenu ) );
                }
            }
            _mySeparatorItems.push( new SeparatorMenuItem() );
            _mySwitchNodeMenu.append( _mySeparatorItems[_mySeparatorItems.length - 1] );
            _mySeparatorItems[_mySeparatorItems.length - 1].show();
        }

        //texture switches
        if ( myTSwitchNodes.length > 0) {
            myTSwitchNodes.sort(sortSwitchNodesByName);
            for (var i = 0; i < myTSwitchNodes.length; ++i) {
                _myHandlers.push( new GtkSwitchNodeGroupHandler( myTSwitchNodes[i],
                                            _mySwitchNodeMenu ) );
            }
        }
        
        _mySwitchNodeMenuItem.sensitive = _myHandlers.length > 0;
    }

    obj.finalize = function() {
        for (var i = 0; i < _myHandlers.length; ++i) {
            _myHandlers[i].finalize();        
        }
        if (_mySwitchNodeMenuItem) {
            _mySwitchNodeMenuItem.submenu = null;
        }
        _mySwitchNodeMenu = null;
        _mySeparatorItems = [];
        gc();
    }
    
    var _mySwitchNodeMenu = null;
    var _mySwitchNodeMenuItem = null;
    var _mySeparatorItems = [];

    var _myHandlers = [];
}
