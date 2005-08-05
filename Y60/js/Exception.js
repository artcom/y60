/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
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


function Exception(what,where,name) {
    var self = this; // inner function this bug workaround
    var _what = what;
    var _where = where;
    var _name = name || "Exception";

    this.what = function() {
        return self._what;
    };
    this.where = function() {
        return self._where;
    };
    this.name = function() {
        return self._name;
    };   
    this.toString = function() {
        return _name + " in " + _where + ": " + _what;
    }    
         
    this.set = function(what, where, name) {
        self._what = what;
        self._where = where;
        self._name = name;
    };
};
