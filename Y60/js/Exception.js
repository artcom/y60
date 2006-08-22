/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: Exception.js,v $
//
//   $Revision: 1.3 $
//
// Description: ExceptionBase class hierarchy
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("Y60JSSL.js");

// Use fileline() function to create location object
// theName is optional.
function Exception(theMessage, theLocation, theName) {
    var self = this; // inner function this bug workaround
    self.message    = theMessage;
    self.fileName   = theLocation ? theLocation.file : "";
    self.lineNumber = theLocation ? theLocation.line : "";
    self.name       = theName || "Exception";

    this.what = function() {
        return self.message;
    }
    this.where = function() {
        return "[" + self.fileName + ":" + self.lineNumber + "]";
    }
    this.toString = function() {
        return self.name + " in " + self.where() + ": " + self.message;
    }

    this.set = function(theMessage, theLocation, theName) {
        self.message = theMessage;
        self.fileName   = theLocation.file;
        self.lineNumber = theLocation.line;
        self.name    = theName;
    }
}

// This functions works for c++ and for JavaScript exceptions
function composeException(theException) {
    var myMessage = "";
    if (typeof(theException) == "object" && "message" in theException) {
        if (theException.name != "Error") {
            myMessage += theException.name + ": ";
        }
        myMessage += trim(theException.message);

        if ("fileName" in theException) {
            myMessage += " [" + getFilenamePart(theException.fileName);
        }

        if ("lineNumber" in theException) {
            myMessage += ":" + theException.lineNumber + "]";
        }
    } else {
        myMessage = "ERROR: " + String(theException);
    }
    return myMessage;
}

function reportException(theException) {
    Logger.error(composeException(theException));
}
