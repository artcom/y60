

//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


function SwitchNodeHandler( theNode, theActiveIndex) {
    this.Constructor( this, theNode, theActiveIndex);
}


SwitchNodeHandler.prototype.Constructor = function( obj, theNode, theActiveIndex) {

    function setup( theActiveChild ) {
        var myName = new String( _myNode.name ); 
        if ( ! myName.match(/^switch_.*/)) {
            Logger.warning("Switch node '" + _myNode.name + " doesn't obey the " +
                "naming conventions. Name should start with 'switch_'.");
        }
        if ( theActiveChild == undefined) {
            var myActiveChildCount = 0;
            for (var i = 0; i < _myNode.childNodesLength(); ++i) {
                if (_myNode.childNode( i ).visible) {
                    if (myActiveChildCount > 1) {
                        Logger.warning("Switch node '" + _myNode.name + "' has more " +
                                "than one active child. Using first.");
                    } else {
                        _myActiveChild = i;
                    }
                    myActiveChildCount += 1;
                }
            }
        } else {
            for (var i = 0; i < _myNode.childNodesLength(); ++i) {
                if ( i == theActiveChild) {
                    _myNode.childNode( i ).visible = true;
                } else {
                    _myNode.childNode( i ).visible = false;
                }
            }
        }
    }
    
    obj.getSwitchName = function() {
        return _myNode.name;
    }

    obj.getChildCount = function() {
        return _myNode.childNodesLength();
    }
    obj.getNode = function() {
        return _myNode;
    }

    obj.getActiveChild = function() {
        return _myNode.childNode( _myActiveChild );
    }
    obj.getActiveIndex = function() {
        return _myActiveChild;
    }
    obj.getActiveName = function() {
        return _myNode.childNode( _myActiveChild ).name;
    }

    obj.setActiveChild = function( theIndex ) {
        _myNode.childNode( _myActiveChild ).visible = false;
        _myActiveChild = theIndex;
        _myNode.childNode( _myActiveChild ).visible = true;
    }
    obj.setActiveChildByName = function( theName ) {
        _myNode.childNode( _myActiveChild ).visible = false;
        for (var i = 0; i < _myNode.childNodesLength(); ++i) {
            if (_myNode.childNode( i ).name == theName) {
                _myActiveChild = i;
                break;
            }
        }
        _myNode.childNode( _myActiveChild ).visible = true;
    }

    var _myNode = theNode;
    var _myActiveChild = 0;

    setup( theActiveIndex );
}

