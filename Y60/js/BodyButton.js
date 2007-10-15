//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
use("BuildUtils.js");

use("Y60JSSL.js");

var ourStyleCache = []

const WHITE_COLOR = asColor("FFFFFF");
const BLACK_COLOR = asColor("000000");


const BLUE_COLOR  = asColor("0000FF");
const GREEN_COLOR  = asColor("00FF00");
const RED_COLOR  = asColor("FF0000");

const LIGHT_GREY_COLOR  = asColor("444444");
const GREY_COLOR  = asColor("CCCCCC");
const DARK_GREY_COLOR = asColor("AAAAAA");

const ALPHA_WHITE_COLOR = asColor("FFFFFF", 0.1);
 
const DEFAULT_STYLE = {
    font:              "FONTS/arial.ttf",
    fontname:          "defaultfont",
    textcolor:         WHITE_COLOR,
    fontsize:          20,
}

function createStyleCache(theStylesNode) {
    for(var i=0; i < theStylesNode.childNodes.length; ++i){
        var myStyleNode = theStylesNode.childNode(i);
        !myStyleNode.getAttribute("topPad")     ? myStyleNode.topPad     = 0 : null;
        !myStyleNode.getAttribute("bottomPad")  ? myStyleNode.bottomPad  = 0 : null;
        !myStyleNode.getAttribute("rightPad")   ? myStyleNode.rightPad   = 0 : null;
        !myStyleNode.getAttribute("leftPad")    ? myStyleNode.leftPad    = 0 : null;
        !myStyleNode.getAttribute("tracking")   ? myStyleNode.tracking   = 0 : null;
        !myStyleNode.getAttribute("lineHeight") ? myStyleNode.lineHeight = 0 : null;
        !myStyleNode.getAttribute("HTextAlign") ? myStyleNode.HTextAlign = Renderer.LEFT_ALIGNMENT : null;
        !myStyleNode.getAttribute("VTextAlign") ? myStyleNode.VTextAlign = Renderer.TOP_ALIGNMENT : null;
        !myStyleNode.getAttribute("textColor")  ? myStyleNode.textcolor  = "FFFFFFFF" : null;
        !myStyleNode.getAttribute("backgroundColor")  ? myStyleNode.backgroundColor  = "000000" : null;
        var myAddedFlag = addStyleToDomCache(checksumFromString(stripIdentifier(myStyleNode.name)), myStyleNode); 
        if (myAddedFlag) {
            window.loadTTF(myStyleNode.name, FONT_PATH + myStyleNode.face, myStyleNode.size);               
        }
    }    
    //print(ourCacheDom)

}

// Cache utility functions:
// consider unification with modelling_functions cache
// and MaterialQuadBody cache.

const OUR_CACHE_SCHEMA = " \
<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>            \
    <xs:element name='cache'>                                  \
        <xs:complexType>                                           \
            <xs:sequence minOccurs='0' maxOccurs='unbounded'>      \
                <xs:element ref='movie'/>                       \
                <xs:element ref='image'/>                       \
                <xs:element ref='material'/>                       \
                <xs:element ref='style'/>                       \
            </xs:sequence>                                         \
        </xs:complexType>                                          \
    </xs:element>                                                  \
    <xs:element name='material'>                                   \
        <xs:complexType>                                           \
            <xs:attribute name='id' type='xs:ID' />          \
            <xs:attribute name='sceneid' type='xs:ID' />          \
        </xs:complexType>                                          \
    </xs:element>                                                  \
    <xs:element name='image'>                                   \
        <xs:complexType>                                           \
            <xs:attribute name='id' type='xs:ID' />          \
            <xs:attribute name='sceneid' type='xs:ID' />          \
        </xs:complexType>                                          \
    </xs:element>                                                  \
    <xs:element name='movie'>                                   \
        <xs:complexType>                                           \
            <xs:attribute name='id' type='xs:ID' />          \
            <xs:attribute name='sceneid' type='xs:ID' />          \
        </xs:complexType>                                          \
    </xs:element>                                                  \
    <xs:element name='style'>                                   \
        <xs:complexType>                                           \
            <xs:attribute name='id' type='xs:ID' />          \
            <xs:attribute name='name' type='xs:string' />          \
            <xs:attribute name='textColor' type='xs:string' />          \
            <xs:attribute name='backgroundColor' type='xs:string' />          \
            <xs:attribute name='size' type='xs:int' />          \
            <xs:attribute name='face' type='xs:string' />          \
            <xs:attribute name='topPad' type='xs:int' />          \
            <xs:attribute name='bottomPad' type='xs:int' />          \
            <xs:attribute name='rightPad' type='xs:int' />          \
            <xs:attribute name='leftPad' type='xs:int' />          \
            <xs:attribute name='tracking' type='xs:int' />          \
            <xs:attribute name='lineHeight' type='xs:int' />          \
            <xs:attribute name='HTextAlign' type='xs:string' />          \
            <xs:attribute name='VTextAlign' type='xs:string' />          \
        </xs:complexType>                                          \
    </xs:element>                                                  \
</xs:schema>                                                       \
";


var ourSchema = Node.createDocument();
ourSchema.parse(OUR_CACHE_SCHEMA);

// the cache for images loaded from files
var ourCacheDom = Node.createDocument();
ourCacheDom.useFactories("w3c-schema,som");
ourCacheDom.addSchema(ourSchema,"");
var ourImageCacheNode = new Node("<cache/>").firstChild;
ourCacheDom.appendChild(ourImageCacheNode)


function addStyleToDomCache(theKey, theStyle) {
    if (ourCacheDom.getElementById(theKey)) {
        return false;
    }
    var myStyleNode = theStyle.cloneNode(true);
    myStyleNode.id = theKey;
    ourImageCacheNode.appendChild(myStyleNode);
    return true;
}
function getCachedStyleByKey(theKey) {
    return ourCacheDom.getElementById(theKey);
}
function getCachedStyleByName(theName) {
    return ourCacheDom.getElementById(checksumFromString(stripIdentifier(theName)));
}

function addItemToDomCache(theKey, theItem) {
    if (ourCacheDom.getElementById(theKey)) {
        return;
    }
    var myCacheItem = Node.createElement(theItem.nodeName);
    myCacheItem.id = theKey;
    myCacheItem.sceneid = theItem.id;
    ourImageCacheNode.appendChild(myCacheItem);
}

function getCachedImage(theKey) {
    var myCacheItem = ourCacheDom.getElementById(theKey);
    var myResult = null;
    if (myCacheItem) {
        myResult =  window.scene.dom.getElementById(myCacheItem.sceneid);
    }
   return myResult;
}


function getCachedMaterial(theKey) {
    var myCacheItem = ourCacheDom.getElementById(theKey);
    var myResult = null;
    if (myCacheItem) {
        myResult =  window.scene.dom.getElementById(myCacheItem.sceneid);
    }
   return myResult;
}

// utility functions for BodyButton:
// (refactoring yet to come 2007-10-15)

function createTextAsImage(theText, theSize, theStyle, thePosition) 
{
    if (!thePosition) {
        thePosition = new Vector2i(0,0);
    }
    if (!theStyle) {
        theStyle = getCachedStyleByName("default");
    }
    !theStyle["topPad"]     ? theStyle.topPad     = 0 : null;
    !theStyle["bottomPad"]  ? theStyle.bottomPad  = 0 : null;
    !theStyle["rightPad"]   ? theStyle.rightPad   = 0 : null;
    !theStyle["leftPad"]    ? theStyle.leftPad    = 0 : null;
    !theStyle["tracking"]   ? theStyle.tracking   = 0 : null;
    !theStyle["lineHeight"] ? theStyle.lineHeight = 0 : null;
    !theStyle["HTextAlign"] ? theStyle.HTextAlign = Renderer.LEFT_ALIGNMENT : null;
    !theStyle["VTextAlign"] ? theStyle.VTextAlign = Renderer.TOP_ALIGNMENT : null;
    !theStyle["textColor"]  ? theStyle.textcolor  = "FFFFFFFF" : null;
    !theStyle["backgroundColor"]  ? theStyle.backgroundColor  = "000000" : null;
    
    var myImageNode = null;
    var myTextSize = null;
    var myText = new String(theText);

    var myKey = checksumFromString(stripIdentifier(theText+theStyle.textColor+theStyle.size+theSize));
    var myImageNode = getCachedImage(myKey);
    if (!myImageNode) {
        window.setTextPadding(theStyle.topPad, theStyle.bottomPad, theStyle.leftPad, theStyle.rightPad);
        window.setHTextAlignment(theStyle.HTextAlign);
        window.setVTextAlignment(theStyle.VTextAlign);
        window.setTextColor(asColor(theStyle.textColor));
        window.setTracking(theStyle.tracking);
        window.setLineHeight(theStyle.lineHeight);

        myImageNode = Node.createElement("image");
        myImageNode.resize = "pad";
        myImageNode.wrapmode = "clamp_to_edge"
        myImageNode.mipmap = false;
        window.scene.images.appendChild(myImageNode);
try {
        myTextSize = window.renderTextAsImage( myImageNode, theText, 
                                               theStyle.name, 
                                               theSize[0], theSize[1],
	thePosition);
} catch(e) {
	Logger.error("when trying to renderTextAsImage(" + myImageNode.nodeName + ", " + theText + ", "
				       + theStyle.name + ", " + theSize[0] + ", "
                                       + theSize[1] + ", " + thePosition + "): " + e);
	throw e;
}
        var myMatrix = new Matrix4f();
        myMatrix.makeScaling(new Vector3f(myTextSize.x / myImageNode.width, 
                                          myTextSize.y / myImageNode.height, 1));
        myImageNode.matrix = myMatrix;

        addItemToDomCache(myKey, myImageNode)
    }
    window.setHTextAlignment(Renderer.LEFT_ALIGNMENT);
    window.setVTextAlignment(Renderer.TOP_ALIGNMENT);
    window.setTextPadding(0,0,0,0);
    
    return myImageNode;
}

function createImageFromFile(theFileName) {
    var myKey = checksumFromString(stripIdentifier(theFileName));
    var myImageNode = getCachedImage(myKey);
    if (!myImageNode) {
        myImageNode = Modelling.createImage(window.scene, theFileName);
        addItemToDomCache(myKey, myImageNode);
    }
    return myImageNode;
}

function createMovieFromFile(theFileName, theDecoderHint) {
    var myKey = checksumFromString(stripIdentifier(theFileName));
    var myMovieNode = getCachedImage(myKey);
    
    if (!myMovieNode) {
        myMovieNode = Node.createElement("movie");
        window.scene.images.appendChild(myMovieNode);
        myMovieNode.src = theFileName;
        myMovieNode.name = theFileName;
        if (theDecoderHint) {
            myMovieNode.decoderhint = theDecoderHint;
        }
        window.scene.loadMovieFrame(myMovieNode);
        
        addItemToDomCache(myKey, myMovieNode);
    }
    return myMovieNode;
}

function createMovieAndMaterial( theFile, theAdditionalKey ) {
    var myMovieDecoderHint = new Playlist().getVideoDecoderHintFromURL(theFile);
    var myMovie =  createMovieFromFile(theFile, myMovieDecoderHint);
    var myMovieSize = getImageSize(myMovie);    
    var myKeyString = "Material_"+theFile;
    if (theAdditionalKey != undefined) {
        myKeyString += theAdditionalKey;
    }
    var myMaterialKey = checksumFromString(stripIdentifier(myKeyString));
    var myMaterial = getCachedMaterial(myMaterialKey);
    if ( !myMaterial) {
        myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myMovie);
        addMaterialRequirement(myMaterial, "vertexparams", "[10[color]]");
        //myMaterial.properties.surfacecolor = [1,1,1,1]        
        myMaterial.transparent = true;
        myMaterial.name = theFile;
        myMaterial.properties.targetbuffers.depth = false;
        addItemToDomCache(myMaterialKey, myMaterial)      
    }    
    return {movie:  myMovie, size: myMovieSize, material: myMaterial}; 
}

function createImageAndMaterial( theFile, theAdditionalKey ) {
    var myImage =  createImageFromFile(theFile);
    var myImageSize = getImageSize(myImage);    
    var myKeyString = "Material_"+theFile;
    if (theAdditionalKey != undefined) {
        myKeyString += theAdditionalKey;
    }
    var myMaterialKey = checksumFromString(stripIdentifier(myKeyString));
    var myMaterial = getCachedMaterial(myMaterialKey);
    if ( !myMaterial) {
        myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myImage);
        addMaterialRequirement(myMaterial, "vertexparams", "[10[color]]");
        //myMaterial.properties.surfacecolor = [1,1,1,1]        
        myMaterial.transparent = true;
        myMaterial.name = theFile;
        myMaterial.properties.targetbuffers.depth = false;
        addItemToDomCache(myMaterialKey, myMaterial)      
    }    
    return {image:  myImage, size: myImageSize, material: myMaterial}; 
}

function createTextAndMaterial( theText, theSize, theStyle, theAdditionalKey ) {
    var myImage =  createTextAsImage(theText, theSize, theStyle);
    var myImageSize = getImageSize(myImage);    
    var myKeyString = "Material_"+theText+theStyle.name;
    if (theAdditionalKey != undefined) {
        myKeyString += theAdditionalKey;
    }
    var myMaterialKey = checksumFromString(stripIdentifier(myKeyString));
    var myMaterial = getCachedMaterial(myMaterialKey);
    if ( !myMaterial) {
        myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myImage);
        addMaterialRequirement(myMaterial, "vertexparams", "[10[color]]");
        //myMaterial.properties.surfacecolor = asColor(theStyle.backgroundColor);
        myMaterial.transparent = true;
        myMaterial.properties.targetbuffers.depth = false;
        addItemToDomCache(myMaterialKey, myMaterial)        
    }    
    return {image:  myImage, size: myImageSize, material: myMaterial}; 
}

function createShapeAndBody( theSize, thePosition, theMaterial, theName, theInsensibleFlag, theVisibleFlag) {
    var myQuad = Modelling.createQuad(window.scene, theMaterial.id, 
                                      [0,0, 0], [theSize.x, theSize.y, 0]);
    var myBody = Modelling.createBody(window.scene.world, myQuad.id);
    myBody.position = thePosition;
    myBody.name = theName;
    myBody.insensible = theInsensibleFlag;
    myBody.visible = theVisibleFlag;
    return {body:myBody, shape:myQuad};
}

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

