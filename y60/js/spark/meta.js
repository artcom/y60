/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2010, ART+COM AG Berlin, Germany <www.artcom.de>
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
// Description: Component class registry and template processor
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
/*global spark, Class, Node, Logger, searchFile*/

/**
 * Map of all component classes, indexed by name.
 */
spark.componentClasses = {};

/**
 * Define a component class.
 *
 * Conceptually, this function is a metaclass
 * providing component-class registration.
 *
 * Should be called on the spark namespace like:
 * 
 *   spark.Fnord = spark.ComponentClass("Fnord");
 *
 * The class will internally be defined by using the Class
 * metamethod, producting an instantiable class.
 * 
 * The client must provide an inner constructor with the
 * same syntax as for normal concrete classes:
 * 
 *    spark.Fnord.Constructor = function(Protected) {
 *        var Public = this;
 *        var Base = {};
 *        %{ class body }% 
 *    };
 * 
 * The real outer constructor for this class will be wrapped
 * with a constructor of the following signature:
 * 
 *  - new MyClass(theNode)
 *     Creates and initializes an instance of MyClass,
 *     optionally deserializing NODE to provide property values.
 * 
 * The component class will be registered in the
 * componentClasses registry under the given NAME,
 * allowing it to be constructed with the factory
 * machinery of the load module.
 * 
 * Note that component classes have a reduced internal
 * interface comprised only of _className_.
 * 
 */
spark.ComponentClass = function (theName) {
    // generate the real constructor
    var myConstructor = Class.call(this, theName);

    // wrap the constructor with an initialization call
    var myInitializingConstructor = function (theNode) {
        myConstructor.call(this);
        if (theNode) {
            this.initialize(theNode);
        }
    };

    // register this class as a component class
    spark.componentClasses[theName] = myInitializingConstructor;

    // XXX: give the class a name, as the normal class definer does
    myInitializingConstructor._className_ = theName;

    // return the class so it can be put in the namespace
    return myInitializingConstructor;
};


/**
 * Internal: Define a templated component class.
 *
 * Conceptually, this function is a metaclass
 * providing component template instantiation.
 *
 * Will be called on the spark namespace equivalently to:
 * 
 *   spark.Fnord = spark.LoadedClass("Fnord", "Fnord.spark");
 *
 * The class will be registered in the componentClasses
 * registry under the given NAME.
 * 
 * Instantiating this class will clone the template from
 * the given file and trigger the construction process
 * for the component hierarchy described therein.
 * 
 * The real outer constructor for this class will be provided
 * and has the following interface:
 * 
 *  - new MyClass(theNode)
 *     Creates and initializes an instance of MyClass,
 *     treating NODE as a call to the template from FILE.
 * 
 * This outer constructor will apply all attributes
 * found in NODE to the root node of a clone of the
 * loaded template, effectively allowing arbitrary
 * properties on the root component of the template
 * to be overridden by the template call.
 * 
 */
 
spark.LoadedClass = function (theClassName, theFile) {
    // load template from file
    var myDocument = new Node();
    var myFileWithPath = searchFile(theFile);
    myDocument.parseFile(myFileWithPath);
    // get the template base class constructor
    var myTemplateNode = spark.findRootElement(myDocument);
    var myBaseClass = myTemplateNode.nodeName;

    if (!(myBaseClass in spark.componentClasses)) {
        Logger.error("Component class " + myBaseClass + " is unknown at this point");
    }

    var myBaseConstructor = spark.componentClasses[myBaseClass];

    // build a constructor wrapper
    var myInitializingConstructor = function (theCallNode) {
        // clone template so we can change it
        var myWovenNode = myTemplateNode.cloneNode(true);

        // merge attributes from the template call
        var myAttributes = theCallNode.attributes;
        for (var i = 0; i < myAttributes.length; i++) {
            var myAttribute = myAttributes[i];
            myWovenNode[myAttribute.nodeName] = myAttribute.nodeValue;
        }

        // call the base class constructor
        myBaseConstructor.call(this, myWovenNode);

        // override the objects class
        this._className_ = theClassName;
        this._class_     = myInitializingConstructor;
        this._sparkFile_ = theFile;

        // instantiate children
        this.instantiateChildren(myWovenNode);
    };

    // register our new class
    spark.componentClasses[theClassName] = myInitializingConstructor;

    // return the thing so it can be put in the namespace
    return myInitializingConstructor;
};