//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

if (__main__ == undefined) var __main__ = "ASSDriverTest";

use("SceneViewer.js");
use("ASSManager.js");
use("KeyButton.js");
use("BuildUtils.js");
use("SoundController.js");

const DISPLAY_SCALE = 20;
const X_MIRROR = false;
const Y_MIRROR = true;
const ORIENTATION = 0.5 * Math.PI;

const PLOT_DATAFILE = "/tmp/ttt.plot"
const FONT_NAME = "Trixie";
const FONT_FILE = "FONTS/TxPl____.ttf"; 
const FONT_SIZE = 18;
const SUBMIT_FONT_SIZE = 16;
const DEFAULT_FONT_COLOR = [1.0, 1.0, 1.0, 1.0];

var DISPLAY_SIZE = new Vector2f(300,40);
var SUBMIT_SIZE = new Vector2f(90,20);
const TYPE_SOUND = "SOUNDS/typewriter.wav";
const KEYSIZE_OFFSET = 1.2;
var KEYSIZE = new Vector2f(19,21);
const KEYS = [["q", "w", "e", "r", "t", "y","u", "i","o", "p"],
              ["a", "s", "d", "f", "g", "h","j", "k","l"],
              ["z", "x", "c", "v", "b", "n","m"]];

var ourButtons = new Array();
var ourFontCache = [];

var ourTypedText = "";

window = new RenderWindow();

function ASSDriverTestApp(theArguments) {
    this.Constructor(this, theArguments);
}



ASSDriverTestApp.prototype.Constructor = function(self, theArguments) {

    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(self, theArguments);
    var Base = [];
    var _myLastEventTime = 0;
    var _myASSManager = null;
    var _myDummyAppContainer = null;
    var _myButtonGroupNode = null;
    var _myWorldCross = null;
    
    var _myPicking     = null;

    var _myDisplayImage = null;
    var _myDisplayMaterial = null;
    var _myDisplayBody = null;
    
    //////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    //////////////////////////////////////////////////////////////////////
   
    // setup
    Base.setup = self.setup;
    self.setup = function(theWidth, theHeight, theTitle) {
        Base.setup(theWidth, theHeight, false, theTitle);
        //window.position = [0, 0];
        window.decorations = false;
        _myPicking = new Picking(window);
        window.resize(theWidth, theHeight);

        _myDummyAppContainer = Modelling.createTransform( window.scene.world );
        _myDummyAppContainer.name = "DummyAppContainer";
        _myDummyAppContainer.scale = [DISPLAY_SCALE, DISPLAY_SCALE, 1];

        _myASSManager = new ASSManager( self, _myDummyAppContainer );
        //_myASSManager.driver.performTara(); // XXX
        //_myASSManager.driver.callibrateTransmissionLevels(); // XXX

        //self.getConfigurator().saveSettings();

        window.camera.frustum.type = ProjectionType.orthonormal;
        window.camera.frustum.width = 400;
        window.camera.position.z = 40;

        loadFont(FONT_NAME + "_" + FONT_SIZE, FONT_FILE, FONT_SIZE);
        loadFont(FONT_NAME + "_" + SUBMIT_FONT_SIZE, FONT_FILE, SUBMIT_FONT_SIZE);
        
        //_myButtonGroupNode = buildGroupNode("ButtonGroup", _myDummyAppContainer);
        _myButtonGroupNode = Modelling.createTransform( _myDummyAppContainer);
        //print(_myButtonGroupNode);
        _myButtonGroupNode.name = "ButtonGroup";
        _myButtonGroupNode.orientation.assignFromEuler(new Vector3f( 0, 0, Math.PI) );
        _myButtonGroupNode.position.y += 1.5;
        _myButtonGroupNode.position.x += 5.5; 
        buildKeyboard();
        buildDisplay();
        buildSubmitButton();
        buildBackspace();
        buildSpacebar();
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);

        _myASSManager.onFrame( theTime );
    }

    Base.onMouseButton = self.onMouseButton;
    self.onMouseButton = function( theButton, theState, theX, theY ) {
        Base.onMouseButton( theButton, theState, theX, theY);
        
    }

    Base.onKey = self.onKey;
    self.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {

        switch( theKey ) {
            case '1':
                if ( theState ) {
                    print("Callibrate transmission levels.");
                    _myASSManager.driver.callibrateTransmissionLevels();
                }
                break;
            case '2':
                if ( theState ) {
                    print("Perform tara.");
                    _myASSManager.driver.performTara();
                }
                break;
            default:
                Base.onKey(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        }
    }

    self.onASSEvent = function( theNode ) {

        _myASSManager.onASSEvent( theNode );

        if (theNode.type == "configure" ) {
            _myASSManager.valueColor = new Vector4f(0,0,1,1);
            //print("event " + theNode.type + " grid size: " + theNode.grid_size);
        } else if ( theNode.type == "lost_communication" || theNode.type == "lost_sync" ) {
            //print("event " + theNode.type );
        } else if ( theNode.type == "touch") {
            var myBody = _myPicking.pickBodyByWorldPos(theNode.position3D);
            if(myBody) {                
                for(var i=0; i<ourButtons.length; ++i ) {
                    if (myBody.id == ourButtons[i].body.id) {
                        ourButtons[i].press();
                    }
                }
            }
        } else {
            if ( ! _myWorldCross ) {
                var myShape = window.scene.world.getElementById("XXX");
                if (myShape) {
                    _myWorldCross = Modelling.createBody( window.scene.world, myShape.id );
                    _myWorldCross.position.z = 5; // XXX
                }
            }
            if ( _myWorldCross ) {
                _myWorldCross.position = new Vector3f( theNode.position3D );
            }
            //print("event " + theNode.type + " at position: " + theNode.position3D +
            //        " dt: " + (theNode.when - _myLastEventTime));
        }
        _myLastEventTime = theNode.when;
    }
    
    self.pressedKey = function(theButton) {
        playSound(TYPE_SOUND, 0.8, 0);
        ourTypedText += theButton.body.name;
        redraw();
    }

    self.submit = function() {
        playSound(TYPE_SOUND, 0.8, 0);
        //copyFile(PLOT_DATAFILE, ourTypedText + ".plot");
        var myString = readFileAsString(PLOT_DATAFILE);
        writeStringToFile("TESTDATA/" + ourTypedText + ".plot",  myString);
    }

    self.backspace = function() {
        playSound(TYPE_SOUND, 0.8, 0);
        ourTypedText = ourTypedText.substring(0, ourTypedText.length-1);
        redraw();
    }

    ///////////////////////////////////////////////////////
    // private funtions 
    ///////////////////////////////////////////////////////
    function redraw() {
        var myDisplayImage = createTextAsImage(ourTypedText, 
                                               FONT_NAME, FONT_SIZE,
                                               DISPLAY_SIZE);
        _myDisplayMaterial.childNode("textures").firstChild.image = myDisplayImage.id;
        //print("text: " + ourTypedText);   
    }
   
    function loadFont(theFontName, theFont, theSize) {
        var myFontCacheName = theFontName + "_" + theSize;
        if (!(myFontCacheName in ourFontCache)) {
            window.loadTTF(theFontName, theFont, theSize);
            ourFontCache[myFontCacheName] = true;
        }
    }

    
    function buildButton(theGroupNode, theFileName, thePressedFileName, 
                     theName, thePosition, theFunctionPtr, theTestID, theType, theSize) 
    {
        //print("Utils::buildButton: theType = " + theType);
        var myButton = new KeyButton(theFileName, thePressedFileName, theName, theTestID, theType, theSize);
        myButton.position = new Vector3f(thePosition.x, thePosition.y, thePosition.z);
        myButton.onClick = theFunctionPtr;
        theGroupNode.appendChild(myButton.body);
        theGroupNode.appendChild(myButton.borderbody);
        return myButton;
    }

    function createTextAsImage(theText, theFontName, theFontSize, 
                               theSize, thePosition, theColor, theUseCacheFlag) 
    {
    
        if (!thePosition) {
            thePosition = new Vector2i(0,0);
        }
        if (!theColor) {
            theColor = DEFAULT_FONT_COLOR;
        }
        window.setTracking(0.0);
        //window.setLineHeight(18.4);
        //window.setMaxFontFittingSize(0);
        window.setTextColor(theColor);
        

        var myImageNode = null;
        var myTextSize = null;
        var myIdentifier = null;
        var myText = new String(theText);

        myImageNode = Node.createElement("image");
        myImageNode.resize = "pad";
        myImageNode.wrapmode = "clamp_to_edge"
        myImageNode.mipmap = false;
        window.scene.images.appendChild(myImageNode);
        myTextSize = window.renderTextAsImage( myImageNode, theText, 
                                               theFontName + "_" + theFontSize, 
                                               theSize.x, theSize.y, thePosition);
        var myMatrix = new Matrix4f();
        myMatrix.makeScaling(new Vector3f(myTextSize.x / myImageNode.width, 
                                          myTextSize.y / myImageNode.height, 1));
        myImageNode.matrix = myMatrix;
        
        return myImageNode;
    }

    function createShapeAndBody( theSize, thePosition, theMaterial, theName, theVisibilityFlag ) {
        var myQuad = Modelling.createQuad(window.scene, theMaterial.id, 
                                          [-theSize.x/2, -theSize.y/2, 0],
                                          [theSize.x/2, theSize.y/2, 0]);
        var myBody = Modelling.createBody(window.scene.world, myQuad.id);
        myBody.position = thePosition;
        myBody.name = theName;
        myBody.insensible = true;
        myBody.visible = theVisibilityFlag;
        return myBody;
    }

    function buildKeyboard() {
        var myPosition = new Vector3f(-3,0,0);
        for(var i=0; i<KEYS.length; i++) {
            for(var j=0; j<KEYS[i].length; j++) {
                var myString = KEYS[i][j];
                myPosition.x += KEYSIZE_OFFSET;
                buildKey(myPosition, myString);
            }
            myPosition.y -= KEYSIZE_OFFSET;
            myPosition.x = -2 + (i*KEYSIZE_OFFSET/2);
        }
    }
    
    function buildKey(thePosition, theString) {
        var myKeyButtonImage = createTextAsImage(theString, 
                                                 FONT_NAME, FONT_SIZE,
                                                 KEYSIZE);
        
        //saveImage(myKeyButtonImage, "keybutton.png");
        var myKeyMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myKeyButtonImage);
        //myKeyMaterial.transparent = true;
        
        var myButton = buildButton(_myButtonGroupNode, 
                                   myKeyMaterial, myKeyMaterial, 
                                   theString,
                                   thePosition, 
                                   self.pressedKey, 
                                   "8smallID", TOGGLE_BUTTON, KEYSIZE);
        
        ourButtons.push(myButton);
    }

    function buildSpacebar() {
        var myKeyButtonImage = createTextAsImage("   space", 
                                                 FONT_NAME, SUBMIT_FONT_SIZE,
                                                 SUBMIT_SIZE);
        
        //saveImage(myKeyButtonImage, "keybutton.png");
        var myKeyMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myKeyButtonImage);
        //myKeyMaterial.transparent = true;
        
        var myButton = buildButton(_myButtonGroupNode, 
                                   myKeyMaterial, myKeyMaterial, 
                                   " ",
                                   new Vector3f(4,-3.7,0), 
                                   self.pressedKey, 
                                   "8smallID", TOGGLE_BUTTON, SUBMIT_SIZE);
        
        ourButtons.push(myButton);
    
    }


    function buildDisplay() {
        _myDisplayImage = createTextAsImage("", //ourTypedText, 
                                            FONT_NAME, FONT_SIZE,
                                            DISPLAY_SIZE);

        _myDisplayMaterial = Modelling.createUnlitTexturedMaterial(window.scene, _myDisplayImage);
        _myDisplayMaterial.transparent = true;

        _myDisplayBody = createShapeAndBody(DISPLAY_SIZE, new Vector3f(5,2.5,0), _myDisplayMaterial, "Display", true);
        //print("displaybody " + _myDisplayBody);
        _myDisplayBody.scale = new Vector3f(0.05,0.05,1.0);
        _myButtonGroupNode.appendChild(_myDisplayBody);
    }

    function buildSubmitButton() {
        var myKeyButtonImage = createTextAsImage("submit", 
                                                 FONT_NAME, SUBMIT_FONT_SIZE,
                                                 SUBMIT_SIZE);
        
        //saveImage(myKeyButtonImage, "keybutton.png");
        var myKeyMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myKeyButtonImage);
        //myKeyMaterial.transparent = true;
        
        var myButton = buildButton(_myButtonGroupNode, 
                                   myKeyMaterial, myKeyMaterial, 
                                   "submit",
                                   new Vector3f(12,0,0), 
                                   self.submit, 
                                   "8smallID", TOGGLE_BUTTON, SUBMIT_SIZE);
        
        ourButtons.push(myButton);
    }

    function buildBackspace() {
        var myKeyButtonImage = createTextAsImage("backspace", 
                                                 FONT_NAME, SUBMIT_FONT_SIZE,
                                                 SUBMIT_SIZE);
        
        //saveImage(myKeyButtonImage, "keybutton.png");
        var myKeyMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myKeyButtonImage);
        //myKeyMaterial.transparent = true;
        
        var myButton = buildButton(_myButtonGroupNode, 
                                   myKeyMaterial, myKeyMaterial, 
                                   "backspace",
                                   new Vector3f(12,-2,0), 
                                   self.backspace, 
                                   "8smallID", TOGGLE_BUTTON, SUBMIT_SIZE);
        
        ourButtons.push(myButton);
    }
}

if (__main__ == "ASSDriverTest") {
    try {
        var ourASSDriverTestApp = new ASSDriverTestApp(
            [expandEnvironment("${PRO}") + "/src/Y60/shader/shaderlibrary_nocg.xml"]);
        //ourASSDriverTestApp.setup(600, 600, "ASSDriverTest");
        ourASSDriverTestApp.setup(1400, 1050, "ASSDriverTest");
        ourASSDriverTestApp.go();
    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
        exit(1);
    }
}

