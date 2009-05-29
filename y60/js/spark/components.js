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
// Description: Component base classes
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

function unpenner() {
   var theEasingFunction = arguments[0];
   var theEasingArguments = Array.prototype.slice.call(arguments, 1);

   var myArguments = [theEasingFunction, 0.0, 1.0, 1.0];
   myArguments = myArguments.concat(theEasingArguments);

   return rcurry.apply(this, myArguments);
} 

spark.Component.Constructor = function(Protected) {
    var Public = this;

    var _myName = null;
    
    Public.name getter = function() {
        return _myName;
    };

    Public.name setter = function(theValue) {
        if(_myName != null) {
            throw new Error("the name of a component can only be set once");
        }
        _myName = theValue;
    };

    // XXX: really bad solution.
    //      this should be higher up and more informative.
    Public.toString = function (){
        return _myName;
    };
    
    var _myParent = null;

    Public.parent getter = function() {
        return _myParent;
    };

    Public.parent setter = function(theParent) {
        _myParent = theParent;
    };

    Public.root getter = function() {
        var myCurrent = Public;
        while(myCurrent.parent != null) {
            myCurrent = myCurrent.parent;
        }
        return myCurrent;
    };

    
    var _myNode = null;
    
    Public.node getter = function() {
        return _myNode;
    };


    Public.initialize = function(theNode) {
        _myNode   = theNode;
        _myName   = Protected.getString("name", "");
    };

    // XXX: realize and post-realize should be one thing,
    //      with the remaining coupled properties uncoupled.
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
            if(arguments.length < 2) {
                Logger.error(_myNode.nodeName + " requires attribute " + theName);
            } else {
                return theDefault;
            }
        }
    };
    
    // XXX: I18N should be implemented with a property type
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
            if(arguments.length < 2) {
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
            if(arguments.length < 2) {
                Logger.error(_myNode.nodeName + " requires attribute " + theName);
            } else {
                return theDefault;
            }
        }
    };
    
    // XXX: this requires a member type. else it don't make no sense.
    Protected.getArray = function(theName, theDefault) {
        if(theName in _myNode) {
            return _myNode[theName].substring(1, _myNode[theName].length -1).replace(/ /g,"").split(",");
        } else {
            if(arguments.length < 2) {
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

    var _myChildren = [];
    var _myNamedChildMap   = {};

    Public.children getter = function() {
        return _myChildren.slice(0); // XXX: clone?
    };

    Public.addChild = function(theChild) {
        _myChildren.push(theChild);

        if(theChild.name) {
            _myNamedChildMap[theChild.name] = theChild;
        }

        theChild.parent = Public;
    };

    Public.removeChild = function(theChild) {
        var myChildIndex = _myChildren.indexOf(theChild);

        if(myChildIndex == -1) {
            throw new Error("Could not remove " + theChild.name + " from " + Public.name + " because its not a child");
        }

        _myChildren.splice(myChildIndex, 1);

        if(theChild.name) {
            delete _myNamedChildMap[theChild.name];
        }

        theChild.parent = null;
    };

    Public.removeAllChildren = function() {
        var myChildren = Public.children;
        for(var i = 0; i < myChildren.length; i++) {
            Public.removeChild(myChildren[i]);
        }
    }

    Public.getChildByName = function(theName) {
        if (theName in _myNamedChildMap) {
            return _myNamedChildMap[theName];
        } else {
            return null;
        }
    };
    
    Public.findChildrenByName = function(theName) {
        var myChildren = [];
        var myOwnChild = Public.getChildByName(theName);
        if(myOwnChild) {
            myChildren.push(myOwnChild);
        }
        for(var i = 0; i < _myChildren.length; i++) {
            var myGrandchildren = myChild.findChildrenByName();
            if(myGrandchildren.length > 0) {
                myChildren = myChildren.concat(myGrandchildren);
            }
        }
        return myChildren;
    };

    // XXX: rework realization
    Base.realize = Public.realize;
    Public.realize = function() {
        Base.realize();
        for(var i = 0; i < _myChildren.length; i++) {
            _myChildren[i].realize();
        }
    };
        
    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        Base.postRealize();
        for(var i = 0; i < _myChildren.length; i++) {
            _myChildren[i].postRealize();
        }
    };

    Public.instantiateChildren = function(theNode) {
        for(var i = 0; i < theNode.childNodesLength(); i++) {
            var myChildNode = theNode.childNode(i);
            spark.instantiateRecursively(myChildNode, Public);
        }
    };
};

