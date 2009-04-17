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
// Description: TODO  
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

function Namespace(theName) {
    return {AbstractClass: AbstractClass, Class: Class, name: theName};
};

function AbstractClass(theName) {
    Logger.info("Defining abstract class " + theName);
    var myNamespace = this;
    return function () {
        Logger.error("Trying to instantiate abstract class " + myNamespace.name + "." + theName);
    };
};

function Class(theName) {
    Logger.info("Defining class " + theName);
    var myNamespace = this;

    function myConstructor () {
        var myPublic = this;
        var myProtected = {};
        
        // initialize magic slots
        myPublic._protected_ = myProtected;
        myPublic._className_ = theName;
        myPublic._class_     = myNamespace[theName];

        // provide weaving functions
        myPublic.Inherit = Inherit;
        
        // call the real constructor
        var myArguments = [myProtected].concat(Array.prototype.slice.call(arguments));
        if (theName in myNamespace){
            myNamespace[theName].Constructor.apply(myPublic, myArguments);
        } else {
            Logger.error("'" + theName + 
                         "' not found in namespace '" + myNamespace.name + "'");
        }

    };

    return myConstructor;
};

function Inherit(theClass) {
    var myArguments = [this._protected_];
    myArguments = myArguments.concat(Array.prototype.slice.call(arguments, 1));
    theClass.Constructor.apply(this, myArguments);
};

function InheritOldschool(theClass) {
    var myArguments = Array.prototype.slice.call(arguments);
    theClass.prototype.Constructor.apply(this, myArguments);
};

