
/**
 * Component base class.
 * 
 * Components are XML-initializable objects organized
 * in a hierarchy. They can be graphical or not.
 */

spark.sceneNodeMap = {};
spark.ourComponentsByNameMap = {};
spark.onClickListeners = {};
spark.onMoveListeners = {};
spark.onReleaseListeners = {};

spark.Component = spark.AbstractClass("Component");

spark.Component.Constructor = function(Protected) {
    var Public = this;

    var _myName = null;
    
    Public.name getter = function() {
        return _myName;
    };


    Public.toString = function (){
        return _myName;
    }
    var _myParent = null;

    Public.parent getter = function() {
        return _myParent;
    };

    Public.parent setter = function(theParent) {
        _myParent = theParent;
    };

    
    var _myNode = null;
    
    Public.node getter = function() {
        return _myNode;
    };


    Public.initialize = function(theNode) {
        _myNode   = theNode;
        _myName   = Protected.getString("name", "");
    };

    Public.realize = function() {
        spark.ourComponentsByNameMap[Public.name] = Public;
        Logger.info("Realizing component named " + Public.name);
    };
    
    Public.postRealize = function() {
        Logger.info("Post-Realizing component named " + Public.name);
    };

    Public.instantiateChildren = function() {
    };

    Protected.getString = function(theName, theDefault) {
        if(theName in _myNode) {
            return _myNode[theName];
        } else {
            if(theDefault == null) {
                Logger.error(_myNode.nodeName + " requires attribute " + theName);
            } else {
                return theDefault;
            }
        }
    };
    
    Public.getString = Protected.getString;
    
    Public.acquisition = function(theComponentTypeName, theItemId) {
        return (_myNode.nodeName == theComponentTypeName &&  Public.node.getElementById(theItemId)) ? _myNode:null;
    }
                
    Protected.realizeI18N = function(theItem, theAttribute) {
        var myConcreteItem = theItem;
        var myId = "";
        if (theAttribute == undefined) {
            myId = Protected.getString("multilanguageId", "");         
        } else {
            myId = Protected.getString(theAttribute, "");         
        }        
        if (myId != "") {
            // find multilanguage item
            var myI18N = Public.acquisition("I18N", myId);
            if (myI18N) {
                myConcreteItem = myI18N.getActiveItem(myId);
                if (myConcreteItem) {
                    myI18N.registerListener(Public);
                }
            }
        }
        return myConcreteItem;
    }
    
    Protected.getBoolean = function(theName, theDefault) {
        if(theName in _myNode) {
            return (_myNode[theName] == "true");
        } else {
            if(theDefault == null) {
                Logger.error(_myNode.nodeName + " requires attribute " + theName);
            } else {
                return theDefault;
            }
        }
    };
    
    Protected.getVector3f = function(theName, theDefault) {
        var myArray = Protected.getArray(theName, theDefault);
        if (myArray.length == 3) {
            return new Vector3f(myArray[0], myArray[1], myArray[2]);
        } else {
            return theDefault;
        }
    };

    Protected.getNumber = function(theName, theDefault) {
        if(theName in _myNode) {
            return Number(_myNode[theName]);
        } else {
            if(theDefault == null) {
                Logger.error(_myNode.nodeName + " requires attribute " + theName);
            } else {
                return theDefault;
            }
        }
    };
    
    Protected.getArray = function(theName, theDefault) {
        if(theName in _myNode) {
            return _myNode[theName].substring(1, _myNode[theName].length -1).replace(/ /g,"").split(",");
        } else {
            if(theDefault == null) {
                Logger.error(_myNode.nodeName + " requires attribute " + theName);
            } else {
                return theDefault;
            }
        }
    };

};


/**
 * Containers, obviously, contain other components.
 * 
 * This class is abstract because there can be different
 * kinds of containers, mostly distinguishing between
 * graphical and non-graphical ones.
 */

spark.Container = spark.AbstractClass("Container");

spark.Container.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Component);

    var _myChildArray = [];
    var _myChildMap   = {};

    Public.children getter = function() {
        return _myChildArray; // XXX: clone?
    };

    Public.addChild = function(theChild) {
        _myChildArray.push(theChild);
        if(theChild.name) {
            _myChildMap[theChild.name] = theChild;
        }
        theChild.parent = Public;
    };

    Public.getChildAt = function(theIndex) {
        return _myChildArray[theIndex];
    };

    Public.getChildByName = function(theName) {
        if (theName in _myChildMap) {
            return _myChildMap[theName];
        } else {
            return null;
        }
    };

    Public.acquisition = function(theComponentTypeName, theItemId) {
        if (Public.node.nodeName == theComponentTypeName &&  
            (theItemId==undefined || Public.node.getElementById(theItemId))) {
            return Public.node;
        } 
        for(var i = 0; i < _myChildArray.length; i++) {
           if (_myChildArray[i].node.nodeName == theComponentTypeName  &&  
               (theItemId==undefined || _myChildArray[i].node.getElementById(theItemId))) {
                return _myChildArray[i];
            }       
        }
        if (Public.parent) {
            return Public.parent.acquisition(theComponentTypeName, theItemId);
        } else {
            return null;
        }
    }

    /*Public.getChildByType = function(theType) {
        return _myChildMap[theName];
    };*/
    Base.realize = Public.realize;
    Public.realize = function() {
        Base.realize();
        for(var i = 0; i < _myChildArray.length; i++) {
            _myChildArray[i].realize();
        }
    };
    
    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        Base.postRealize();
        for(var i = 0; i < _myChildArray.length; i++) {
            _myChildArray[i].postRealize();
        }
    };

    Public.instantiateChildren = function(theNode) {
        for(var i = 0; i < theNode.childNodesLength(); i++) {
            var myChildNode = theNode.childNode(i);
            spark.instantiateRecursively(myChildNode, Public);
        }
    };
};


/**
 * Scene nodes wrap Y60 scene objects.
 * 
 * This is where positioning, orientation, scaling,
 * visibility and sensibility go.
 */



spark.Widget = spark.AbstractClass("Widget");

spark.Widget.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Container);

    var _mySceneNode   = null;
    var _myPressedFlag = new Array();
    Public.sceneNode getter = function() {
        return _mySceneNode;
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
    
    Public.isPressed = function(theButton) {
        return _myPressedFlag[theButton];
    }
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
        if(Public.parent) {
            Public.parent.propagateSensibility();
        }
    };

    Public.propagateSensibility = function(){
        var mySensibility = Public.sensible;
        for(var i = 0; i < Public.children.length; i++) {
            var myChild = Public.children[i];
            if ("sensible" in myChild) {
                mySensibility |= myChild.sensible;
            }
        }
        Public.sensible = mySensibility;
    }
    // SIZE
    
    Public.size getter = function() {
        var myBoundingBox = _mySceneNode.boundingbox;
        var myWidth = 0;
        if(myBoundingBox == "[]"){
            Logger.warning("BoundingBox not initialized - size getter not yet implemented")
            return new Vector2f(0,0);
        } else {
            return myBoundingBox.size;
        }
        
    }
    
    Public.width getter = function(){
        return Public.size.x;
    }
    
    Public.height getter = function(){
        return Public.size.y;
    }
    
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

    Public.worldposition getter = function() {
        return _mySceneNode.globalmatrix.getTranslation();
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


    // #
    
    Public.pivot getter = function() {
        return _mySceneNode.pivot.clone();
    };

    Public.pivot setter = function(theValue) {
        _mySceneNode.pivot.x = theValue.x;
        _mySceneNode.pivot.y = theValue.y;
        _mySceneNode.pivot.z = theValue.z;
    };

    // ORIGIN
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
        _myPressedFlag[LEFT_BUTTON]   = false;    
        _myPressedFlag[MIDDLE_BUTTON] = false;    
        _myPressedFlag[RIGHT_BUTTON]  = false;    
        
        _mySceneNode = theSceneNode;

        _mySceneNode.sticky = true;
        _mySceneNode.insensible = true;

        spark.sceneNodeMap[_mySceneNode.id] = Public;

        Base.realize();
                
        Public.visible = Protected.getBoolean("visible", true);

        Public.alpha = Protected.getNumber("alpha", 1.0);

        Public.position = new Vector3f(Protected.getNumber("x", 0.0),
                                       Protected.getNumber("y", 0.0),
                                       Protected.getNumber("z", 0.0));
                                       
        var myPos = Protected.getArray("pos", []);
        if(myPos.length > 0) {
            Public.position = new Vector3f(myPos);    
        }
                                       
        Public.scale = new Vector3f(Protected.getNumber("scaleX", 1.0),
                                    Protected.getNumber("scaleY", 1.0),
                                    Protected.getNumber("scaleZ", 1.0));

        Public.rotation = new Vector3f(Protected.getNumber("rotationX", 0.0),
                                       Protected.getNumber("rotationY", 0.0),
                                       Protected.getNumber("rotationZ", 0.0));
                                       
        Public.pivot = Protected.getVector3f("pivot", new Vector3f(0,0,0));
        
    };
    
    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        Public.sensible = Protected.getBoolean("sensible", false);
        /*if (Public.sensible) {
            spark.onClickListeners[Public.sceneNode.id] = Public;
            spark.onReleaseListeners[Public.sceneNode.id] = Public;                        
            spark.onMoveListeners[Public.sceneNode.id] = Public;
        }*/
        Base.postRealize();
        
    };  
      
    Public.onClick = function (theButton, thePickedBodyId, theButtonState, theX, theY) {
        _myPressedFlag[theButton] = theButtonState;
        if (theButtonState) {
            Logger.info("Widget('" + Public.name + "') ::onClick() " + " " + theButton + " " + theButtonState + " " + theX + " " + theY)        
        }
    }
    
    Public.onMove = function (theX, theY) {
        Logger.info("Widget('" + Public.name + "') ::onMove() " + " " + theX + " " + theY)        
    }

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
 * UNTESTED: Container that switches between a bunch of children.
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

spark.Body = spark.AbstractClass("Body");

spark.Body.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Widget);

    Base.propagateAlpha = Public.propagateAlpha;
    Public.propagateAlpha = function() {
        Base.propagateAlpha();
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
        var myImageSource = Protected.getString("src");
        
        _myImage    = spark.getCachedImage(myImageSource);

        _myTexture  = Modelling.createTexture(window.scene, _myImage);
        _myTexture.name = Public.name + "-texture";
        _myTexture.wrapmode = "clamp_to_edge";

        _myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, _myTexture, Public.name + "-material", true);

        var mySize = new Vector3f(Protected.getNumber("width" , _myImage.raster.width),
                               Protected.getNumber("height", _myImage.raster.height), 0);
                               
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
    
    Public.updateI18N = function() {   
        Public.text = Protected.realizeI18N(_myText);
    }        
    
    Base.realize = Public.realize;
    Public.realize = function() {
        // get attributes
        _mySize = new Vector2f(Protected.getNumber("width", 0),
                               Protected.getNumber("height", 0));
                               
        // retrieve text
        _myText = Protected.getString("text", "");
        // maybe overwrite with a language dependent version        
        _myText = Protected.realizeI18N(_myText);
        
        _myStyle = spark.fontStyleFromNode(Public.node);
        // construct scene
        _myImage    = spark.createTextImage(_mySize);
        _myImage.name = Public.name + "-image";
        _myTexture  = Modelling.createTexture(window.scene, _myImage);
        _myTexture.name = Public.name + "-texture";
        _myTexture.wrapmode = "clamp_to_edge";
        
        // render
        render();
        _myInitialSize = getImageSize(_myImage);
        if(_mySize)
        _myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, _myTexture, Public.name + "-material", true);
        
        _myShape    = Modelling.createQuad(window.scene, _myMaterial.id, new Vector3f(0,0,0),
                                           new Vector3f(_myInitialSize.x, _myInitialSize.y, 0));
        _myShape.name = Public.name + "-shape";

        _myBody  = Modelling.createBody(Public.parent.sceneNode, _myShape.id);
        _myBody.name = Public.name;
        
        // render
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


spark.SliderImpl = spark.ComponentClass("SliderImpl");

spark.SliderImpl.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    const CURSOR_TEXTURE_OFFSET = 15;
    
    var _mySliderFrame = null;
    var _mySliderBg = null;
    var _myCursor = null;
    var _myHeadline = null;
    var _myStartValueText = null;
    var _myEndValueText = null;
    var _myUnit = null;
    var _mySingleUnit = null;
    var _myText = null;
    var _myCursorLabel = null;
    var _myCurrentValue = 0;
    var _myDistance = 0;
    var _myRasterValues = [];
    var _myInitialValue = 0;
    var _myCursorLabelText = null;
    var _myMinValue = null;
    var _myIndex = null;
    
    this.Inherit(spark.Transform);

    Public.value getter = function () {
        return _myCurrentValue;
    }

    Public.sliderPosition setter = function(thePosition) {
        
        var myMaxPos = getImageSize(_mySliderBg.image).x - _myCursor.size.x + CURSOR_TEXTURE_OFFSET;

        _myCursor.position.x = thePosition;
        _myCursor.position.x = clamp(_myCursor.position.x, -CURSOR_TEXTURE_OFFSET, myMaxPos);
        
        var myRange = getImageSize(_mySliderBg.image).x - _myCursor.size.x + 2*CURSOR_TEXTURE_OFFSET;
        _myCurrentValue = (_myCursor.position.x + CURSOR_TEXTURE_OFFSET) / myRange;
        
        if (_myRasterValues.length > 0) {
            _myIndex = Number((_myCursor.position.x + _myCursor.size.x/2) / _myDistance);
            _myIndex = Math.round(_myIndex);
            _myIndex --;
            _myCurrentValue = _myRasterValues[_myIndex][0];
            _myCursor.position.x = _myRasterValues[_myIndex][1];
        }

        updateCursorLabel();
        
        if (ourRemoteData) {
            var myGuiId = Protected.getString("guiid", "");
            if (myGuiId != "") {
                ourRemoteData.set(myGuiId, _myCurrentValue);
            }
        }      
    }

    Public.sliderPosition getter = function() {
        return _myCursor.position.x;
    } 
    
    Public.reset = function () {
        var myRange = getImageSize(_mySliderBg.image).x - _myCursor.size.x + 2*CURSOR_TEXTURE_OFFSET;
        var myNewPos = Math.floor(_myInitialValue * myRange) - CURSOR_TEXTURE_OFFSET;
        
        if (_myRasterValues.length > 0) {
            for (var i=0; i<_myRasterValues.length; i++) {
                    if (_myInitialValue == _myRasterValues[i][0]) {
                        _myIndex = i;
                    }
                }   
                myNewPos = _myRasterValues[_myIndex][1];
        }
        var myCursorAnimation = new GUI.PropertyAnimation(500, null, Public, "sliderPosition", Public.sliderPosition, myNewPos);
        playAnimation(myCursorAnimation);
    }
    
    
    Base.realize = Public.realize;
    Public.realize = function() {
        Base.realize();
        
        _mySliderFrame = Public.getChildByName("SliderFrame");
        _mySliderBg = _mySliderFrame.getChildByName("SliderBg");
        _myCursor = _mySliderBg.getChildByName("Cursor");
        _myCursorLabel = _mySliderBg.getChildByName("CursorLabel");
        
        var myText = null;
        _myText = _mySliderFrame.getChildByName("Text");
        if (_myText) {
            myText = Protected.getString("Text", "");
            myText = Protected.realizeI18N(myText, "Text_Id");
            if(myText != "") {
                _myText.text = myText;    
            } else {
                _myText.visible = false;
            }
        }
        
        _myHeadline = _mySliderFrame.getChildByName("Headline");
        if (_myHeadline) {
            myText = Protected.getString("Headline", "");
            myText = Protected.realizeI18N(myText, "Headline_Id");
            if(myText != "") {
                _myHeadline.text = myText;    
            } else {
                _myHeadline.visible = false;
            }
        }
        
        _myStartValueText = _mySliderFrame.getChildByName("StartText");
        if (_myStartValueText) {
            myText = Protected.getString("StartText", "");
            myText = Protected.realizeI18N(myText, "StartText_Id");
            if(myText != "") {
                _myStartValueText.text = myText;    
            } else {
                _myStartValueText.visible = false;
            }
        }
        
        _myEndValueText = _mySliderFrame.getChildByName("EndText");
        if (_myEndValueText) {
            myText = Protected.getString("EndText", "");
            myText = Protected.realizeI18N(myText, "EndText_Id");
            if(myText != "") {
                _myEndValueText.text = myText;    
            } else {
                _myEndValueText.visible = false;
            }
        }
        
        _myUnit = Protected.getString("Unit", "");
        _myUnit = Protected.realizeI18N(_myUnit, "Unit_Id");
        
        _mySingleUnit = Protected.getString("SingleUnit", "");
        _mySingleUnit = Protected.realizeI18N(_mySingleUnit, "SingleUnit_Id");
        
        _myMinValue = Protected.getString("MinValue", "");
        _myMinValue = Protected.realizeI18N(_myMinValue, "MinValue_Id");
        
        _myCurrentValue = Protected.getNumber("initValue", 0);
        _myInitialValue = _myCurrentValue; 
        
         var myRange = getImageSize(_mySliderBg.image).x - _myCursor.size.x + 2*CURSOR_TEXTURE_OFFSET;
        _myCursor.position.x = Math.floor(_myCurrentValue * myRange) - CURSOR_TEXTURE_OFFSET;
        
        var myRasterPoints = Protected.getArray("rastervalues", []);
        if (myRasterPoints.length > 0) {
            _myDistance = (getImageSize(_mySliderBg.image).x - _myCursor.size.x +2*CURSOR_TEXTURE_OFFSET) / (myRasterPoints.length-1);
            var i =0;
            for (i=0; i< myRasterPoints.length; i++) { 
                _myRasterValues[i] = [];
                _myRasterValues[i][0] = Number(myRasterPoints[i]);
                _myRasterValues[i][1] = i*_myDistance - CURSOR_TEXTURE_OFFSET;
            }
            for (i=0; i<_myRasterValues.length; i++) {
                if (_myInitialValue == _myRasterValues[i][0]) {
                    _myIndex = i;
                }
            }   
            _myCursor.position.x = _myRasterValues[_myIndex][1];
        }
        updateCursorLabel();
    };

    Public.updateI18N = function() {   
        if (_myText) {     
            var myText = Protected.realizeI18N(_myText.text, "Text_Id");
            _myText.text = myText; 
        }
        if (_myMinValue) {
            _myMinValue = Protected.realizeI18N(_myMinValue, "MinValue_Id");
        }
        if (_mySingleUnit) {
            _mySingleUnit = Protected.realizeI18N(_mySingleUnit, "SingleUnit_Id");
        }
        if (_myUnit) {
            _myUnit = Protected.realizeI18N(_myUnit, "Unit_Id");
            updateCursorLabel();
        }
        if (_myHeadline) {   
            myText = Protected.realizeI18N(_myHeadline.text, "Headline_Id");
            _myHeadline.text = myText;
        }
        if (_myStartValueText) {    
            myText = Protected.realizeI18N(_myStartValueText.text, "StartText_Id");
            _myStartValueText.text = myText;   
        }
        if (_myEndValueText) {
            myText = Protected.realizeI18N(_myEndValueText.text, "EndText_Id");
            _myEndValueText.text = myText;   
        } 
    }
    
    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        Base.postRealize();
        // overwrite widgets automagic callback to be handles by this
        spark.onClickListeners[_myCursor.sceneNode.id]      = Public;
        spark.onClickListeners[_mySliderBg.sceneNode.id] = Public;
        spark.onMoveListeners[Public.sceneNode.id]          = Public;
        spark.onReleaseListeners[_myCursor.sceneNode.id]    = Public;
    }
    
    Base.onClick = Public.onClick;
    Public.onClick = function (theButton, thePickedBodyId, theButtonState, theX, theY) {
        Base.onClick(theButton, thePickedBodyId, theButtonState, theX, theY);
        if (theButtonState) {
            moveCursor(theX);
        }
    }

    Public.onMove = function (theX, theY) {
        if(Public.isPressed(LEFT_BUTTON)) {
            moveCursor(theX);
        }
    }
    
    function moveCursor(theX) {
        var myMovement = theX - _myCursor.worldposition.x - _myCursor.size.x/2;
        Public.sliderPosition += myMovement;
    }
    
    function updateCursorLabel(){
        if (_myCursorLabel) {  
            var myId = Protected.getString("Text_Id", "");                     
            var myI18N = Public.acquisition("I18N", myId);
            var myNewValue = String(_myCurrentValue);
            if (myI18N.language == "en") {
                myNewValue = myNewValue.replace(/\,/, "."); 
            } else {
                myNewValue = myNewValue.replace(/\./, ",");
            }  
            if (_myIndex == 0 && _myMinValue) {
                _myCursorLabel.text = _myMinValue;
            } else if (_myCurrentValue == 1 && _mySingleUnit) {
                _myCursorLabel.text = myNewValue + " " +  _mySingleUnit;
            } else {
                _myCursorLabel.text = myNewValue + " " +  _myUnit; 
            }
            var myMax = getImageSize(_mySliderBg.image).x - _myCursorLabel.width;
            _myCursorLabel.position.x = _myCursor.position.x + _myCursor.width/2 - _myCursorLabel.width/2;
            _myCursorLabel.position.x = clamp( _myCursorLabel.position.x, 0, myMax);
        }
    }
    
}

spark.Model = spark.ComponentClass("Model");

spark.Model.Constructor = function(Protected) {
    var Base = {};
    var Public = this;
    this.Inherit(spark.Body);    
    Base.realize = Public.realize;
    Public.realize = function() {
        var myY60Name = Protected.getString("y60_name", "");  
        if (myY60Name != "") {                           
            var myY60DomObject = window.scene.dom.find("//*[@name='" + myY60Name + "']");
            var myPosition = new Vector3f(myY60DomObject.position);
            var myOrientation = new Quaternionf(myY60DomObject.orientation);
            var myScale = new Vector3f(myY60DomObject.scale);
            Base.realize(myY60DomObject);            
            Public.position = myPosition;
            Public.orientation = myOrientation;
            Public.scale = myScale;
        }
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
