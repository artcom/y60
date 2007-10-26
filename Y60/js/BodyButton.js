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

const BUTTON_TYPE_TOGGLE = 0;
const BUTTON_TYPE_PUSH = 1;

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
        				 + theStyle.name + ", " + theSize[0] + ", " + theSize[1] + ", " + thePosition + "): " + e);
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
        Protected.shape = myBodyShapeInfo. shape;    
        Protected.type = "image";     
        ourShow.registerButton(self);
    }    
    ////////////////////////////////////////
    // Member 
    ////////////////////////////////////////
    setup();
}
function TextBodyButton(theName, theText, theStyle, thePosition, theCallBackFunction, theSize) {
    this.Constructor(this, theName, theText, theStyle, thePosition, theCallBackFunction, theSize);
}
TextBodyButton.prototype.Constructor = function(self, theName, theText, theStyle, thePosition, 
                                                theCallBackFunction, theSize) 
{
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
        var myImageMaterialInfo = createTextAndMaterial(theText, myTextSize, theStyle);
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

function ImageToggleTextBodyButton(theName, 
                                   theText, 
                                   theTextSize, 
                                   theStyle, 
                                   theFilenames, 
                                   thePosition, 
                                   theCallBackFunction, 
                                   theGroupNode) 
{

var Protected = {};
    this.Constructor(this, Protected, theName, theText, theTextSize, theStyle, theFilenames, 
                     thePosition, theCallBackFunction, theGroupNode);
}

ImageToggleTextBodyButton.prototype.Constructor = function(self, Protected, theName, theText, 
                                                           theTextSize, theStyle, theFilenames, 
                                                           thePosition, theCallBackFunction, 
                                                           theGroupNode) 
{

    if (theGroupNode == undefined) {
        Logger.trace("GroupNode undefined.. setting to window.scene.world");
        theGroupNode = window.scene.world;
    }
    
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
        if (_myType == BUTTON_TYPE_TOGGLE) {
            self.state = (_myState == BUTTON_STATE_DOWN) ? BUTTON_STATE_UP:BUTTON_STATE_DOWN;
            self.BodyButtonBase.press();              
        } else if (_myType == BUTTON_TYPE_PUSH) {
            self.state = BUTTON_STATE_DOWN;
        }
    }
    
    self.unpress = function() {
        if (_myType == BUTTON_TYPE_PUSH) {
            self.resetState();
            // call callback only, when mousebutton has been released
            self.BodyButtonBase.press(); 
        }
    }

    self.state getter = function() {
        return _myState;
    }

    self.state setter = function(theState) {
        _myState = theState;
        self.setState(theState);
    }

    self.type getter = function() {
        return _myType;
    }

    self.type setter = function(theType) {
        _myType = theType;
    }


    self.resetState = function() {
        if(_myState == BUTTON_STATE_DOWN){
            self.setState(BUTTON_STATE_UP);
        }
    }

    self.setVisible = function(theVisibleFlag){
        Logger.trace("setVisible(" + theVisibleFlag + ")" );
        self.body.visible = theVisibleFlag;
        self.body.insensible = !theVisibleFlag;
        if (_myTextBody) {        
            _myTextBody.visible = theVisibleFlag;
        }
        if(_myShadowBodyShapeInfo){
            _myShadowBodyShapeInfo.body.visible = theVisibleFlag;
        }
    }
    
    self.setInsensible = function(theSensibleFlag){
        self.body.insensible = theSensibleFlag;
    }
    self.setState = function(theState) {      
        Logger.trace("setState(" + theState + ")");
        for (var i = 0;  i < Protected.elementnodes.length;i++) {
            var myElementNode = Protected.elementnodes[i];
            myElementNode.material = _myImageMaterialIds[_myState];
        }
    }
    
    self.setAlpha = function(theAlpha){
        Modelling.setAlpha(self.body, theAlpha);
        if (self.textbody) {
            Modelling.setAlpha(self.textbody, theAlpha);
        }
    }
    
    Protected.setupImage = function() {

        var myImageMaterialInfoDown = {};
        var myImageMaterialInfoUp = {};

        Logger.trace("typeof theFilenames[0] = " + typeof theFilenames[0]);

        if (theFilenames[0] instanceof Node &&
            theFilenames[1] instanceof Node) {

            Logger.trace("theFilenames are instances of Node()");

            myImageMaterialInfoDown.material = createMaterialFromImage(theFilenames[0]);
            myImageMaterialInfoDown.size = getImageSize(theFilenames[0]);

            Logger.trace("getImageSize() = " + myImageMaterialInfoDown.size);
            Logger.trace("image.width = " + theFilenames[0].width + 
                         " image.height = " + theFilenames[0].height);

            myImageMaterialInfoUp.material = createMaterialFromImage(theFilenames[1]);
            myImageMaterialInfoUp.size = getImageSize(theFilenames[1]);

        } else if ((typeof theFilenames[0] == "string" &&
                    typeof theFilenames[1] == "string") || 
                   (theFilenames[0] instanceof String &&
                    theFilenames[1] instanceof String)) {

            myImageMaterialInfoDown = createImageAndMaterial(theFilenames[0]);
            myImageMaterialInfoUp   = createImageAndMaterial(theFilenames[1]);
            
            if(theFilenames.length >2){
                Logger.trace("creating drop shadow");

                _myShadowImageMaterialInfo   = createImageAndMaterial(theFilenames[2]);
                var myShadowImageSize = getIconSize(_myShadowImageMaterialInfo.material);
                var myUpImageSize = getIconSize(myImageMaterialInfoUp.material);
                var myShadowXOffset = (myShadowImageSize.x - myUpImageSize.x)/2;
                var myShadowYOffset = (myShadowImageSize.y - myUpImageSize.y)/2;
                var myShadowPosition = new Vector3f(thePosition.x - myShadowXOffset, 
                                                    thePosition.y - myShadowYOffset, 
                                                    thePosition.z - DROP_SHADOW_DEPTH);
                _myShadowBodyShapeInfo = createShapeAndBody(myShadowImageSize, myShadowPosition, 
                                                            _myShadowImageMaterialInfo.material,
                                                            theName + "_shadow", true, true);
                theGroupNode.appendChild(_myShadowBodyShapeInfo.body);
            }
        }

        var myImageSize = getIconSize(myImageMaterialInfoUp.material);
        var myBodyShapeInfo = createShapeAndBody(myImageSize, thePosition, 
                                                 myImageMaterialInfoUp.material,
                                                 theName, false, true);
        theGroupNode.appendChild(myBodyShapeInfo.body);
        Protected.body = myBodyShapeInfo.body;
        Protected.size = myImageMaterialInfoUp.size;        
        Protected.pressedmaterial = myImageMaterialInfoDown.material;       
        Protected.material = myImageMaterialInfoUp.material;       
        Protected.type = "toggle";     
        Protected.elementnodes = myBodyShapeInfo.shape.getNodesByTagName("elements");
        _myImageMaterialIds.push(myImageMaterialInfoDown.material.id);
        _myImageMaterialIds.push(myImageMaterialInfoUp.material.id);       
        
        ourShow.registerButton(self);
    }
    
    Protected.getTextMaterial = function(){
        return _myTextMaterialInfo;    
    }

    Protected.textbody setter = function(theTextBody) {
        _myTextBody = theTextBody;
    }

    self.textbody getter = function() {
        return _myTextBody;
    }
    
    ////////////////////////////////////////
    // Private
    ////////////////////////////////////////    
    
    function setupText(theSize, theTextPosition) {
        var myTextSize = new Vector2f(theSize);
        _myTextMaterialInfo = createTextAndMaterial(theText,myTextSize, theStyle);
        var myImageSize = getImageSize(_myTextMaterialInfo.image);
        var myXOffset = ((self.size.x - myImageSize.x)/2);
        var myYOffset = ((self.size.y - myImageSize.y)/2);
        var myBodyInfo = createShapeAndBody(_myTextMaterialInfo.size, 
                                             new Vector3f(theTextPosition.x + myXOffset, 
                                                          theTextPosition.y + myYOffset,
                                                          theTextPosition.z),
                                             _myTextMaterialInfo.material,
                                             theName + "_text", true, true);
        _myTextBody = myBodyInfo.body;
        theGroupNode.appendChild(_myTextBody);
        Logger.trace("setting up the button text:\n" + 
                     "text image size: " + myImageSize + "\n" + 
                     "self.size: " + self.size + "\n" + 
                     "offset: [" + myXOffset + ", " + myYOffset + "]\n" + 
                     "text position: " + theTextPosition + "\n");
    }

    self.setup = function(){
        Logger.trace("Creating Button " + theName);
        Protected.setupImage();
        var myPosition = new Vector3f(thePosition.x, thePosition.y, TEXT_Z_POSITION);
        if (theText) {
            setupText(theTextSize, myPosition);
        }
    }    
    ////////////////////////////////////////
    // Member 
    ////////////////////////////////////////
   Protected.elementnodes = [];
   var _myState = BUTTON_STATE_UP;
   var _myType = BUTTON_TYPE_TOGGLE;
   var _myImageMaterialIds = [];
   var _myTextBody = null;
   var _myTextMaterialInfo = null;
   var _myShadowImageMaterialInfo = null;
   var _myShadowBodyShapeInfo = null;
    
}



function MultiLanguageButton(theName, 
                             theTexts, 
                             theTextSize, 
                             theStyle, 
                             theFilenames, 
                             thePosition, 
                             theCallBackFunction, 
                             theGroupNode) 
{
    var Protected = {};
    this.Constructor(this, Protected, theName, theTexts, theTextSize, theStyle, 
                     theFilenames, thePosition, theCallBackFunction, theGroupNode);
}


MultiLanguageButton.prototype.Constructor = function(self, 
                                                     Protected, 
                                                     theName, 
                                                     theTexts, 
                                                     theTextSize, 
                                                     theStyle, 
                                                     theFilenames, 
                                                     thePosition, 
                                                     theCallBackFunction, 
                                                     theGroupNode) 
{
    ImageToggleTextBodyButton.prototype.Constructor(self, Protected, theName, theTexts[0], 
                                                    theTextSize, theStyle, theFilenames, 
                                                    thePosition, theCallBackFunction, theGroupNode);
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

    self.size getter = function() {
        return getIconSize(self.material);
    }

    self.position getter = function() {
        return self.body.position;
    }

    self.position setter = function(thePosition) {
        self.body.position = thePosition;
        updateTextBodyPosition(); 
    }

    self.setState = function(theState) {
        Logger.trace("setState(" + theState + ")");
        // switch material for all languages!
        for (var lang = 0; lang < Protected.elementnodes.length; lang++) {
            for (var i = 0;  i < Protected.elementnodes[lang].length;i++) {
                var myElementNode = Protected.elementnodes[lang][i];
                if (theState == BUTTON_STATE_DOWN) {
                    myElementNode.material = _myImageMaterialInfos.down[lang].material.id;
                } else {
                    myElementNode.material = _myImageMaterialInfos.up[lang].material.id;
                }
            }
        }
    }

    Protected.setupImage = function() {

        Logger.trace("setupImage");
        // check validity of theFilenames 
        // (only imagesnodes and filenames are valid)
        var valid_nodes = true;
        var valid_filenames = true;
        for (var i = 0; i < theFilenames.length; i++) {
            valid_nodes &= theFilenames[i] instanceof Node && theFilenames[i].nodeName == "image" ;
            valid_filenames &= typeof theFilenames[i] == "string";// || theFilenames[i] instanceof String;
            // XXX: instanceof String results in an internal error in JSNode,
            //      so don't use filenames in String objects for now :-)
            //      This is probably a bug in JSNode and should be investigated further.
        }


        if (valid_nodes) {
            Logger.trace("Setting up materials with image nodes");
            // setup materials with image nodes
            // images are sorted like this: de_up, de_down, en_up, en_down, ...
            for (var lang = 0; lang < theFilenames.length; lang=lang+2) {
                Logger.trace("creating material for language " + (lang * 0.5));
                var myUpMaterialInfo = {};
                var myDownMaterialInfo = {};

                myUpMaterialInfo.material = createMaterialFromImage(theFilenames[lang+1]);
                myUpMaterialInfo.size = getImageSize(theFilenames[lang+1]);
                _myImageMaterialInfos.up.push(myUpMaterialInfo);
                
                myDownMaterialInfo.material = createMaterialFromImage(theFilenames[lang]);
                myDownMaterialInfo.size = getImageSize(theFilenames[lang]);
                _myImageMaterialInfos.down.push(myDownMaterialInfo);

                var myQuad = Modelling.createQuad(window.scene, 
                                                  myUpMaterialInfo.material.id,
                                                  [0,0,0],
                                                  [myUpMaterialInfo.size.x,
                                                   myUpMaterialInfo.size.y,
                                                   0]);
                myQuad.name = theName + "_imagebody";
                Logger.trace("myUpMaterialInfo.material.id: " + myUpMaterialInfo.material.id);
                Protected.elementnodes.push(myQuad.getNodesByTagName('elements'));
                _myImageQuads.push(myQuad);
            }
        } else if (valid_filenames) {
            Logger.trace("Setting up materials with filenames");
            // setup materials with filenames
            for (var lang = 0; lang < theFilenames.length; lang=lang+2) {
                var myUpMaterialInfo = createImageAndMaterial(theFilenames[lang]);
                _myImageMaterialInfos.up.push(myUpMaterialInfo);
                _myImageMaterialInfos.down.push(createImageAndMaterial(theFilenames[lang+1]));
                var myQuad = Modelling.createQuad(window.scene,
                                                        myUpMaterialInfo.material.id,
                                                        [0,0,0],
                                                        [myUpMaterialInfo.size.x,
                                                         myUpMaterialInfo.size.y,
                                                         0]);
                myQuad.name = theName + "_imagebody";
                _myImageQuads.push(myQuad);
                Logger.trace("myUpMaterialInfo.material.id: " + myUpMaterialInfo.material.id);
                Protected.elementnodes.push(myQuad.getNodesByTagName('elements'));
            }
        } else {
            Logger.error("theFilenames must be either image nodes or valid filenames!");
            return;
        }
       
        Logger.trace("creating body");
        Logger.trace("with shape id: " + _myImageQuads[0].id);
        var myImageBody = Modelling.createBody(theGroupNode, _myImageQuads[0].id);
        Logger.trace("built!");
        myImageBody.position = thePosition;
        Protected.body = myImageBody;
        Protected.material = _myImageMaterialInfos.up[0].material;       
        Protected.pressedmaterial = _myImageMaterialInfos.down[0].material;       
        Protected.type = "toggle";     
        
        ourShow.registerButton(self);
    }
    
    self.switchLanguage = function(theLanguage){
        Logger.trace("switchLanguage(" + theLanguage + ")");
        var myImageSize = 0;
        if (theLanguage >= _myMultiLanguageMaterials.length) {
            Logger.error("Language " + theLanguage + " not supported!");
        }

        _myTextMaterialInfo = _myMultiLanguageMaterials[theLanguage];
        
        if (self.state == BUTTON_STATE_DOWN) {
            Protected.material = _myImageMaterialInfos.down[theLanguage].material;
        } else {
            Protected.material = _myImageMaterialInfos.up[theLanguage].material;
        }
  
        // switch shapes (materials should have been switched already
        //                during state change)
        self.textbody.shape = _myTextQuads[theLanguage].id;
        self.body.shape = _myImageQuads[theLanguage].id; 

        updateTextBodyPosition();
            
    }

    ////////////////////////////////////////
    // Private
    ////////////////////////////////////////    
    
    function updateTextBodyPosition() {
        Logger.trace("updateTextBodyPosition()");
        var myImageSize = getImageSize(_myTextMaterialInfo.image);
        Logger.trace("\tself.size: " + self.size);
        Logger.trace("\tImageSize: " + myImageSize);
        // calculate new text position (must be centered within the background quad);
        var myXOffset = ((self.size.x - myImageSize.x)/2);
        var myYOffset = ((self.size.y - myImageSize.y)/2);
        Logger.trace("\tmyXOffset = " + myXOffset + " myYOffset = " + myYOffset);
        var myNewPosition = new Vector3f(self.body.position.x + myXOffset, 
                                         self.body.position.y + myYOffset,
                                         TEXT_Z_POSITION)
        self.textbody.position =  myNewPosition;                                                
        Logger.trace("\tbody position: " + self.body.position);
        Logger.trace("\tnew text position: " + myNewPosition);
    }

    
    function setupText(theSize, theTextPosition) {
        Logger.trace("setupText(" + theSize + ", " + theTextPosition + ")");

        var myTextSize = theSize;
        
        for (var lang = 0; lang < theTexts.length; lang++) {
            var myMaterialInfo = createTextAndMaterial(theTexts[lang], new Vector2i(0,0), theStyle);
            _myMultiLanguageMaterials.push(myMaterialInfo);
            var myImageSize = getImageSize(myMaterialInfo.image);
            var myQuad = Modelling.createQuad(window.scene,
                                              myMaterialInfo.material.id,
                                              [0,0,0],
                                              [myImageSize.x, myImageSize.y, TEXT_Z_POSITION]);
            myQuad.name = theName + "_textshape";
            _myTextQuads.push(myQuad);
            Logger.trace("myMaterialInfo.material.id: " + myMaterialInfo.material.id);
        }
                                                    
        _myTextMaterialInfo = _myMultiLanguageMaterials[0];
        Logger.trace("create text body");
        Logger.trace("with shape id: " + _myTextQuads[0].id);
        Protected.textbody = Modelling.createBody(theGroupNode, _myTextQuads[0].id);
        self.textbody.insensible = true;
    }
    
    self.setup = function(){
        var myPosition = new Vector3f(thePosition.x, thePosition.y, TEXT_Z_POSITION);
        Protected.setupImage();
        setupText(theTextSize, myPosition);
        self.switchLanguage(GERMAN);
        ourShow.registerButton(self);
    }    

    ////////////////////////////////////////
    // Member 
    ////////////////////////////////////////
   
    // material information for each language 
    var _myImageMaterialInfos = {
        up : [],
        down : []
    }
    var _myMultiLanguageMaterials = [];
    // shapes and bodies for each language
    var _myImageQuads = [];
    // shapes for each text
    var _myTextQuads = [];

    // current text material info
    var _myTextMaterialInfo = null;
    
}




function RadioButtonGroup() {
    this.Constructor(this);
}
RadioButtonGroup.prototype.Constructor = function(self) {

    var _myButtons = [];

    self.add = function(theButton) {  
        Logger.trace("Added new button to button group");
        _myButtons.push(theButton);
        var myCallback = theButton.onClick;

        // clear unpress method if it's a push button
        if (theButton.type == BUTTON_TYPE_PUSH) {
            theButton.unpress = function() {};
        }

        theButton.onClick = function() {
            Logger.trace("Extended button group callback");
            // ensure that the button remains pressed, when
            // you click on it again
            this.state = BUTTON_STATE_DOWN;
            myCallback();
            for (var b = 0; b < _myButtons.length; b++) {
                Logger.trace("Checking button " + _myButtons[b].body.id);
                if (_myButtons[b] != this) {
                    Logger.trace("Resetting state of button " + _myButtons[b].body.id);
                    _myButtons[b].resetState();
                }
            }
        }
    }
}




