//=============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

function KeyButton(theIcon, thePressedIcon, theName, theTestID, theType, theSize) {
    this.Constructor(this, theIcon, thePressedIcon, theName, theTestID, theType, theSize);
}


var ourMaterials = {}

const STATE_PRESSED = "pressed";
const STATE_UNPRESSED = "unpressed";

const TOGGLE_BUTTON = "togglebutton";
const PUSH_BUTTON = "pushbutton";

KeyButton.prototype.Constructor = function(Public, 
                                           theMaterial, 
                                           thePressedMaterial, 
                                           theName,
                                           theTestID,
                                           theType,
                                           theSize) {
   
    var _myMaterial = theMaterial;
    var _myPressedMaterial = thePressedMaterial;
    var _myQuad = null;
    var _myPressedQuad = null;
    var _myBody = null;
    var _myName = theName;
    var _myImageSize = theSize;
    var _myState = STATE_UNPRESSED;
    var _myTestID = theTestID;
    var _myType = PUSH_BUTTON;

    ////////////////////////////////////////
    // Public 
    ////////////////////////////////////////
    
    Public.body getter = function() {
        return _myBody;
    }
    
    Public.state getter = function() {
        return _myState;
    }

    Public.body setter = function(theBody) {
        _myBody = theBody;
    }
    Public.shapes getter = function() {
        return {pressed: _myPressedQuad, unpressed:_myQuad};
    }
 
    Public.getBody = function() {
        return _myBody;   
    }
        
    Public.onClick = function() {
        Logger.warning("no on click function implemented for button " + _myBody.name);
    }
       
    Public.press = function() {
        //print("KeyButton::press(): name : " + theName + " , _myState = " + _myState + " _myType = " + _myType);
        if (_myType == TOGGLE_BUTTON) {
            if (_myState == STATE_UNPRESSED) {
                _myState = STATE_PRESSED;
                _myBody.shape = _myPressedQuad.id;
            } else {
                _myState = STATE_UNPRESSED;
                _myBody.shape = _myQuad.id;
            }
        } 
        Public.onClick(Public);
    }

    Public.getSize = function() {
        return getIconSize(_myMaterial);   
    }
    Public.getTestID = function (){
        return _myTestID;
    }

    
    
    ////////////////////////////////////////
    // Private
    ////////////////////////////////////////
    
    function getIconSize(theMaterial) {
        //var myTextureNode = getDescendantByTagName(theMaterial, "texture", true);
        var myTextureNode = theMaterial.getNodesByTagName("texture")[0];
        
        var myImageNode = myTextureNode.getElementById(myTextureNode.image);
        return getImageSize(myImageNode); //new Vector2f(myImageNode.width, myImageNode.height);
    }
    
    
    ////////////////////////////////////////
    // Cstor 
    ////////////////////////////////////////

    if (theType) {
        _myType = theType;
    }
    

    
    if(theSize != undefined) {
        _myImageSize = theSize;
    }
    //print("##### name :" + theName + " -> size: " + _myImageSize);
    //Logger.warning("creating button " + _myName + "  " + _myImageSize);
    _myQuad = Modelling.createQuad(window.scene, _myMaterial.id,
                                   [-_myImageSize.x/2.0, -_myImageSize.y/2.0,0],
                                   [_myImageSize.x/2.0,_myImageSize.y/2.0,0]);
    _myPressedQuad = Modelling.createQuad(window.scene, _myPressedMaterial.id,
                                          [-_myImageSize.x/2.0,-_myImageSize.y/2.0,0],
                                          [_myImageSize.x/2.0,_myImageSize.y/2.0,0]);
    _myBody = Modelling.createBody(window.scene.world, _myQuad.id);
    _myBody.name = _myName;
    _myBody.scale = new Vector3f(0.05,0.05,1.0);
    //_myBody.visible = false;
    _myBody.insensible = false;
    //_myBody.testID = _myTestID;
    //ourShow.registerKeyButton(Public);
}
