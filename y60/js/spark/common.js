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
// Description: Common object system (will be replaced by flint)
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
/*global Logger, Exception*/

/**
 * Global metamethod: Define a namespace on the target.
 * 
 * This metamethod allows the definition of a namespace.
 * 
 * It should only be used to define global namespaces as follows:
 * 
 *   var spark = Namespace("spark");
 * 
 */
function Namespace(theName) {
    return {AbstractClass: AbstractClass, Class: Class, name: theName};
}

/**
 * Namespace metamethod: Define an abstract class in the target namespace.
 * 
 * This metamethod is used to define an abstract (uninstantiable) class.
 * 
 * It should be used as follows:
 * 
 *   spark.BaseClass = spark.AbstractClass("BaseClass");
 * 
 * A constructor for the class will be defined. The constructor is not
 * intended to be used but exists for debugging. It will always
 * log an error.
 * 
 */
function AbstractClass(theName) {
    Logger.info("Defining abstract class " + theName);
    var myNamespace = this;

    var myConstructor = function () {
        Logger.error("Trying to instantiate abstract class " + myNamespace.name + "." + theName);
        // TODO check if this makes sense:: throw "Trying to instantiate abstract class " + myNamespace.name + "." + theName;
    };

    myConstructor._className_ = theName;

    return myConstructor;
}

/**
 * Namespace metamethod: Define a concrete class in the target namespace.
 * 
 * This metamethod is used to define a concrete (instantiable) class.
 * 
 * It should be used as follows:
 * 
 *    spark.MyClass = spark.Class("MyClass");
 * 
 * This will define a constructor that will call the real, user-provided
 * constructor of the class. This constructor must be defined as follows:
 * 
 *    spark.MyClass.Constructor = function(Protected, %{ ctor signature }% ) {
 *        var Public = this;
 *        var Base = {};
 *        %{ class body }% 
 *    };
 * 
 * The constructor will be called with the protected subobject of the
 * new instance as its first argument. Any arguments passed to the real,
 * outer, constructor will be passed as additional arguments.
 * 
 * Note that component classes should not have any additional constructor
 * arguments in CTOR SIGNATURE, as the component framework imposes its
 * own constructor signature involving the component-describing XML node,
 * effectively creating a default constructibility invariant.
 * 
 * The following metamethods will be attached to any new instance:
 *   Inherit, Getter, Setter, Signal, Property, Initialize
 *
 * See the respective JSDoc for the specification of these metamethods.
 * 
 */
function Class(theName) {
    Logger.info("Defining class " + theName);
    var myNamespace = this;

    function myConstructor() {
        var myPublic = this;
        var myProtected = {};

        // initialize internal slots
        myPublic._protected_ = myProtected;
        myPublic._className_ = theName;
        myPublic._class_     = myNamespace[theName];

        myPublic._sparkFile_ = "";

        myPublic._classes_   = {};
        myPublic._classes_[theName] = myNamespace[theName];

        myPublic._properties_ = [];
        myPublic._signals_    = [];

        // provide metamethods
        myPublic.Inherit    = Inherit;
        myPublic.Getter     = Getter;
        myPublic.Setter     = Setter;
        myPublic.Signal     = Signal;
        myPublic.Property   = Property;
        myPublic.Initialize = Initialize;
        myPublic.GetterOverride = GetterOverride;
        myPublic.SetterOverride = SetterOverride;

        myProtected.Getter     = Getter;
        myProtected.Setter     = Setter;
        myProtected.Property   = Property;
        myProtected.GetterOverride = GetterOverride;
        myProtected.SetterOverride = SetterOverride;

        // call the real constructor
        var myArguments = [myProtected].concat(Array.prototype.slice.call(arguments));
        if (theName in myNamespace) {
            myNamespace[theName].Constructor.apply(myPublic, myArguments);
        } else {
            Logger.error("'" + theName +
                         "' not found in namespace '" + myNamespace.name + "'");
        }
    }

    myConstructor._className_ = theName;

    return myConstructor;
}

/**
 * Metamethod: Makes the target object be an instance of the given class.
 * 
 * This method calls the constructor of the given class on the target object,
 * giving it an environment appropriate for extending the target object
 * as to make it an instance the the CLASS.
 * 
 * Used like this (in the context of a class ctor):
 * 
 *   this.Inherit(spark.BaseClass);
 * 
 */
function Inherit(theClass) {
    var myArguments = [this._protected_];
    myArguments = myArguments.concat(Array.prototype.slice.call(arguments, 1));
	// XXX this is compatibility goo for XIP
	if ("_className_" in theClass) {
        this._classes_[theClass._className_] = theClass;
	} else {
	    Logger.warning("Warning. Inheriting oldschool class without _className_. Precedence lists will be incomplete.");
	}
    theClass.Constructor.apply(this, myArguments);
}

/**
 * Internal metamethod: Inherit an oldschool (Y60) class into the target object.
 * 
 * This, somewhat magic and special, metamethod is used to inherit
 * a class Y60 javascript class into a SPARK class.
 * 
 * It should, in general, not be used. However, Window needs to inherit
 * SceneViewer. So this is here as a generalized hack for achieving such things.
 * 
 */
function InheritOldschool(theClass) {
    var myArguments = Array.prototype.slice.call(arguments);
    theClass.prototype.Constructor.apply(this, myArguments);
}

/**
 * Metamethod: Define a getter.
 * 
 * This is a simple wrapper around __defineGetter__, present only for stylistic purposes.
 * 
 * It replaces the classic syntax:
 * 
 *    this.NAME getter = FUNCTION
 * 
 * With the almost as pretty:
 * 
 *    this.Getter(NAME, FUNCTION);
 * 
 */
function Getter(theName, theFunction) {
    var myNextGetter = this.__lookupGetter__(theName);

    if (myNextGetter) {
        Logger.warning("Defining setter for property " + theName + " in class " +
                       this._className_ + ", which already has a setter for that property.");
    }

    this.__defineGetter__(theName, theFunction);
}

/**
 * Metamethod: Override a getter.
 * 
 * This method uses __lookupGetter__ and __defineGetter__ to safely override
 * a getter function, erring if the override is not an override at all.
 * 
 * It also passes the getter function an additional argument, NEXTGETTER,
 * which refers to the getter being overridden.
 * 
 */
function GetterOverride(theName, theFunction) {
    var myNextGetter = this.__lookupGetter__(theName);

    if (!myNextGetter) {
        Logger.error("Trying to override getter for property " + theName + " in class " +
                     this._className_ + ", which does not have a getter for that property.");
    }

    var myWrapper = function (theValue) {
        return theFunction.call(this, myNextGetter);
    };

    this.__defineGetter__(theName, myWrapper); 
}

/**
 * Metamethod: Define a setter.
 * 
 * This is a simple wrapper around __defineSetter__, present only for stylistic purposes.
 * 
 * It replaces the classic syntax:
 * 
 *    this.NAME setter = FUNCTION
 * 
 * With the almost as pretty:
 * 
 *    this.Setter(NAME, FUNCTION);
 * 
 */
function Setter(theName, theFunction) {
    var myNextSetter = this.__lookupSetter__(theName);

    if (myNextSetter) {
        Logger.warning("Defining setter for property " + theName + " in class " +
                       this._className_ + ", which already has a setter for that property.");
    }

    this.__defineSetter__(theName, theFunction);
}

/**
 * Metamethod: Override a getter.
 * 
 * This method uses __lookupGetter__ and __defineGetter__ to safely override
 * a getter function, erring if the override is not an override at all.
 * 
 * It also passes the setter function an additional argument, NEXTSETTER,
 * which refers to the setter being overridden.
 * 
 */
function SetterOverride(theName, theFunction) {
    var myNextSetter = this.__lookupSetter__(theName);

    if (!myNextSetter) {
        Logger.error("Trying to override setter for property " + theName + " in class " +
                     this._className_ + ", which does not have a setter for that property.");
    }

    var myWrapper = function(theValue) {
        theFunction.call(this, theValue, myNextSetter);
    };

    this.__defineSetter__(theName, myWrapper);
}

/**
 * Metamethod: Define a signal on the target object.
 * 
 * Calling this metamethod will create a signal slot on the target object.
 * 
 * This signal slot can be given callbacks that will be called when the
 * signal is signalled. The interface of the signal is in the form of
 * a POJSO, reachable via a property named after the signal.
 * 
 * The interface of this POJSO is as follows:
 *  - signal()
 *     Signal the slot, calling all handlers on their respective context.
 *  - call(theHandler, theContext)
 *     Add a HANDLER to the signal slot, indicating the CONTEXT
 *     that will be used as the 'this' of said handler on call.
 * 
 */
function Signal(theName) {
    var mySignal = {};

    var myHandlers = [];
    var myContexts = [];

    mySignal.signal = function () {
        var myArguments = Array.prototype.slice.call(arguments);
        for (var i = 0; i < myHandlers.length; i++) {
            myHandlers[i].apply(myContexts[i], myArguments);
        }
    };

    mySignal.call = function (theHandler, theContext) {
        // TODO catch undefined since it will then be the global object.
        // see https://developer.mozilla.org/En/Core_JavaScript_1.5_Reference/Objects/Function/Apply
        myHandlers.push(theHandler);
        myContexts.push(theContext);
    };

    this[theName] = mySignal;
}

/**
 * Internal: Generic serialization-grade conversion from value to string.
 */
function ConvertFromString(theType, theString) {
    switch (theType) {
    case Boolean:
        return !(theString === "false");
    case String:
        return theString;
    case Number:
        return Number(theString);
    default:
        throw new Exception("Do not know how to convert to the given type " + theType);
    }
}

/**
 * Internal: Generic serialization-grade conversion from string to value.
 */
function ConvertToString(theType, theValue) {
    switch (theType) {
    case Boolean:
    case Number:
    case String:
        return String(theValue);
    default:
        throw new Exception("Do not know how to convert from the given type " + theType);
    }
}

/**
 * Metamethod: Define a dynamic property on an object
 * 
 * Calling this metamethod on an object has the following effects:
 * 
 *   - defining a getter named NAME on the object
 *   - defining a setter named NAME on the object
 *   - ensuring initialization of the property to its DEFAULT value
 *   - the setter will call the HANDLER when the property is changed
 *   - adding the property to the internal list of properties
 *     on the object, allowing additional metamethods such as Initialize
 * 
 * Furthermore, the property will define an internal interface,
 * used by serializers and deserializers that can be reached by
 * iterating the _properties_ array of the object. For every defined
 * property, a POJSO with the following interface will be present:
 * 
 *  - name
 *     Read-only property indicating the name if the property.
 *  - setFromString(theString)
 *     Allows setting the property from string form. HANDLER will be called.
 *  - getAsString()
 *     Allows getting the property value in string form.
 * 
 */
function Property(theName, theType, theDefault, theHandler) {
    var myProperty = {};

    var myValue = theDefault;

    this.Getter(theName, function () {
        return myValue;
    });

    this.Setter(theName, function (theValue) {
        myValue = theValue;
        if (theHandler) {
            theHandler(myValue);
        }
    });

    myProperty.name = theName;

    myProperty.setFromString = function (theString) {
        myValue = ConvertFromString(theType, theString);
        if (theHandler) {
            theHandler(myValue);
        }
    };

    myProperty.getAsString = function () {
        return ConvertToString(theType, myValue);
    };

    this._properties_.push(myProperty);
}

/**
 * Metamethod: Initialize properties of an object from XML attributes
 * 
 * This metamethods iterates over all properties defined on its
 * target object, setting the property according to the corresponding
 * XML attribute, if it is present.
 * 
 */
function Initialize(theNode) {
    var myProps = this._properties_;

    for (var i = 0; i < myProps.length; i++) {
        var myProp = myProps[i];
        var myName = myProp.name;
        if (myName in theNode) {
            var myString = theNode[myName];
            myProp.setFromString(myString);
        }
    }
}