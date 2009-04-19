/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2009, ART+COM AG Berlin, Germany <www.artcom.de>
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
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

/**
 * Widgets wrap Y60 scene objects.
 * 
 * This is where positioning, orientation, scaling,
 * visibility and sensibility go.
 */
spark.Widget = spark.AbstractClass("Widget");

spark.Widget.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.EventDispatcher);

    var _mySceneNode   = null;
    
    // XXX: try to get rid of this.
    Public.sceneNode getter = function() {
        return _mySceneNode;
    };

    // XXX: function for getting screen-aligned bounds
    Public.worldposition getter = function() {
        return _mySceneNode.globalmatrix.getTranslation();
    };

    // XXX: function for getting bounds
    Public.size getter = function() {
        var myBoundingBox = _mySceneNode.boundingbox;
        var myWidth = 0;
        if(myBoundingBox == "[]"){
            Logger.warning("BoundingBox not initialized - size getter not yet implemented");
            return new Vector2f(0,0);
        } else {
            return myBoundingBox.size;
        }
    };

    Public.width getter = function(){
        return Public.size.x;
    };
    
    Public.height getter = function(){
        return Public.size.y;
    };
    


    // ALPHA

    var _myAlpha = 1.0;

    Public.alpha getter = function() {
        return _myAlpha;
    };

    Public.alpha setter = function(theValue) {
        _myAlpha = theValue;
        Public.propagateAlpha();
    };


    var _myActualAlpha = 1.0;

    Public.actualAlpha getter = function() {
        return _myActualAlpha;
    };


    Public.parentAlpha getter = function() {
        var myParentAlpha = 1.0;
        if((Public.parent) && ("actualAlpha" in Public.parent)) {
            myParentAlpha = Public.parent.actualAlpha;
        }
        return myParentAlpha;
    };


    Public.propagateAlpha = function() {
        _myActualAlpha = Public.parentAlpha * _myAlpha;

        for(var i = 0; i < Public.children.length; i++) {
            var myChild = Public.children[i];
            if("propagateAlpha" in myChild) {
                myChild.propagateAlpha();
            }
        }
    };
    
    
    // VISIBILITY

    Public.visible getter = function() {
        return _mySceneNode.visible;
    };

    Public.visible setter = function(theValue) {
        _mySceneNode.visible = theValue;
    };


    // SENSIBILITY

    Public.sensible getter = function() {
        return !_mySceneNode.insensible;
    };

    Public.sensible setter = function(theValue) {
        _mySceneNode.insensible = !theValue;
    };
    
    // POSITION

    Public.x getter = function() {
        return _mySceneNode.position.x;
    };

    Public.x setter = function(theValue) {
        _mySceneNode.position.x = theValue;
    };

    Public.y getter = function() {
        return _mySceneNode.position.y;
    };

    Public.y setter = function(theValue) {
        _mySceneNode.position.y = theValue;
    };

    Public.z getter = function() {
        return _mySceneNode.position.z;
    };

    Public.z setter = function(theValue) {
        _mySceneNode.position.z = theValue;
    };

    Public.position getter = function() {
        return _mySceneNode.position;
    };

    
    Public.position setter = function(theValue) {
        _mySceneNode.position.x = theValue.x + _myOrigin.x;
        _mySceneNode.position.y = theValue.y + _myOrigin.y;
        _mySceneNode.position.z = theValue.z + _myOrigin.z;
    };
    
    // SCALE
    
    Public.scaleX getter = function() {
        return _mySceneNode.scale.x;
    };

    Public.scaleX setter = function(theValue) {
        _mySceneNode.scale.x = theValue;
    };

    Public.scaleY getter = function() {
        return _mySceneNode.scale.y;
    };

    Public.scaleY setter = function(theValue) {
        _mySceneNode.scale.y = theValue;
    };

    Public.scaleZ getter = function() {
        return _mySceneNode.scale.z;
    };

    Public.scaleZ setter = function(theValue) {
        _mySceneNode.scale.z = theValue;
    };

    Public.scale getter = function() {
        return _mySceneNode.scale.clone();
    };

    Public.scale setter = function(theValue) {
        _mySceneNode.scale.x = theValue.x;
        _mySceneNode.scale.y = theValue.y;
        _mySceneNode.scale.z = theValue.z;
    };


    // PIVOT
    // XXX: finish implementing pivot wrappers

    Public.pivot getter = function() {
        return _mySceneNode.pivot.clone();
    };

    Public.pivot setter = function(theValue) {
        _mySceneNode.pivot.x = theValue.x;
        _mySceneNode.pivot.y = theValue.y;
        _mySceneNode.pivot.z = theValue.z;
    };

    // ORIGIN
    // XXX: finish implementing origin wrappers

    var _myOrigin = new Vector3f(0,0,0);
    
    Protected.originX getter = function() {
        return _myOrigin.x;
    };

    Protected.originY getter = function() {
        return _myOrigin.y;
    };

    Protected.originZ getter = function() {
        return _myOrigin.z;
    };

    Protected.origin getter = function() {
        return new Vector3f(_myOrigin);
    };

    Protected.origin setter = function(theValue) {
        _myOrigin.x = theValue.x;
        _myOrigin.y = theValue.y;
        _myOrigin.z = theValue.z;        
    }
    
    // ROTATION
    var _myRotationDegrees = new Vector3f();
    
    Public.rotationX getter = function() {
        return _myRotationDegrees.x;
    };

    Public.rotationX setter = function(theValue) {
        _myRotationDegrees.x = theValue;
        applyRotation();
    };

    Public.rotationY getter = function() {
        return _myRotationDegrees.y;
    };

    Public.rotationY setter = function(theValue) {
        _myRotationDegrees.y = theValue;
        applyRotation();
    };

    Public.rotationZ getter = function() {
        return _myRotationDegrees.z;
    };

    Public.rotationZ setter = function(theValue) {
        _myRotationDegrees.z = theValue;
        applyRotation();
    };

    Public.rotation getter = function() {
        return _myRotationDegrees.clone();
    };

    Public.rotation setter = function(theValue) {
        _myRotationDegrees.x = theValue.x;
        _myRotationDegrees.y = theValue.y;
        _myRotationDegrees.z = theValue.z;
        applyRotation();
    };

    function applyRotation() {
        var myRotation = new Vector3f(radFromDeg(_myRotationDegrees.x),
                                      radFromDeg(_myRotationDegrees.y),
                                      radFromDeg(_myRotationDegrees.z));

        var myQuaternion = Quaternionf.createFromEuler(myRotation);

        _mySceneNode.orientation = myQuaternion;
    };


    // ANIMATION HELPERS

    Public.animateProperty = function(theDuration, theEasing, theProperty, theStart, theEnd) {
        return new GUI.PropertyAnimation(theDuration, theEasing, Public, theProperty, theStart, theEnd);
    };

    Public.animateFadeIn = function(theDuration, theEasing) {
        if(!theDuration) {
            theDuration = 250.0;
        }
        if(!theEasing) {
            theEasing = null;
        }
        return Public.animateProperty(theDuration, theEasing, "alpha", 0.0, 1.0);
    };

    Public.animateFadeOut = function(theDuration, theEasing) {
        if(!theDuration) {
            theDuration = 250.0;
        }
        if(!theEasing) {
            theEasing = null;
        }
        return Public.animateProperty(theDuration, theEasing, "alpha", 1.0, 0.0);
    };



    Base.realize = Public.realize;
    Public.realize = function(theSceneNode) {
        _mySceneNode = theSceneNode;

        _mySceneNode.sticky = true;
        _mySceneNode.insensible = false;

        spark.sceneNodeMap[_mySceneNode.id] = Public;

        Base.realize();
                
        Public.visible = Protected.getBoolean("visible", true);

        Public.alpha = Protected.getNumber("alpha", 1.0);

        Public.position = new Vector3f(Protected.getNumber("x", 0.0),
                                       Protected.getNumber("y", 0.0),
                                       Protected.getNumber("z", 0.0));
        
        var myPosition = Protected.getArray("position", []);
        if(myPosition.length > 0) {
            Public.position = new Vector3f(myPosition);    
        }
                                       
        Public.scale = new Vector3f(Protected.getNumber("scaleX", 1.0),
                                    Protected.getNumber("scaleY", 1.0),
                                    Protected.getNumber("scaleZ", 1.0));

        Public.rotation = new Vector3f(Protected.getNumber("rotationX", 0.0),
                                       Protected.getNumber("rotationY", 0.0),
                                       Protected.getNumber("rotationZ", 0.0));
                                       
        Public.pivot = Protected.getVector3f("pivot", new Vector3f(0,0,0));

        Public.sensible = Protected.getBoolean("sensible", true);
        
    };
    
};


/**
 * Container wrapping a DOM transform.
 */
spark.Transform = spark.ComponentClass("Transform");

spark.Transform.Constructor = function(Protected) {
    var Base = {};
    var Public = this;
    
    this.Inherit(spark.Widget);
    
    Base.realize = Public.realize;
    Public.realize = function() {
        var myTransform = Modelling.createTransform(Public.parent.sceneNode, Public.name);
        Base.realize(myTransform);
    };

};


/**
 * Container that switches between a bunch of children.
 */
spark.Switch = spark.ComponentClass("Switch");

spark.Switch.Constructor = function(Protected) {
    var Base = {};
    var Public = this;
    
    this.Inherit(spark.Transform);

    var _myShown = null;

    Public.shown getter = function() {
        return _myShown;
    };

    Public.shown setter = function(theName) {
        var myWanted = Public.getChildByName(theName);
        if(myWanted) {
            _myShown = theName;

            var myChildren = Public.children;
            for(var i = 0; i < myChildren.length; i++) {
                var myChild = myChildren[i];

                myChild.visible = (myChild == myWanted);
            }
        }
    };
};


/**
 * Wrapper to Y60 worlds.
 * 
 * This gets the world of the one and only SceneViewer
 * and wraps it as a spark component.
 * 
 * Bottom line: use this as the toplevel component of your app.
 */
spark.World = spark.ComponentClass("World");

spark.World.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Widget);

    Base.realize = Public.realize;
    Public.realize = function() {
        Base.realize(window.scene.world);
    };
};

/**
 * Simple wrapper for the Y60 scene viewer.
 */
spark.SceneViewer = spark.ComponentClass("SceneViewer");

spark.SceneViewer.Constructor = function(Protected) {
    var Public = this;
    var Base = {};
    
    this.Inherit(spark.Widget);

    SceneViewer.prototype.Constructor(this, []);
    
    Base.realize = Public.realize;
    Public.realize = function() {
        window = new RenderWindow();
        
        window.position = [
            Protected.getNumber("positionX", 0),
            Protected.getNumber("positionY", 0)
        ];
        
        window.decorations = Protected.getBoolean("decorations", true);
        
        Public.setup(Protected.getNumber("width", 640),
                     Protected.getNumber("height", 480),
                     Protected.getBoolean("fullscreen", false),
                     Protected.getString("title", "SPARK Application"));
        
        window.swapInterval = Protected.getNumber("swapInterval", 1);
        
        spark.setupCameraOrtho(window.scene.dom.find(".//camera"), window.width, window.height);
        
        Base.realize(window.scene.world);
    };
    
    const PICK_RADIUS = 1;
    
    Public.pickWidget = function(theX, theY) {
        var myBody = Public.picking.pickBodyBySweepingSphereFromBodies(theX, theY, PICK_RADIUS, Public.sceneNode);
        if(myBody) {
            var myBodyId = myBody.id;
            if(myBodyId in spark.sceneNodeMap) {
                var myWidget = spark.sceneNodeMap[myBodyId];
                return myWidget;
            }
        }
        return null;
    };
    
    //////////////////////////////////////////////////////////////////////
    // Callbacks
    //////////////////////////////////////////////////////////////////////

    //  Will be called first in renderloop, has the time since application start
    Base.onFrame = Public.onFrame;
    Public.onFrame = function(theTime) {
        Base.onFrame(theTime);
    };

    // Will be called before rendering the frame
    Base.onPreRender = Public.onPreRender;
    Public.onPreRender = function() {
        Base.onPreRender();
    };

    // Will be called after rendering the frame, but before swap buffer
    Base.onPostRender = Public.onPostRender;
    Public.onPostRender = function() {
        Base.onPostRender();
    };

    // Will be called on a mouse move
    var _myMouseFocusedWidget = null;
    Base.onMouseMotion = Public.onMouseMotion;
    Public.onMouseMotion = function(theX, theY) {
        Base.onMouseMotion(theX, theY);
        
        var myWidget = Public.pickWidget(theX, theY);
        if(myWidget) {
            if(myWidget != _myMouseFocusedWidget) {
                Logger.info("Mouse focuses " + myWidget.name
                            + (_myMouseFocusedWidget ? ", leaving " + _myMouseFocusedWidget.name : ""));
                
                if(_myMouseFocusedWidget) {
                    var myLeaveEvent = new spark.MouseEvent(spark.MouseEvent.LEAVE, theX, theY);
                    _myMouseFocusedWidget.dispatchEvent(myLeaveEvent);
                }
                
                _myMouseFocusedWidget = myWidget;
                
                var myEnterEvent = new spark.MouseEvent(spark.MouseEvent.ENTER, theX, theY);
                myWidget.dispatchEvent(myEnterEvent);
            }
        } else {
            if(_myMouseFocusedWidget) {
                Logger.info("Mouse leaves " + _myMouseFocusedWidget.name);
                var myLeaveEvent = new spark.MouseEvent(spark.MouseEvent.LEAVE, theX, theY);
                _myMouseFocusedWidget.dispatchEvent(myLeaveEvent);
            }
        }
        _myMouseFocusedWidget = myWidget;
    };

    // Will be called on a mouse button
    Base.onMouseButton = Public.onMouseButton;
    Public.onMouseButton = function(theButton, theState, theX, theY) {
        Base.onMouseButton(theButton, theState, theX, theY);
        
        if(theState) {
            var myWidget = Public.pickWidget(theX, theY);
            if(myWidget) {
                Logger.info("Mouse clicks " + myWidget.name);
                var myEvent = new spark.MouseEvent(spark.MouseEvent.CLICK, theX, theY);
                myWidget.dispatchEvent(myEvent);
            }
        }
    };

    // Will be called on a keyboard event
    Base.onKey = Public.onKey;
    Public.onKey = function(theKey, theKeyState, theX, theY,
                         theShiftFlag, theControlFlag, theAltFlag) {
        Base.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theControlFlag, theAltFlag);
    };

    // Will be called on a mouse wheel event
    Base.onMouseWheel = Public.onMouseWheel;
    Public.onMouseWheel = function(theDeltaX, theDeltaY) {
        Base.onMouseWheel(theDeltaX, theDeltaY);
    };

    // Will be called on a joystick axis event
    Base.onAxis = Public.onAxis;
    Public.onAxis = function(device, axis, value) {
        Base.onAxis(device, axis, value);
    };

    // Will be called on a joystick button event
    Base.onButton = Public.onButton;
    Public.onButton = function(theDevice, theButton, theState) {
        Base.onButton(theDevice, theButton, theState);
    };

    // Will be called on a window reshape event
    Base.onResize = Public.onResize;
    Public.onResize = function(theNewWidth, theNewHeight) {
        Base.onResize(theNewWidth, theNewHeight);
    };

    // Will be called before exit
    Base.onExit = Public.onExit;
    Public.onExit = function() {
        Base.onExit();
    };
};

/**
 * Wrapper to Y60 bodies.
 * 
 * Used to wrap everything that is a body in the scene.
 */
spark.Body = spark.AbstractClass("Body");

spark.Body.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Widget);

    Base.propagateAlpha = Public.propagateAlpha;
    Public.propagateAlpha = function() {
        Base.propagateAlpha();
        
        // XXX: why this condition!?
        if (Public.sceneNode.nodeName == "body") {
            Modelling.setAlpha(Public.sceneNode, Public.actualAlpha);
        }
    };
};


/**
 * A simple Quad-image.
 * 
 * NOTE: does not adjust it's size when image is changed.
 */
spark.Image = spark.ComponentClass("Image");

spark.Image.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Body);

    var _myImage    = null;
    var _myTexture  = null;
    var _myMaterial = null;
    var _myShape    = null;
    var _myBody     = null;    
    
    Public.image getter = function() {
        return _myImage;
    };

    Public.image setter = function(theNode) {
        _myImage = theNode;
        _myTexture.image = theNode.id;
    };

    // XXX: this should not exist.
    Public.texture getter = function() {
        return _myTexture;
    };
    
    // XXX: this should not exist.
    Public.textureId setter = function(theTextureId) {
        _myMaterial.childNode("textureunits").firstChild.texture = theTextureId;
    };
    
    Protected.material getter = function() {
        return _myMaterial;
    }
    
    Protected.body getter = function() {
        return _myBody;
    }
    
    Protected.shape getter = function() {
        return _myShape;
    }

    Base.realize = Public.realize;
    Public.realize = function() {
        var myImageSource = Protected.getString("src", "");

        var myWidth;
        var myHeight;
        if(myImageSource == "") {
            myWidth  = Protected.getNumber("width");
            myHeight = Protected.getNumber("height");
            _myImage = Modelling.createImage(window.scene, myWidth, myHeight, "BGRA");
        } else {
            _myImage = spark.getCachedImage(myImageSource);
            myWidth  = Protected.getNumber("width" , _myImage.raster.width);
            myHeight = Protected.getNumber("height", _myImage.raster.height);
        }

        _myTexture  = Modelling.createTexture(window.scene, _myImage);
        _myTexture.name = Public.name + "-texture";
        _myTexture.wrapmode = "clamp_to_edge";

        _myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, _myTexture, Public.name + "-material", true);

        var mySize = new Vector3f(myWidth, myHeight, 0);
                               
        Protected.origin = Protected.getVector3f("origin", [0,0,0]);
        var myLowerLeft = new Vector3f(-Protected.origin.x,-Protected.origin.y,-Protected.origin.z);
        var myUpperRight = new Vector3f(mySize.x - Protected.origin.x, mySize.y - Protected.origin.y, mySize.z - Protected.origin.z);
        
        _myShape    = Modelling.createQuad(window.scene, _myMaterial.id, myLowerLeft, myUpperRight);
        _myShape.name = Public.name + "-shape";
        
        var myBody  = Modelling.createBody(Public.parent.sceneNode, _myShape.id);
        myBody.name = Public.name;
	
        Base.realize(myBody);        
    };
};


// XXX: I18N
spark.I18N = spark.ComponentClass("I18N");

const GERMAN  = "de";
const ENGLISH = "en";

spark.I18N.Constructor = function(Protected) {
    var Base = {};
    var Public = this;
    var _myActiveTag = "";

    var _myI18NComponents = [];
    
    this.Inherit(spark.Container);
    
    Public.language getter = function() {
        return _myActiveTag;
    }
    
    Public.language setter = function(theLanguage) {
        if (_myActiveTag != theLanguage) {
            _myActiveTag = theLanguage;
            Public.update();      
        }
    }

    Public.getActiveItem = function(theId) {
        var myResult = null;
        var myI18NItem = Public.node.getElementById(theId);
        if (myI18NItem) {
            var myLanguageNode = myI18NItem.childNode(_myActiveTag);
            myResult = myLanguageNode.childNode(0).nodeValue
        }
        return myResult;
    }
    
    Public.registerListener = function(theListener) {
        _myI18NComponents[theListener.name] = theListener;    
    }
    
    Base.realize = Public.realize;
    Public.realize = function() {
        Base.realize();
        _myActiveTag = Protected.getString("default", "de")
    }   
    Public.update = function() {
        for (var myI18NClientKey in _myI18NComponents) {
            _myI18NComponents[myI18NClientKey].updateI18N();
        }
    }    
}

spark.I18NItem = spark.ComponentClass("I18NItem");

spark.I18NItem.Constructor = function(Protected) {
    var Base = {};
    var Public = this;
   
    this.Inherit(spark.Container);


    Base.realize = Public.realize;
    Public.realize = function() {
        Base.realize();
    }   
    
    Public.instantiateChildren = function(theNode) {}
}



/**
 * A labelesque text component.
 */
spark.Text = spark.ComponentClass("Text");

spark.Text.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Body);

    var _myImage    = null;
    var _myTexture  = null;
    var _myMaterial = null;
    var _myShape    = null;
    var _myBody     = null;

    var _mySize  = null;
    var _myInitialSize = null;
    var _myStyle = null;
    var _myText  = "";
    
    Public.text getter = function() {
        return _myText;
    };

    Public.text setter = function(theValue) {
        _myText = theValue;
        render(_mySize);
        if(_myInitialSize != getImageSize(_myImage)) {
            var myXFactor = getImageSize(_myImage).x/_myInitialSize.x;
            var myYFactor = getImageSize(_myImage).y/_myInitialSize.y;
           _myBody.scale = new Vector3f(myXFactor, myYFactor, 1);
        }
    };
    
    Public.style getter = function() {
        return _myStyle;
    };

    function render(theSize) {
        spark.renderText(_myImage, _myText, _myStyle, theSize);
    }
    
    // XXX: I18N
    Public.updateI18N = function() {   
        Public.text = Protected.realizeI18N(_myText);
    }        
    
    Base.realize = Public.realize;
    Public.realize = function() {
        // size is mandatory
        _mySize = new Vector2f(Protected.getNumber("width"),
                               Protected.getNumber("height"));
        
        // retrieve text
        _myText = Protected.getString("text", "");
        
        // handle internationalization
        _myTextId = Protected.getString("textId", "");
        if(_myTextId != "") {
            var myI18nText = Protected.getI18nText(_myTextId);
            if(myI18nText) {
                _myText = myI18nText;
            }
        }
        
        // extract font style
        _myStyle = spark.fontStyleFromNode(Public.node);
        
        // construct scene representation
        _myImage      = spark.createTextImage(_mySize);
        _myImage.name = Public.name + "-image"; // XXX: must have name
        
        _myTexture          = Modelling.createTexture(window.scene, _myImage);
        _myTexture.name     = Public.name + "-texture"; // XXX: must have name
        _myTexture.wrapmode = "clamp_to_edge";
        
        _myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, _myTexture, Public.name + "-material", true); // XXX: must have name
        
        _myShape      = Modelling.createQuad(window.scene, _myMaterial.id, new Vector3f(0,0,0),
                                             new Vector3f(_myInitialSize.x, _myInitialSize.y, 0));
        _myShape.name = Public.name + "-shape"; // XXX: must have name

        _myBody      = Modelling.createBody(Public.parent.sceneNode, _myShape.id);
        _myBody.name = Public.name; // XXX: must have name
        
        // finally, render the text
        render();

        // initialize upwards
        Base.realize(_myBody);
    };
};


/**
 * And even less done... movie nodes.
 */
spark.Movie = spark.ComponentClass("Movie");

spark.Movie.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Body);

    var _myMovie    = null;
    var _myTexture  = null;
    var _myMaterial = null;
    var _myShape    = null;
    var _myBody     = null;    

    Public.play = function() {
        _myMovie.playmode = "play";
    };
    
    Public.stop = function() {
        _myMovie.playmode = "stop";
    };

    Public.loopcount getter = function() {
        return _myMovie.loopcount;
    };

    Public.loopcount setter = function(theCount) {
        _myMovie.loopcount = theCount;
    };

    Base.realize = Public.realize;
    Public.realize = function() {
        var myMovieSource = Protected.getString("src");

        setupMovie(myMovieSource);

        _myTexture  = Modelling.createTexture(window.scene, _myMovie);
        _myTexture.name = Public.name + "-texture";
        _myTexture.wrapmode = "clamp_to_edge";

        _myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, _myTexture, Public.name + "-material", true);
        
        var mySize = new Vector3f(_myMovie.width,_myMovie.height, 0);
        Protected.origin = Protected.getVector3f("origin", [0,0,0]);
        var myLowerLeft = new Vector3f(-Protected.origin.x,-Protected.origin.y,-Protected.origin.z);
        var myUpperRight = new Vector3f(mySize.x - Protected.origin.x, mySize.y - Protected.origin.y, mySize.z - Protected.origin.z);
                
        _myShape    = Modelling.createQuad(window.scene, _myMaterial.id, myLowerLeft, myUpperRight);
        _myShape.name = Public.name + "-shape";

        var myBody  = Modelling.createBody(Public.parent.sceneNode, _myShape.id);
        myBody.name = Public.name;
	
        Base.realize(myBody);
    };

    function setupMovie(theFilename) {
        _myMovie = Node.createElement("movie");

        window.scene.images.appendChild(_myMovie);

        _myMovie.src = theFilename;
        _myMovie.name = Public.name + "-movie";
        _myMovie.resize = "none";
        _myMovie.loopcount = 1;
        _myMovie.playmode = "stop";

        window.scene.loadMovieFrame(_myMovie);
    };

};

// XXX: revisit. good idea though.
spark.Model = spark.ComponentClass("Model");

spark.Model.Constructor = function(Protected) {
    var Base = {};
    var Public = this;
    
    this.Inherit(spark.Body);
        
    Base.realize = Public.realize;
    Public.realize = function() {
        var myY60Name = Protected.getString("rootName");  
        var myY60DomObject = window.scene.dom.find("//*[@name='" + myY60Name + "']");
        var myPosition = new Vector3f(myY60DomObject.position);
        var myOrientation = new Quaternionf(myY60DomObject.orientation);
        var myScale = new Vector3f(myY60DomObject.scale);
        Base.realize(myY60DomObject);            
        Public.position = myPosition;
        Public.orientation = myOrientation;
        Public.scale = myScale;
    }
    
    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        Base.postRealize();
    }
    
    Base.propagateAlpha = Public.propagateAlpha;
    Public.propagateAlpha = function() {
        Base.propagateAlpha();
        propagateAlpha(Public.sceneNode, Public.actualAlpha);
    };
    
    function propagateAlpha(theNode, theAlpha) {
        if (theNode.nodeName == "body") {
            Modelling.setAlpha(theNode, theAlpha);
        }
        for (var i = 0; i <theNode.childNodesLength(); i++) {
            propagateAlpha(theNode.childNode(i), theAlpha);
        }        
    }    
}


if("useQuirkySparkWidgets" in this) {

// XXX: non-generic component
spark.Button = spark.ComponentClass("Button");

spark.Button.Constructor = function(Protected) {
    var Base = {};
    var Public = this;
    this.Inherit(spark.Transform);    
    Base.realize = Public.realize;
    Public.realize = function() {
        Base.realize();
    }
    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        Base.postRealize();
        for(var i = 0; i < Public.children.length; i++) {
            var myChild = Public.children[i];
            if (myChild.sensible) {
                Logger.info("register onClick for: " + myChild.name);
                spark.onClickListeners[myChild.sceneNode.id]      = Public;
                spark.onReleaseListeners[myChild.sceneNode.id]    = Public;
            }
        }
        
    }    
    Base.onClick = Public.onClick;
    Public.onClick = function (theButton, thePickedBodyId, theButtonState, theX, theY) {
        Base.onClick(theButton, thePickedBodyId, theButtonState, theX, theY);
        if(Public.isPressed(LEFT_BUTTON)) {
            Logger.info("Button::onCLick, pressed: " + Public.name);
        }
    }
    
}

// XXX: non-generic component
spark.LanguageButton = spark.ComponentClass("LanguageButton");

spark.LanguageButton.Constructor = function(Protected) {
    var Base = {};
    var Public = this;
    var _mySelectedLanguageWidgets = {};
    var _myUnSelectedLanguageWidgets = {};
    var _myCurtain = null;
    var _myTriggeredLanguage = null;
    var _myOnChangeCBs = new Array();
    this.Inherit(spark.Transform);    
    
    Public.registerOnChangeCB = function(theCB) {
        _myOnChangeCBs.push(theCB);        
    }
    Base.realize = Public.realize;
    Public.realize = function() {
        Base.realize();
    }
    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        Base.postRealize();

        // overwrite stuff
        var myTransform = Public.getChildByName("selected");
        for(var ii = 0; ii < myTransform.children.length; ii++) {
            var myChild = myTransform.children[ii];
            _mySelectedLanguageWidgets[myChild.node.lang] = myChild
            spark.onClickListeners[myChild.sceneNode.id]      = Public;
            spark.onReleaseListeners[myChild.sceneNode.id]    = Public;
        }
        var myTransform = Public.getChildByName("unselected");
        for(var ii = 0; ii < myTransform.children.length; ii++) {
            var myChild = myTransform.children[ii];
            _myUnSelectedLanguageWidgets[myChild.node.lang] = myChild
            spark.onClickListeners[myChild.sceneNode.id]      = Public;
            spark.onReleaseListeners[myChild.sceneNode.id]    = Public;
        }
        _myCurtain = Public.getChildByName("curtain");
        Public.update(GERMAN, true);
    }
    
    Base.onClick = Public.onClick;
    Public.onClick = function (theButton, thePickedBodyId, theButtonState, theX, theY) {
        Base.onClick(theButton, thePickedBodyId, theButtonState, theX, theY);
        if(Public.isPressed(LEFT_BUTTON)) {
            var mySparkObject = spark.sceneNodeMap[thePickedBodyId];
            if (_myTriggeredLanguage != mySparkObject.node.lang) {
                Public.update(mySparkObject.node.lang);
            }
            if (ourRemoteData) {
                ourRemoteData.set("currentlanguage", mySparkObject.node.lang);
            }
        }
    }
    Public.update = function(theLanguage, theEnforceFlag) {
        var myLanguageChangeFlag = false;
        var myI18Ns = Protected.getArray("I18NObjects", []);
        if (theEnforceFlag == undefined) {
            for (var i = 0; i < myI18Ns.length; i++) {
                var myI18N = spark.ourComponentsByNameMap[myI18Ns[i]];
                if (myI18N) {
                    if (theLanguage != myI18N.language) {
                        myLanguageChangeFlag = true;
                    }
                }
            }
        } else {
            myLanguageChangeFlag = theEnforceFlag;
        }
        if (myLanguageChangeFlag && _myCurtain) {
            var mySeqAnimation = new GUI.SequenceAnimation(); 
            var myInAnimation = new GUI.PropertyAnimation(200, null, _myCurtain, "alpha", 0, 1); 
            if (theLanguage == ENGLISH) {
                    _mySelectedLanguageWidgets[ENGLISH].visible  = true; 
                    _myUnSelectedLanguageWidgets[GERMAN].visible = true;
                    _myUnSelectedLanguageWidgets[ENGLISH].visible = false;   
                    _mySelectedLanguageWidgets[GERMAN].visible    = false;    
                } else {
                    _mySelectedLanguageWidgets[ENGLISH].visible  = false; 
                    _myUnSelectedLanguageWidgets[GERMAN].visible = false;
                    _myUnSelectedLanguageWidgets[ENGLISH].visible = true;   
                    _mySelectedLanguageWidgets[GERMAN].visible    = true;  
            }
            myInAnimation.onFinish = function() {
                for (var i = 0; i < myI18Ns.length; i++) {
                    var myI18N = spark.ourComponentsByNameMap[myI18Ns[i]];
                    if (myI18N) {
                        myI18N.language = theLanguage
                        
                        for (var ii = 0; ii < _myOnChangeCBs.length; ii++) {
                            _myOnChangeCBs[ii](theLanguage);
                        }
                    } else {
                        Logger.warning("Sorry, could not find I18N:'" + myI18Ns[i] + "' object to toggle language!");
                    }                
                }
            }
            mySeqAnimation.add(myInAnimation);
            var myOutAnimation = new GUI.PropertyAnimation(200, null, _myCurtain, "alpha", 1, 0); 
            mySeqAnimation.add(myOutAnimation);
            playAnimation(mySeqAnimation);
            _myTriggeredLanguage = theLanguage;            
        }
    }

}

}
