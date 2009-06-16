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

/*
 * Map of all component classes.
 */
spark.componentClasses = {};


/**
 * Define a component class.
 * 
 * Conceptually, this is a metaclass providing
 * component-class registration.
 * 
 * Should be called on the spark namespace like:
 *   spark.Fnord = spark.ComponentClass("Fnord");
 * 
 */
spark.ComponentClass = function(theName) {
    // generate the real constructor
    var myConstructor = Class.call(this, theName);

    // wrap the constructor with an initialization call
    var myInitializingConstructor = function(theNode) {
        myConstructor.call(this);
        if(theNode) {
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
 * Define a templated component class.
 * 
 * Conceptually, this is a metaclass providing
 * component template instantiation.
 * 
 * Should be called on the spark namespace like:
 *   spark.Fnord = spark.LoadedClass("Fnord", "Fnord.spark");
 * 
 */
spark.LoadedClass = function(theClassName, theFile) {
    // load template from file
    var myDocument = new Node();
    myDocument.parseFile(theFile);

    // get the template base class constructor
    var myTemplateNode = spark.findRootElement(myDocument);
    var myBaseClass = myTemplateNode.nodeName;

    if(!(myBaseClass in spark.componentClasses)) {
        Logger.error("Component class " + myBaseClass + " is unknown at this point");
    }

    var myBaseConstructor = spark.componentClasses[myBaseClass];
    
    // build a constructor wrapper
    var myInitializingConstructor = function(theCallNode) {
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

        // instantiate children
        this.instantiateChildren(myWovenNode);
    };

    // register our new class
    spark.componentClasses[theClassName] = myInitializingConstructor;

    // return the thing so it can be put in the namespace
    return myInitializingConstructor;
};

