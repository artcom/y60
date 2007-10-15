//=============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

const BUTTON_STATE_DOWN = 0;                
const BUTTON_STATE_UP   = 1;                

var ourStaticInvisibleMaterial = null;

function BodyButtonBase(Protected, theName, theCallBackFunction, theTestID) {
    this.Constructor(this, Protected, theName, theCallBackFunction, theTestID);
}

BodyButtonBase.prototype.Constructor = function(Public, Protected, theName, theCallBackFunction, theTestID) {
   
    var _myBody = null;
    var _myShape = null;
    var _myName = theName;
    var _myTestID = theTestID;
    var _mySize = null;
    var _myMaterial = null;
    var _myPressedMaterial = null;
    var _myType = null;
    
    ////////////////////////////////////////
    // Public 
    ////////////////////////////////////////
    
    Public.body getter = function() {
        return _myBody;
    }
    Public.shape getter = function() {
        return _myShape;
    }
    Public.size getter = function() {
        return _mySize;
    }
    Public.material getter = function() {
        return _myMaterial;
    }
    Public.pressedmaterial getter = function() {
        return _myPressedMaterial;
    }
    Public.name getter = function() {
        return _myName;
    }
    Public.type getter = function() {
        return _myType;
    }
    
    Public.onClick = function() {}
    Public.press = function() {Public.onClick(Public);}
    ////////////////////////////////////////
    // Protected
    ////////////////////////////////////////
    Protected.body setter = function(theBody) {
        _myBody = theBody;
    }
    Protected.shape setter = function(theShape) {
        _myShape = theShape;
    }
    Protected.size setter = function(theSize) {
        _mySize = theSize;
    }
    Protected.type setter = function(theType) {
        _myType = theType;
    }
    Protected.material setter = function(theMaterial) {
        _myMaterial = theMaterial;
    }
    Protected.pressedmaterial setter = function(thePressedMaterial) {
        _myPressedMaterial = thePressedMaterial;
    }
    ////////////////////////////////////////
    // Constructor
    ////////////////////////////////////////
    if (theCallBackFunction) {
        Public.onClick = theCallBackFunction;
    }
}       

function ImageBodyButton(theName, theImageInfo, thePosition, theCallBackFunction) {
    this.Constructor(this, theName, theImageInfo, thePosition, theCallBackFunction);
}
ImageBodyButton.prototype.Constructor = function(self, theName, theImageInfo, thePosition, theCallBackFunction) {
    var Protected = {};
    BodyButtonBase.prototype.Constructor(self, Protected, theName, theCallBackFunction);
    self.BodyButtonBase = [];

    ////////////////////////////////////////
    // Public 
    ////////////////////////////////////////
    self.BodyButtonBase.press = self.press;    
    self.press = function() {        
        self.BodyButtonBase.press();              
    }
    ////////////////////////////////////////
    // Private
    ////////////////////////////////////////
    
    function setup() {
        
        var myImageMaterialInfo = createImageAndMaterial(theImageInfo);        
        Protected.size = myImageMaterialInfo.size;
        Protected.material = myImageMaterialInfo.material;       
        var myBodyShapeInfo = createShapeAndBody(myImageMaterialInfo.size, thePosition, myImageMaterialInfo.material, theName, false, true);
    
        Protected.body = myBodyShapeInfo.body;
        Protected.shape = myBodyShapeInfo.shape;    
        Protected.type = "image";    
        ourShow.registerButton(self);
    }    
    ////////////////////////////////////////
    // Member 
    ////////////////////////////////////////
    setup();
}
function TextBodyButton(theName, theImageInfo, theStyle, thePosition, theCallBackFunction, theSize) {
    this.Constructor(this, theName, theImageInfo, theStyle, thePosition, theCallBackFunction, theSize);
}
TextBodyButton.prototype.Constructor = function(self, theName, theImageInfo, theStyle, thePosition, theCallBackFunction, theSize) {
    var Protected = {};
    BodyButtonBase.prototype.Constructor(self, Protected, theName, theCallBackFunction);
    self.BodyButtonBase = [];

    ////////////////////////////////////////
    // Public 
    ////////////////////////////////////////
    self.BodyButtonBase.press = self.press;    
    self.press = function() {        
        self.BodyButtonBase.press();              
    }
    ////////////////////////////////////////
    // Private
    ////////////////////////////////////////    
    function setup() {
        var myTextSize = new Vector2i(0,0);
        if(theSize){
            myTextSize = theSize;
        }
        var myImageMaterialInfo = createTextAndMaterial(theImageInfo, myTextSize, theStyle);
        Protected.size = myImageMaterialInfo.size;
        Protected.material = myImageMaterialInfo.material;
        var myBodyShapeInfo = createShapeAndBody(myImageMaterialInfo.size, thePosition, myImageMaterialInfo.material, theName, false, true);
        
        Protected.body = myBodyShapeInfo.body;
        Protected.shape = myBodyShapeInfo.shape;
        Protected.type = "text";    
        ourShow.registerButton(self);
    }    
    ////////////////////////////////////////
    // Member 
    ////////////////////////////////////////
    setup();
}

function ImageToggleTextBodyButton(theName, theText, theTextSize, theStyle, theFilenames, thePosition, theCallBackFunction) {
    var Protected = {};
    this.Constructor(this, Protected, theName, theText, theTextSize, theStyle, theFilenames, thePosition, theCallBackFunction);
}
ImageToggleTextBodyButton.prototype.Constructor = function(self, Protected, theName, theText, theTextSize, theStyle, theFilenames, thePosition, theCallBackFunction) {
    
    BodyButtonBase.prototype.Constructor(self, Protected, theName, theCallBackFunction);
    self.BodyButtonBase = [];
    const BUTTON_STATE_DOWN = 0;                
    const BUTTON_STATE_UP   = 1;        
    
    const DROP_SHADOW_DEPTH = 1;    
    ////////////////////////////////////////
    // Public 
    ////////////////////////////////////////
    self.BodyButtonBase.press = self.press;    
    self.press = function() {      
        _myState = (_myState == BUTTON_STATE_DOWN) ? BUTTON_STATE_UP:BUTTON_STATE_DOWN;
        for (var i = 0;  i < _myElementNodes.length;i++) {
            var myElementNode = _myElementNodes[i];
            myElementNode.material = _myImageMaterialIds[_myState];
        }
        self.BodyButtonBase.press();              
    }
    
    self.state getter = function() {
        return _myState;
    }
    self.resetState = function() {
        if(_myState == BUTTON_STATE_DOWN){
            _myState = BUTTON_STATE_UP;
            for (var i = 0;  i < _myElementNodes.length;i++) {
                var myElementNode = _myElementNodes[i];
                myElementNode.material = _myImageMaterialIds[_myState];
            }
        }
    }
    self.setVisible = function(theVisibleFlag){
        self.body.visible = theVisibleFlag;
        self.body.insensible = !theVisibleFlag;
        if (_myTextBodyInfo) {        
            _myTextBodyInfo.body.visible = theVisibleFlag;
        }
        if(_myShadowBodyShapeInfo){
            _myShadowBodyShapeInfo.body.visible = theVisibleFlag;
        }
    }
    
    self.setInsensible = function(theSensibleFlag){
        self.body.insensible = theSensibleFlag;
    }
    self.setState = function(theState) {      
        _myState = theState;
        for (var i = 0;  i < _myElementNodes.length;i++) {
            var myElementNode = _myElementNodes[i];
            myElementNode.material = _myImageMaterialIds[_myState];
        }
    }
    
    self.setAlpha = function(theAlpha){
        Modelling.setAlpha(self.body, theAlpha);
        if (_myTextBodyInfo) {
            Modelling.setAlpha(_myTextBodyInfo.body, theAlpha);
        }
        if(_myShadowBodyShapeInfo){
            Modelling.setAlpha(_myShadowBodyShapeInfo.body, theAlpha);
        }
    }
    
    Protected.setupImage = function() {
        var myImageMaterialInfoDown = createImageAndMaterial(theFilenames[0]);
        var myImageMaterialInfoUp   = createImageAndMaterial(theFilenames[1]);
        if(theFilenames.length >2){
            _myShadowImageMaterialInfo   = createImageAndMaterial(theFilenames[2]);
            var myShadowImageSize = getIconSize(_myShadowImageMaterialInfo.material);
            var myShadowXOffset = (myShadowImageSize.x - (getIconSize(myImageMaterialInfoUp.material)).x)/2;
            var myShadowYOffset = (myShadowImageSize.y - (getIconSize(myImageMaterialInfoUp.material)).y)/2;
            var myShadowPosition = new Vector3f(thePosition.x - myShadowXOffset, thePosition.y - myShadowYOffset, thePosition.z - DROP_SHADOW_DEPTH);
            _myShadowBodyShapeInfo = createShapeAndBody(myShadowImageSize, myShadowPosition, _myShadowImageMaterialInfo.material,
                                                 theName + "_shadow", true, true);
        }

        var myImageSize = getIconSize(myImageMaterialInfoUp.material);
        var myBodyShapeInfo = createShapeAndBody(myImageSize, thePosition, myImageMaterialInfoUp.material,
                                                 theName, false, true);
        Protected.body = myBodyShapeInfo.body;
        Protected.size = myImageMaterialInfoUp.size;        
        Protected.pressedmaterial = myImageMaterialInfoDown.material;       
        Protected.material = myImageMaterialInfoUp.material;       
        Protected.type = "toggle";     
        _myElementNodes = myBodyShapeInfo.shape.getNodesByTagName("elements");
        _myImageMaterialIds.push(myImageMaterialInfoDown.material.id);
        _myImageMaterialIds.push(myImageMaterialInfoUp.material.id);       
        
        
        
        ourShow.registerButton(self);
    }
    
    Protected.getTextMaterial = function(){
        return _myTextMaterialInfo;    
    }
    Protected.getTextBodyInfo = function(){
        return _myTextBodyInfo;    
    }
    
    ////////////////////////////////////////
    // Private
    ////////////////////////////////////////    
    
    function setupText(theSize, theTextPosition) {
        var myTextSize = theSize;
        
        _myTextMaterialInfo = createTextAndMaterial(theText, myTextSize, theStyle);
        var myImageSize = getImageSize(_myTextMaterialInfo.image);
        var myXOffset = ((self.size.x - myImageSize.x)/2);
        var myYOffset = ((self.size.y - myImageSize.y)/2);
        _myTextBodyInfo = createShapeAndBody(_myTextMaterialInfo.size, new Vector3f(theTextPosition.x + myXOffset, theTextPosition.y + myYOffset,
                                                 theTextPosition.z), _myTextMaterialInfo.material,
                                                 theName + "_text", true, true);
    }
    function setup(){
        Protected.setupImage();
        var myPosition = new Vector3f(thePosition.x, thePosition.y, TEXT_Z_POSITION);
        if (theText) {
            setupText(theTextSize, myPosition);
        }
    }    
    ////////////////////////////////////////
    // Member 
    ////////////////////////////////////////
   var _myState = BUTTON_STATE_UP;
   var _myImageMaterialIds = [];
   var _myTextBodyInfo = null;
   var _myTextMaterialInfo = null;
   var _myElementNodes = null;
   var _myShadowImageMaterialInfo = null;
   var _myShadowBodyShapeInfo = null;
   setup();
    
}



function LanguageImageToggleTextBodyButton(theName, theTexts, theTextSize, theStyle, theFilenames, thePosition, theCallBackFunction) {
    var Protected = {};
    this.Constructor(this, Protected, theName, theTexts, theTextSize, theStyle, theFilenames, thePosition, theCallBackFunction);
}
LanguageImageToggleTextBodyButton.prototype.Constructor = function(self, Protected, theName, theTexts, theTextSize, theStyle, theFilenames, thePosition, theCallBackFunction) {
    
    ImageToggleTextBodyButton.prototype.Constructor(self, Protected, theName, theTexts[0], theTextSize, theStyle, theFilenames, thePosition, theCallBackFunction);
    self.Base = [];

    ////////////////////////////////////////
    // Public 
    ////////////////////////////////////////
    self.Base.setVisible = self.setVisible;    
    self.setVisible = function(theVisibleFlag) {      
        self.Base.setVisible(theVisibleFlag);   
    }
    
    self.Base.setAlpha = self.setAlpha;    
    self.setAlpha = function(theAlpha){
        self.Base.setAlpha(theAlpha);
    }
    
    self.switchLanguage = function(theLanguage){
        var myImageSize = 0;
        if(theLanguage == GERMAN) {
            _myTextMaterialInfo.material.childNode("textures").firstChild.image = _myGermanTextMaterialInfo.image.id;
            _myTextBodyInfo.body.shape = _myGermanTextQuad.id;
            myImageSize = getImageSize(_myGermanTextMaterialInfo.image);
            
        } else {
            _myTextMaterialInfo.material.childNode("textures").firstChild.image = _myEnglishTextMaterialInfo.image.id;
            _myTextBodyInfo.body.shape = _myEnglishTextQuad.id;
            myImageSize = getImageSize(_myEnglishTextMaterialInfo.image);
        }
        
        var myXOffset = ((self.size.x - myImageSize.x)/2);
        var myYOffset = ((self.size.y - myImageSize.y)/2);
        var myNewPosition = new Vector3f(thePosition.x + myXOffset, thePosition.y + myYOffset,
                                                 TEXT_Z_POSITION)
        _myTextBodyInfo.body.position =  myNewPosition;                                                
            
    }
    ////////////////////////////////////////
    // Private
    ////////////////////////////////////////    
    
    function setupText(theSize, theTextPosition) {
        var myTextSize = theSize;
        
        _myGermanTextMaterialInfo = createTextAndMaterial(theTexts[0], new Vector2i(0,0), theStyle);
        _myEnglishTextMaterialInfo = createTextAndMaterial(theTexts[1],new Vector2i(0,0), theStyle);
        var myImageSize = getImageSize(_myEnglishTextMaterialInfo.image);
        _myEnglishTextQuad = Modelling.createQuad(window.scene, _myEnglishTextMaterialInfo.material.id, 
                                      [0, 0, 0],
                                      [myImageSize.x, myImageSize.y, TEXT_Z_POSITION]);
                                      
        _myTextMaterialInfo = Protected.getTextMaterial();
        _myTextBodyInfo = Protected.getTextBodyInfo();
        
        myImageSize = getImageSize(_myGermanTextMaterialInfo.image);
        _myGermanTextQuad = Modelling.createQuad(window.scene, _myGermanTextMaterialInfo.material.id, 
                                      [0, 0, 0],
                                      [myImageSize.x, myImageSize.y, TEXT_Z_POSITION]);
        
    }
    
    function setup(){
        var myPosition = new Vector3f(thePosition.x, thePosition.y, TEXT_Z_POSITION);
        setupText(theTextSize, myPosition);
        self.switchLanguage(GERMAN);
    }    
    ////////////////////////////////////////
    // Member 
    ////////////////////////////////////////
   var _myEnglishTextMaterialInfo = null;
   var _myGermanTextMaterialInfo = null;
   var _myTextMaterialInfo = null;
   var _myTextBodyInfo = null;
   var _myEnglishTextQuad = null;
   var _myGermanTextQuad = null;
   setup();
    
}

