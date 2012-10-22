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

/*jslint nomen: false, plusplus: false*/
/*globals spark, Logger*/

spark.I18nContext = spark.ComponentClass("I18nContext");

spark.I18nContext.Constructor = function (Protected) {
    var Public = this;
    var Base = {};

    Public.Inherit(spark.Container);
    Public.Inherit(spark.EventDispatcher);

    var _myLanguage = "";
    var _myDefaultLanguage = "";

    Public.__defineGetter__("language", function () {
        return _myLanguage;
    });

    Public.__defineGetter__("defaultLanguage", function () {
        return _myDefaultLanguage;
    });
    
    Public.switchLanguage = function (theLanguage) {
        Logger.info("I18n context " + Public.name + " switching to language " + theLanguage + " current language: " + _myLanguage);
        if (theLanguage !== _myLanguage) {
            _myLanguage = theLanguage;
            var myChildren = Public.children;
            var myChildCount = myChildren.length;
            for (var i = 0; i < myChildCount; i++) {
                var myChild = myChildren[i];
                myChild.switchLanguage(theLanguage);
            }

            var myContextEvent = new spark.I18nEvent(theLanguage);
            Public.dispatchEvent(myContextEvent);
        }
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
        _myDefaultLanguage = Protected.getString("defaultLanguage", "en");
        Public.switchLanguage(_myDefaultLanguage);
    };

    Base.addChild = Public.addChild;
    Public.addChild = function (theChild) {
        Base.addChild(theChild);
        if (_myLanguage !== "") {
            theChild.switchLanguage(_myLanguage);
        }
    };
};

spark.I18nEvent = spark.Class("I18nEvent");

spark.I18nEvent.LANGUAGE = "language";

spark.I18nEvent.Constructor = function (Protected, theLanguage) {
    var Public = this;
    var Base = {};

    Public.Inherit(spark.Event, spark.I18nEvent.LANGUAGE);

    var _myLanguage = theLanguage;

    Public.__defineGetter__("language", function () {
        return _myLanguage;
    });
};

spark.I18nItem = spark.AbstractClass("I18nItem");

spark.I18nItem.Constructor = function (Protected) {
    var Public = this;
    var Base = {};

    Public.Inherit(spark.Component);
    Public.Inherit(spark.EventDispatcher);

    var _myLanguage = "";

    Public.__defineGetter__("language", function () {
        return _myLanguage;
    });

    var _myLanguageData = {};
    var _myLanguageNodes = {}; 

    Protected.languageData = function () {
        return _myLanguageData;
    };

    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        var myNode = Public.node;
        for (var i = 0; i < myNode.childNodesLength(); i++) {
            var myChild = myNode.childNode(i);
            var myLang = myChild.nodeName;
            var myData = "";
            var myNumChildren = myChild.childNodesLength();
            for (var j = 0; j < myNumChildren; j++) {
                myData += myChild.childNode(j).nodeValue;
            }
            Public.addLanguageData(myLang, myData);
            Public.addLanguageNode(myLang, myChild);
        }
    };

    Protected.createEvent = function (theLanguage) {
        return new spark.I18nEvent(theLanguage);
    };

    Public.getLanguageData = function (theLanguage) {
        if (!theLanguage) {
            theLanguage = _myLanguage;
        }
        if (!(theLanguage in _myLanguageData)) {
            Logger.debug("I18n item '" + Public.name + "' does not contain language '" + theLanguage + "'");
            if (Public.parent.defaultLanguage in _myLanguageData) {
                return _myLanguageData[Public.parent.defaultLanguage];
            } else {
                Logger.warning("I18n item '" + Public.name + "' does not contain defaultlanguage '" + Public.parent.defaultLanguage + "'");
                return null;
            }
        } else {
            return _myLanguageData[theLanguage];
        }
    };

    Public.hasLanguageData = function (theLanguage) {
        return theLanguage in _myLanguageData;
    };
    
    Public.getLanguageNode = function(theLanguage) {
        if(!theLanguage) {
            theLanguage = _myLanguage;
        }
        if(!(theLanguage in _myLanguageNodes)) {
            Logger.info("I18n item '" + Public.name + "' does not contain language '" + theLanguage + "'");
            if (Public.parent.defaultLanguage in _myLanguageNodes) {
                return _myLanguageNodes[Public.parent.defaultLanguage];
            } else {
                Logger.warning("I18n item '" + Public.name + "' does not contain defaultlanguage '" + Public.parent.defaultLanguage + "'");
                return null;
            }
        } else {
            return _myLanguageNodes[theLanguage];
        }
    };
    
    Public.hasLanguageNode = function(theLanguage) {
        return theLanguage in _myLanguageNodes;
    };

    Public.switchLanguage = function (theLanguage) {
        if (theLanguage == _myLanguage) {
            return;
        }

        // XXX; one of these tests is redundant now
        if (!(theLanguage in _myLanguageData)) {
            Logger.debug("I18n item " + Public.name + " does not contain language " + theLanguage);
        }
        if(!(theLanguage in _myLanguageNodes)) {
            Logger.debug("I18n item " + Public.name + " does not contain language node " + theLanguage);
        }
        _myLanguage = theLanguage;
        var myEvent = Protected.createEvent(theLanguage);
        Public.dispatchEvent(myEvent);
    };

    Public.addLanguageData = function (theLanguage, theData) {
        if (theLanguage in _myLanguageData) {
            Logger.warning("duplicate i18n data for item " + Public.name + " in language " + theLanguage);
        }
        _myLanguageData[theLanguage] = theData;
    };
    	   
    Public.addLanguageNode = function(theLanguage, theNode) {
        if(theLanguage in _myLanguageNodes) {
            Logger.warning("duplicate i18n node for item " + Public.name + " in language " + theLanguage);
        }
        _myLanguageNodes[theLanguage] = theNode;
    };

    Public.changeLanguageData = function (theLanguage, theData) {
        if (theLanguage in _myLanguageData) {
            Logger.info("overwrite i18n data for item " + Public.name + " in language " + theLanguage);
        }
        _myLanguageData[theLanguage] = theData;
        var myEvent = Protected.createEvent(theLanguage);
        Public.dispatchEvent(myEvent);
    };

};

spark.I18nText = spark.ComponentClass("I18nText");

spark.I18nText.Constructor = function (Protected) {
    var Public = this;
    var Base = {};

    Public.Inherit(spark.I18nItem);

    Base.createEvent = Protected.createEvent;
    Protected.createEvent = function (theLanguage) {
        var myEvent = Base.createEvent(theLanguage);
        myEvent.text = Public.text;
        myEvent.fontStyle = Public.fontStyle;
        return myEvent;
    };

    Public.__defineGetter__("text", function () {
        var myData = Public.getLanguageData(Public.language);
        if (myData === null) {
            return "";
        } else {
            return myData;
        }
        // see if this can be replaced by
        // return Public.getLanguageData(Public.language) || "";
    });
		
    Public.__defineGetter__("fontStyle", function() {
        var myFontStyleNode = Public.getLanguageNode(Public.language);
        return myFontStyleNode;
    });
};

spark.I18nImage = spark.ComponentClass("I18nImage");

spark.I18nImage.Constructor = function (Protected) {
    var Public = this;
    var Base = {};

    Public.Inherit(spark.I18nItem);

    Base.createEvent = Protected.createEvent;
    Protected.createEvent = function (theLanguage) {
        var myEvent = Base.createEvent(theLanguage);
        myEvent.src = Public.src;
        return myEvent;
    };

    Public.__defineGetter__("src", function () {
        var myData = Public.getLanguageData(Public.language);
        if (myData === null) {
            return "";
        } else {
            return myData;
        }
        // see if this can be replaced by
        // return Public.getLanguageData(Public.language) || "";
    });
};

spark.I18nMovie = spark.ComponentClass("I18nMovie");

spark.I18nMovie.Constructor = function (Protected) {
    var Public = this;
    var Base = {};

    Public.Inherit(spark.I18nItem);
    
    Base.createEvent = Protected.createEvent;
    Protected.createEvent = function (theLanguage) {
        var myEvent = Base.createEvent(theLanguage);
        myEvent.src = Public.src;
        return myEvent;
    };

    Public.__defineGetter__("src", function () {
        var myData = Public.getLanguageData(Public.language);
        if (myData === null) {
            return "";
        } else {
            return myData;
        }
        // see if this can be replaced by
        // return Public.getLanguageData(Public.language) || "";
    });
};

spark.I18nAudio = spark.ComponentClass("I18nAudio");

spark.I18nAudio.Constructor = function (Protected) {
    var Public = this;
    var Base = {};

    Public.Inherit(spark.I18nItem);
    
    Base.createEvent = Protected.createEvent;
    Protected.createEvent = function (theLanguage) {
        var myEvent = Base.createEvent(theLanguage);
        myEvent.src = Public.src;
        return myEvent;
    };

    Public.__defineGetter__("src", function () {
        var myData = Public.getLanguageData(Public.language);
        if (myData === null) {
            return "";
        } else {
            return myData;
        }
        // see if this can be replaced by
        // return Public.getLanguageData(Public.language) || "";
    });
};
