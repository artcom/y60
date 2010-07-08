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

/*jslint nomen: false, plusplus: false*/
/*global spark, Logger, Vector2f, Vector3f, Vector4f, trim*/

spark.ourComponentsByNameMap = {};

/**
 * Component base class.
 *
 * Components are XML-initializable objects organized
 * in a hierarchy. They can be graphical or not.
 */

spark.Component = spark.AbstractClass("Component");

spark.Component.Constructor = function (Protected) {
    var Public = this;
    
    /**
     * Components can have names.
     */
    this.Property("name", String, null);

    var _myNode = null;
    var _myParent = null;

    /**
     * Printing method.
     */
    Public.toString = function () {
        return "[" + Public._className_ + " " + Public.vocation + "]";
    };

    /**
     * Get the parent of this component.
     */
    Public.parent getter = function () {
        return _myParent;
    };

    /**
     * Internal: change the parent of this component.
     */
    Public.parent setter = function (theParent) {
        _myParent = theParent;
    };

    /**
     * Find the root of the component hierarchy.
     * 
     * NOTE: This is not identical to finding the
     *       stage of a widget.
     */
    Public.root getter = function () {
        var myCurrent = Public;
        while (myCurrent.parent !== null) {
            myCurrent = myCurrent.parent;
        }
        return myCurrent;
    };

    /**
     * Get the XML node used to instantiate this component.
     */
    Public.node getter = function () {
        return _myNode;
    };

    /**
     * Initialize this component from an XML node.
     * 
     * This mostly kicks property initialization.
     */
    Public.initialize = function (theNode) {
        Public.Initialize(theNode);
        _myNode   = theNode;
    };

    /**
     * Get the so-called vocation of this component.
     * 
     * The vocation of a component is a human-readable
     * string describing the components identity.
     */
    Public.vocation getter = function () {
        if (Public.name) {
            return "named " + Public.name;
        }
        if ("id" in Public && Public.id) {
            return "with id " + Public.id;
        }
        return "without a name";
    };

    /**
     * Given the node describing this component,
     * instantiate all children and initialize them.
     * 
     * This method is used by the component instantiator
     * to create the children of a component.
     * 
     * It can, however, also be used to coerce the
     * "child" description of a component into something
     * entirely different. One could, for example,
     * override this and use it to put data items
     * in the component description. I18n works like this.
     */
    Public.instantiateChildren = function (theNode) {
    };


    // XXX: realize and post-realize should be one thing,
    //      with the remaining coupled properties uncoupled.
    Public.realize = function () {
        spark.ourComponentsByNameMap[Public.name] = Public;
        Logger.debug("Realizing " + Public._className_ + " " + Public.vocation);
    };

    Public.postRealize = function () {
        Logger.debug("Post-Realizing " + Public._className_ + " " + Public.vocation);
    };

    // XXX: I18N should be implemented with a property type
    Protected.realizeI18N = function (theItem, theAttribute) {
        var myConcreteItem = theItem;
        var myId = "";
        if (theAttribute === undefined) {
            myId = Protected.getString("multilanguageId", "");
        } else {
            myId = Protected.getString(theAttribute, "");
        }
        if (myId !== "") {
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
    };

    Protected.getString = function (theName, theDefault) {
        if (!_myNode) {
            if (arguments.length < 2) {
                Logger.error(Public._className_ + " cannot be instantiated from code because it needs attribute " + theName);
            } else {
                return theDefault;
            }
        }
        if (theName in _myNode) {
            return _myNode[theName];
        } else {
            if (arguments.length < 2) {
                Logger.error(Public._className_ + " requires attribute " + theName);
            } else {
                return theDefault;
            }
        }
    };

    Protected.getBoolean = function (theName, theDefault) {
        if (!_myNode) {
            if (arguments.length < 2) {
                Logger.error(Public._className_ + " cannot be instantiated from code because it needs attribute " + theName);
            } else {
                return theDefault;
            }
        }
        if (theName in _myNode) {
            return (_myNode[theName] === "true");
        } else {
            if (arguments.length < 2) {
                Logger.error(_myNode.nodeName + " requires attribute " + theName);
            } else {
                return theDefault;
            }
        }
    };
    
    Protected.getVector4f = function (theName, theDefault) {
        var myArray = Protected.getArray(theName, theDefault);
        if (myArray && myArray.length === 4) {
            return new Vector4f(myArray[0], myArray[1], myArray[2], myArray[3]);
        } else {
            return theDefault;
        }
    };

    Protected.getVector3f = function (theName, theDefault) {
        var myArray = Protected.getArray(theName, theDefault);
        if (myArray && myArray.length === 3) {
            return new Vector3f(myArray[0], myArray[1], myArray[2]);
        } else {
            return theDefault;
        }
    };

    Protected.getVector2f = function (theName, theDefault) {
        var myArray = Protected.getArray(theName, theDefault);
        if (myArray && myArray.length === 2) {
            return new Vector2f(myArray[0], myArray[1]);
        } else {
            return theDefault;
        }
    };

    Protected.getNumber = function (theName, theDefault) {
        if (!_myNode) {
            if (arguments.length < 2) {
                Logger.error(Public._className_ + " cannot be instantiated from code because it needs attribute " + theName);
            } else {
                return theDefault;
            }
        }
        if (theName in _myNode) {
            return Number(_myNode[theName]);
        } else {
            if (arguments.length < 2) {
                Logger.error(_myNode.nodeName + " requires attribute " + theName);
            } else {
                return theDefault;
            }
        }
    };

    function findChildArray(theArrayString) {
        var myStrings, i;
        var myArray = [];
        if (theArrayString[0] === "[") {
            theArrayString = trim(theArrayString.substring(1, theArrayString.length - 1));
            theArrayString = theArrayString.replace(/, /g, ",");
            myStrings = theArrayString.split("],[");
            for (i = 0; i  < myStrings.length; ++i) {
                if (myStrings[i].length > 0) {
                    myArray.push(findChildArray(myStrings[i]));
                }
            }
            return myArray;
        } else {
            myStrings = theArrayString.split(",");
            for (i = 0; i < myStrings.length; ++i) {
                myStrings[i] = trim(myStrings[i]);
            }
            return myStrings;
        }
    }

    // XXX: this requires a member type. else it don't make no sense.
    Protected.getArray = function (theName, theDefault) {
        if (!_myNode) {
            if (arguments.length < 2) {
                Logger.error(Public._className_ + " cannot be instantiated from code because it needs attribute " + theName);
            } else {
                return theDefault;
            }
        }
        if (theName in _myNode) {
            var myString = _myNode[theName].substring(1, _myNode[theName].length - 1);
            if (myString.length === 0) {
                return [];
            }
            var myArray = findChildArray(trim(myString));
            return myArray;
        } else {
            if (arguments.length < 2) {
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

spark.Container.Constructor = function (Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.Component);

    var _myChildren = [];

    /**
     * Get an array containing all children of this container.
     */
    Public.children getter = function () {
        return _myChildren;
    };

    /**
     * Add the given child to this container.
     */
    Public.addChild = function (theChild) {
        _myChildren.push(theChild);
        theChild.parent = Public;
    };

    /**
     * Remove the given child.
     */
    Public.removeChild = function (theChild) {
        var myChildIndex = js.array.indexOf(_myChildren, theChild);

        if (myChildIndex === -1) {
            throw new Error("Could not remove " + theChild.name + " from " + Public.name + " because its not a child");
        }
        _myChildren.splice(myChildIndex, 1);
        theChild.parent = null;
    };

    /**
     * Remove all children.
     */
    Public.removeAllChildren = function () {
        var i = _myChildren.length;
        while (i--) {
            _myChildren[i].name = null;
        }
        _myChildren = [];
    };

    /**
     * Retrieve a (direct) child by name.
     */
    Public.getChildByName = function (theName) {
        var i = _myChildren.length, myChild;
        while (i--) {
            myChild = _myChildren[i];
            if (myChild.name === theName) {
                return myChild;
            }
        }
        return null;
    };

    /**
     * Recursively retrieve children of the given name.
     */
    // TODO: check if this is used at all - seems it cannot work - see myChild!
    Public.findChildrenByName = function (theName) {
        var myChildren = [];
        var myOwnChild = Public.getChildByName(theName);
        if (myOwnChild) {
            myChildren.push(myOwnChild);
        }
        for (var i = 0; i < _myChildren.length; i++) {
            var myGrandchildren = myChild.findChildrenByName(); // what is myChild?
            if (myGrandchildren.length > 0) {
                myChildren = myChildren.concat(myGrandchildren);
            }
        }
        return myChildren;
    };

    /**
     * Given the XML node describing this component,
     * recursively instantiate and initialize all
     * specified child components.
     * 
     * See implementation in Component for rationale.
     */
    Public.instantiateChildren = function (theNode) {
        for (var i = 0; i < theNode.childNodesLength(); i++) {
            var myChildNode = theNode.childNode(i);
            spark.instantiateRecursively(myChildNode, Public);
        }
    };

    // XXX: rework realization
    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        for (var i = 0; i < _myChildren.length; i++) {
            _myChildren[i].realize();
        }
    };

    // XXX: rework realization
    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
        for (var i = 0; i < _myChildren.length; i++) {
            _myChildren[i].postRealize();
        }
    };
};
