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
// Description: Event handling
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

/*jslint nomen: false*/
/*globals spark, Logger, plug, Node*/

spark.enableDSASensoric = function (theSettingsFile) {
    var myDSAConfig = new Node();
    Logger.info("DSA sensoric support enabled using sensoric settings " + theSettingsFile);
    myDSAConfig.parseFile(theSettingsFile);
    plug("DSADriver", myDSAConfig);
};

spark.DSAEvent = spark.Class("DSAEvent");
spark.DSAEvent.TOUCH = "dsa-touch";

spark.DSAEvent.Constructor = function (Protected, theType, theEventName, theId, theBitMask, theGridSize, theCount) {
    var Public = this;
    Public.Inherit(spark.Event, theType);
    
    /////////////////////
    // Private Members //
    /////////////////////

    var _myEventName = theEventName;
    var _myId        = theId;
    var _myBitMask   = theBitMask;
    var _myGridSize  = theGridSize;
    var _myCount     = theCount;

    ////////////////////
    // Public Methods //
    ////////////////////

    Public.__defineGetter__("id", function () {
        return _myId;
    });

    Public.__defineGetter__("bitmask", function () {
        return _myBitMask;
    });

    Public.__defineGetter__("gridsize", function () {
        return _myGridSize;
    });

    Public.__defineGetter__("count", function () {
        return _myCount;
    });
};
