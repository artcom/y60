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
// Description: Top-level load file for SPARK
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

// object system
use("common.js");

/**
 * The SPARK namespace.
 * 
 * Everything defined by the spark library goes into this namespace.
 */
var spark = Namespace("spark");

spark.findRootElement = function(theDocument) {
    var myRoot = theDocument.firstChild;
    // firstChild might be a xml processing instruction, a
    // comment, a doctype declaration or god knows what else.
    while (myRoot.nodeType != Node.ELEMENT_NODE) {
        if ( ! myRoot.nextSibling) {
            throw new Error("spark document contains no xml element");
        }
        myRoot = myRoot.nextSibling;
    }
    return myRoot;
}

// component instantiator
use("meta.js");
// spark file (and dom) loader
use("load.js");
// hacks around y60 text rendering
use("text.js");
// camera utility functions
use("camera.js");
// component registry
use("registry.js");
// image caching
use("cache.js");
// basic component classes
use("components.js");
// event handling
use("events.js");
// internationalization
use("internationalization.js");

// XXX: spark widgets
if ("useSparkWidgets" in this) {
    use("widgets.js");
}
