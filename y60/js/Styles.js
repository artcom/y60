/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
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
*/

use("Y60JSSL.js");

var _font_default_counter = 0;

function addDefaultStyleValues(theStyle) {

    // these are required values - if the don't exist,
    // we try to find sensible values or set defaults.

    if (theStyle instanceof Node) {
    Logger.error("no nodes as style primitives!");
    }

    if (!theStyle["topPad"])           theStyle.topPad     = 0;
    if (!theStyle["bottomPad"])        theStyle.bottomPad  = 0;
    if (!theStyle["rightPad"])         theStyle.rightPad   = 0;
    if (!theStyle["leftPad"])          theStyle.leftPad    = 0;
    if (!theStyle["tracking"])         theStyle.tracking   = 0;
    if (!theStyle["lineHeight"])       theStyle.lineHeight = 0;

    if (!theStyle["HTextAlign"])       theStyle.HTextAlign = Renderer.LEFT_ALIGNMENT;
    if (!theStyle["VTextAlign"])       theStyle.VTextAlign = Renderer.TOP_ALIGNMENT;

    if (!theStyle["color"]) {
        theStyle.color = "FFFFFF";
    }

    if (!theStyle["backgroundColor"])  theStyle.backgroundColor  = "000000";

    if (!theStyle["textColor"])        {
        if (theStyle["textcolor"]) {
        theStyle.textColor = theStyle["textcolor"];
        } else if (theStyle["color"]) {
        theStyle.textColor = theStyle["color"];
        } else {
            theStyle.textColor  = "FFFFFF";
        }
    }

    if (!theStyle["fontfile"]) {
        if (theStyle["font"]) {
             theStyle.fontfile   = theStyle["font"];
        } else if (theStyle["face"]) {
             theStyle.fontfile   = theStyle["face"];
        } else {
        Logger.warning("style " + theStyle + " has no font file and no face!");
        }

    if (!fileExists(theStyle.fontfile) && fileExists("FONTS/"+theStyle.fontfile)) {
             theStyle.fontfile = "FONTS/" + theStyle.fontfile;
    }
    }

    if (!theStyle["fontsize"]) {
         if (theStyle["size"]) {
             theStyle.fontsize = theStyle["size"];
         } else if (theStyle["fontSize"]) {
             theStyle.fontsize = theStyle["fontSize"];
         } else {
             theStyle.fontsize   = 20;
         }
    }

    if (!theStyle["name"])             theStyle.name       = theStyle["fontfile"] + "_" + theStyle["fontsize"];


    return theStyle;
}

var DEFAULT_STYLE = addDefaultStyleValues({});

var ourFontStyleCache = {};

function getCachedStyle(theKey) {
        if (theKey == "default") {
        Logger.trace("using default style");
            return DEFAULT_STYLE;
        }
        if (theKey in ourFontStyleCache) {
            var theCachedStyle = ourFontStyleCache[theKey];
        Logger.trace("got cached style for " + theKey + ":");
        }
        return undefined;
}

function getOrCreateCachedStyle(theKey, theTemplate) {
    Logger.trace("getOrCreate cached style for " + theKey);
        var result = getCachedStyle(theKey);
    if (!result) {

        var theItem = addDefaultStyleValues(clone(theTemplate));

        Logger.trace("created new style for " + theKey + ":");
        for (i in theItem) {
            print(i+ ": "+ theItem[i]);
        }
        ourFontStyleCache[theKey] = theItem;
        if (theItem.fontface) {
                   Logger.warning("LOADING FONT "+theItem.name + " " + theItem.fontfile + " " + theItem.fontsize + " " + theItem.fontface);
           window.loadTTF(theItem.name, theItem.fontfile, theItem.fontsize, theItem.fontface);
        } else {
                   Logger.warning("LOADING FONT "+theItem.name + " " + theItem.fontfile + " " + theItem.fontsize + " ");
                   window.loadTTF(theItem.name, theItem.fontfile, theItem.fontsize);
        }
    }
    return getCachedStyle(theKey);
}
