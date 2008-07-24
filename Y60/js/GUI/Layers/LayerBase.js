//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


function LayerBase(Protected, theDepth, theName, theType) {
    this.Constructor(this, Protected, theDepth, theName, theType);
}

LayerBase.prototype.Constructor = function(Public, Protected, theDepth, theName, theType) {

    var _myZ            = Number(theDepth);
    var _myName         = theName;
    var _myType         = theType;
    var _myEffect       = null;
    var _myActiveFlag = true;
    
    //////////////////////////////////////////////////////////////////////      
    //
    // Public
    //
    //////////////////////////////////////////////////////////////////////
    Public.setup = function(theLayerNode) {
        
    }
    Public.onKey = function(theKey, theKeyState, theX, theY,theShiftFlag, theControlFlag, theAltFlag) {return false}
    Public.onFrame = function(theTime) {}
    Public.onPostRender = function() {}
    Public.onASSEvent = function(theNode) {}    
    Public.name getter = function(){
        return _myName;
    }
    Public.type getter  = function() {
        return _myType;
    }
    Public.active setter  = function(theFlag) {
        _myActiveFlag = theFlag
    }
    Public.active getter  = function() {
        return _myActiveFlag;
    }
    //////////////////////////////////////////////////////////////////////
    //
    // Protected
    //
    //////////////////////////////////////////////////////////////////////
    Protected.getDepth = function(){
        return _myZ;
    }        
    //////////////////////////////////////////////////////////////////////
    //
    // Private
    //
    //////////////////////////////////////////////////////////////////////
}
