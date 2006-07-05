
//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


function GtkSwitchNodeHandler( theSwitchHandler, theParentMenu ) {
    this.Constructor( this, theSwitchHandler, theParentMenu );
}


GtkSwitchNodeHandler.prototype.Constructor = function( obj, theSwitchHandler, theParentMenu ) {
    
    function setup() {
        _myItem = new MenuItem( _myHandler.getSwitchName() );
        _myItem.show();
        theParentMenu.append( _myItem );
        _mySubMenu = new Menu();
        _myItem.submenu = _mySubMenu;
        collectSwitchNodeChildren( _myHandler, _mySubMenu );
    }

    function collectSwitchNodeChildren( theHandler, theSubMenu ) {
        var myNode = theHandler.getNode();
        for (var i = 0; i < myNode.childNodesLength(); ++i) {
            var myChild = myNode.childNode( i );
            var myItem = new CheckMenuItem( myChild.name, myChild.visible );
            myItem.active = myChild.visible;
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
            _myChildren[ _myHandler.getActiveName() ].active = false; 
            // activate new one.
            _myHandler.setActiveChildByName( theName );
        }
    }

    obj.finalize = function() {
        for (var h in _mySignalHandlers) {
            _mySignalHandlers[h].disconnect();
        }
        _myItem.submenu = null;
        _mySubMenu = null;
        _myChildren = {};
        gc();
    }

    var _myHandler = theSwitchHandler;
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

    obj.setup = function( theViewer ) {

        _mySwitchNodeMenuItem = ourGlade.get_widget("switchnode_menu");
        _mySwitchNodeMenu = new Menu();
        _mySwitchNodeMenu.show();
        _mySwitchNodeMenuItem.submenu = _mySwitchNodeMenu;

        var mySwitchNodes = theViewer.getSwitchNodes();
        if ( mySwitchNodes.length > 0) {
            for (var i = 0; i < mySwitchNodes.length; ++i) {

                _myHandlers.push( new GtkSwitchNodeHandler( mySwitchNodes[i],
                            _mySwitchNodeMenu ) );
            }
            _mySwitchNodeMenuItem.sensitive = true;
        } else {
            _mySwitchNodeMenuItem.sensitive = false;
        }
    }

    obj.finalize = function() {
        for (var i = 0; i < _myHandlers.length; ++i) {
            _myHandlers[i].finalize();        
        }
        if (_mySwitchNodeMenuItem) {
            _mySwitchNodeMenuItem.submenu = null;
        }
        _mySwitchNodeMenu = null;
        gc();
    }
    
    var _mySwitchNodeMenu = null;
    var _mySwitchNodeMenuItem = null;

    var _myHandlers = [];
}
