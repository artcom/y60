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
        if (_myItem) {
            removeMenuItem();
        }

        var myLabel = theHandler.switchName;
        if (theLabel != undefined) {
            myLabel = theLabel;
        }
        _myItem = new MenuItem( myLabel );

        _myTearOff = new TearoffMenuItem();
        _myTearOff.show();
        theMenu.append(_myItem);
        _myItem.show();
        _mySubMenu = new Menu();
        _mySubMenu.append(_myTearOff);
        _myItem.submenu = _mySubMenu;
        collectSwitchNodeChildren( theHandler, _mySubMenu );
    }

    function collectSwitchNodeChildren( theHandler, theSubMenu ) {
        var myNode = theHandler.node;
        var myGroupItem = null;

        // texture switch nodes
        if (String(myNode.name).match(/^tswitch_.*/)) {
            // find the transform node with the texture references first
            var myReferenceNode = getDescendantByName(window.scene.world, "textureswitches", true);
            if (!myReferenceNode) {
                Logger.error("Could not find reference node for textureswitch " + myNode.name);
                return;
            }

            for (var i=0; i<myReferenceNode.childNodesLength(); ++i) {
                var myChildNode = myReferenceNode.childNode(i);
                if ((myChildNode.name.indexOf(myNode.name) != -1) || ((myChildNode.name+"M").indexOf(myNode.name) != -1)) {

                    for (var j=0; j<myChildNode.childNodesLength(); ++j) {
                        var myChild = myChildNode.childNode(j);
                        var myItem = createSubmenuItem(myChild, theSubMenu, myGroupItem);
                        if(myGroupItem == null) {
                            myGroupItem = myItem;
                        }
                    }
                    return;
                }
            }
            return;
        }

        // material and geometry switches
        for (var i = 0; i < myNode.childNodesLength(); ++i) {
            var myChild = myNode.childNode( i );
            var myItem = createSubmenuItem(myChild, theSubMenu, myGroupItem);
            if(myGroupItem == null) {
                myGroupItem = myItem;
            }

            if (String(myNode.name).match(/^mswitch_.*/) && i == 0) {
                // switch to first material switch there is
                myItem.active = true;
            } else {
                if (myChild.visible) {
                    myGroupItem.active = false;
                }
                myItem.active = myChild.visible;
            }
        }
    }

    function createSubmenuItem(theNode, theSubMenu, theGroupItem) {
        var myLabelString = theNode.name.replace(/_/g, " ");
        var myItem = new RadioMenuItem(myLabelString, false);

        if (theGroupItem) {
            myItem.setGroupFromItem(theGroupItem);
        }
        myItem.active = false;
        myItem.show();
        theSubMenu.append(myItem);

        _myChildren[ theNode.name ] = myItem;

        var myFunctionString = 'this.onSwitchNodeSwitched(\'' + theNode.name + '\');';
        obj['_myGtkSwitchNodeHandler_'+theNode.name] = new Function (myFunctionString);



        _mySignalHandlers[ theNode.name ] =
            myItem.signal_activate.connect( obj, "_myGtkSwitchNodeHandler_" + theNode.name);

        return myItem;
    }

    obj.onSwitchNodeSwitched = function( theName ) {
        if ( _myHandlers.length <= 0 ||
             _myHandlers[0].activeName == theName) {
            return;
        }

        if ( _myChildren[ theName ].active ) {
            // get previous item and disable it.
            if (_myHandlers[0].activeName) {
                _myChildren[ _myHandlers[0].activeName ].active = false;
            }

            // activate new one.
            for (var i=0; i < _myHandlers.length; ++i) {
                _myHandlers[i].setActiveChildByName( theName );
            }
        }

        if (_myHandlers.length > 0) {
            // remember last switched material for use in materialtable
            Logger.info("onSwitchNodeSwitched: "+theName+" switchName: "+_myHandlers[0].switchName);
            ourViewer.lastSwitched[_myHandlers[0].switchName] = theName;
        }

        if (ourMaterialComboBox != null) {
            updateMaterialEditor();
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
                _myHandlers[0].switchName + " (" + String(_myHandlers.length) + ")");

        // make sure all similar nodes added to this menu item are in the same condition as the first one
        _myHandlers[_myHandlers.length - 1].setActiveChild(_myHandlers[0].activeIndex);
    }

    var _myHandlers = [];
    var _myItem = null;
    var _mySubMenu = null;
    var _myTearOff = null;
    var _myChildren = {};
    var _mySignalHandlers = {};

    setup();
}



function SwitchNodeMenu( ) {
    this.Constructor(this);
}

SwitchNodeMenu.prototype.Constructor = function( obj ) {

    function sortSwitchNodesByName(a,b) {
        if (a.switchName > b.switchName) {
            return 1;
        } else if (a.switchName < b.switchName) {
            return -1;
        }
        return 0;
    }

    function sortSwitchNodes(a,b) {
        if (a.switchName > b.switchName) {
            return 1;
        } else if (a.switchName < b.switchName) {
            return -1;
        } else {
            if (a.childCount > b. childCount) {
                return 1;
            } else if (a.childCount < b. childCount){
                return -1;
            }
            return 0;
        }
    }

    function hasDescendantWithCode(theNode, theCode) {
        if ((theNode.name.search(/_(.+?)_/) != -1) && (theCode == RegExp.$1)) {
            return true;
        }

        for (var i=0; i<theNode.childNodesLength(); ++i) {
            var myChildNode = theNode.childNode(i);
            if (hasDescendantWithCode(myChildNode, theCode)) {
                return true;
            }
        }

        return false;
    }

    function areEqualSwitchNodes(a,b) {
        if (a.childCount != b.childCount) {
            return false;
        }

        if (a.switchName == b.switchName) {
            var i = 0;
            while (i < a.childCount) {
                var myName = a.node.childNode(i).name;
                var myCodePosition = a.node.childNode(i).name.search(/_(.+?)_/);
                var myCode = RegExp.$1;
                var myMatch = hasDescendantWithCode(b.node, myCode);
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
            myMSwitchNodes.sort(sortSwitchNodes);
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
            mySwitchNodes.sort(sortSwitchNodes);

            var mySwitchNodesSameName;
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
