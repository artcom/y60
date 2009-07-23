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
// Description: Standard widgets
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

    this.Inherit(spark.Container);
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


    // STAGE

    Public.stage getter = function() {
        var myCurrent = Public;
        while(myCurrent) {
            if("Stage" in myCurrent._classes_) {
                return myCurrent;
            }
            myCurrent = myCurrent.parent;
        }
        Logger.fatal("Widget " + Public.name + " is not the child of a valid stage.");
    };

    
    // STAGE EVENTS

    Base.addEventListener = Public.addEventListener;
    Public.addEventListener = function(theType, theListener, theUseCapture) {
        if(!("Stage" in Public._classes_)) {
            switch(theType) {
            case spark.StageEvent.FRAME:
            case spark.StageEvent.PRE_RENDER:
            case spark.StageEvent.POST_RENDER:
                if(theUseCapture) {
                    Logger.fatal("Capture of stage events is forbidden.");
                }
                Public.stage.addEventListener(
                    theType,
                    function(theEvent) {
                        Public.dispatchEvent(theEvent);
                    },
                    false
                );
                break;
            }
        }
        Base.addEventListener(theType, theListener, theUseCapture);
    };

    Base.removeEventListener = Public.removeEventListener;
    Public.removeEventListener = function(theType, theListener, theUseCapture) {
        switch(theType) {
        case spark.StageEvent.FRAME:
        case spark.StageEvent.PRE_RENDER:
        case spark.StageEvent.POST_RENDER:
            Logger.fatal("Removal of stage event listeners is not supported");
            break;
        default:
            Base.removeEventListener(theType, theListener, theUseCapture);
            break;
        }
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
    
    
    // VISIBILITY AND SENSIBLITY

    Public.visible getter = function() {
        return _mySceneNode.visible;
    };

    Public.visible setter = function(theValue) {
        _mySceneNode.visible = theValue;
        Public.sensible = Public.sensible;
    };

    var _myRealSensiblity = true;

    Public.sensible getter = function() {
        return _myRealSensiblity;
    };

    Public.sensible setter = function(theValue) {
        _myRealSensiblity = theValue;

        var myActualSensibility = theValue && _mySceneNode.visible;

        _mySceneNode.insensible = !myActualSensibility;
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
        _mySceneNode.position.x = theValue.x;
        _mySceneNode.position.y = theValue.y;
        _mySceneNode.position.z = theValue.z;
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
    // XXX: origins must be set up before realization

    var _myOrigin = new Vector3f(0,0,0);
    
    Public.originX getter = function() {
        return _myOrigin.x;
    };

    Public.originY getter = function() {
        return _myOrigin.y;
    };

    Public.originZ getter = function() {
        return _myOrigin.z;
    };

    Public.origin getter = function() {
        return new Vector3f(_myOrigin);
    };

    Public.origin setter = function(theValue) {
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

    // INTERNATIONALISATION HOOKS

    var _myI18nContext = null;

    Public.i18nContext getter = function() {
        if(_myI18nContext) {
            return _myI18nContext;
        } else {
            if(Public.parent) {
                return Public.parent.i18nContext;
            } else {
                return null;
            }
        }
    };

    Public.i18nContext setter = function(theValue) {
        _myI18nContext = theValue; // XXX: re-trigger i18n events? how?
    };

    Public.i18nContexts getter = function() {
        var myContexts = [];
        var myCurrent = Public;
        while(myCurrent) {
            if(myContexts.length > 0) {
                var myContext = myCurrent.i18nContext;
                if(myContexts[myContexts.length - 1] != myContext) {
                    myContexts.push(myCurrent.i18nContext);
                }
            } else {
                myContexts.push(myCurrent.i18nContext);
            }
            myCurrent = myCurrent.parent;
        }
        return myContexts;
    };

    Public.getI18nItemByName = function(theName) {
        var myContexts = Public.i18nContexts;
        for(var i = 0; i < myContexts.length; i++) {
            var myContext = myContexts[i];
            var myItem = myContext.getChildByName(theName);
            if(myItem) {
                return myItem;
            }
        }
        return null;
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

    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        var myContextName = Protected.getString("i18nContext", "");
        if(myContextName != "") {
            _myI18nContext = Public.getChildByName(myContextName);
        }

        Base.postRealize();
    };
    
}
