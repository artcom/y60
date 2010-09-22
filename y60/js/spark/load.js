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
// Description: Document loader and instantiation engine
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

/*jslint nomen:false*/
/*globals spark, use, Node, Logger, searchFile*/

/**
 * Load an xml string into the spark world.
 */
spark.loadString = function (theString, theParent) {
    var myNode = new Node(theString);
    return spark.loadDocument(myNode, theParent);
};

/**
 * Load an xml file into the spark world.
 */
spark.loadFile = function (theFile, theParent, theArguments) {
    Logger.info("Loading spark file " + theFile);
    var myFileWithPath = searchFile(theFile);
    if (!myFileWithPath) {
        throw new Error("spark file '" + theFile + "' does not exist.");
    }
    var myNode = new Node();
    myNode.parseFile(myFileWithPath);
    return spark.loadDocument(myNode, theParent, theFile, theArguments);
};

/**
 * Load a dom tree into the spark world.
 *
 * Note that this will keep references to the DOM tree
 * in the constructed components. If you wan't referential
 * independence, clone your dom first.
 */
spark.loadDocument = function (theNode, theParent, theFile, theArguments) {
    var myRoot = spark.findRootElement(theNode);
    var myComponent = spark.instantiateRecursively(myRoot, theParent, theArguments);
    if (theFile) {
        myComponent._sparkFile_ = theFile;
    }
    myComponent.realize();
    myComponent.postRealize();
    return myComponent;
};


/**
 * Internal: instantiation engine.
 */
spark.instantiateRecursively = function (theNode, theParent, theArguments) {
    if (!theNode) {
        Logger.fatal("Internal fault: instantiateRecursively called without a node.");
    }

    // XXX: this is how templates are registered and coerced into
    //      template classes. this mechanism might be generalizable
    //      and generally useful. Think "node handlers vs component instantiation",
    //      where the former could, for example, be components that can be given
    //      child nodes that are purely descriptive.
    if (theNode.nodeName === "Template") {
        var myFile = theNode.src;
        //var myName = theNode.name;
        Logger.info("Loading template " + theNode.name + " from file " + myFile);
        spark[theNode.name] = spark.LoadedClass(theNode.name, myFile);
        return null;
    }

    // ignore comments
    if (theNode.nodeName === "#comment") {
        return null;
    }

    var myName = theNode.nodeName;

    // ignore and warn about unknown component classes
    if (!(myName in spark.componentClasses)) {
        Logger.warning("Unknown component class " +  myName + ". Ignoring definition.");
        return null;
    }

    // instantiate the component
    var myInstance = spark.instantiateComponent(theNode, theArguments);

    // add to parent if we have one
    if (theParent) {
        theParent.addChild(myInstance);
    }

    // trigger child instantiation / recurse
    myInstance.instantiateChildren(theNode);

    // break recursion
    return myInstance;
};

/**
 * Internal: instantiate a single component
 */
spark.instantiateComponent = function (theNode, theArguments) {
    Logger.info("Instantiating " + theNode.nodeName +
                ("name" in theNode ? " named " + theNode.name :
                ("id" in theNode ? " with id " + theNode.id : "")));

    var myClassName = theNode.nodeName;
    var myNode = theNode;

    var MyConstructor = spark.componentClasses[myClassName];
    return new MyConstructor(myNode, theArguments);
};
