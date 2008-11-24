//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

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

function InheritForeign(theClass) {
    var myArguments = Array.prototype.slice.call(arguments);
    theClass.prototype.Constructor.apply(this, myArguments);
};

