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
*/

/*global use, Namespace, Node*/

// object system
use("common.js");

/**
 * The SPARK namespace.
 *
 * Everything defined by the spark library goes into this namespace.
 */
var spark = Namespace("spark");

spark.findRootElement = function (theDocument) {
    var myRoot = (theDocument.nodeType === Node.DOCUMENT_NODE) ? theDocument.firstChild 
                                                               : theDocument;
    // firstChild might be a xml processing instruction, a
    // comment, a doctype declaration or god knows what else.
    while (myRoot.nodeType !== Node.ELEMENT_NODE) {
        if (! myRoot.nextSibling) {
            throw new Error("spark document contains no xml element");
        }
        myRoot = myRoot.nextSibling;
    }
    return myRoot;
};

spark.CALL_PROPERTY_HANDLER_ON_CHANGE = 1;
spark.CALL_PROPERTY_HANDLER_ON_CALL   = 0;
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
// cursor object
use("cursor.js");
// event handling
use("events.js");
// multitouch support
use("multitouch.js");
// internationalization
use("internationalization.js");
// dsa sensor support
use("dsa.js");
// EventWrapper to convert SPARK into JS callbacks
use("CallbackWrapper.js");

// TODO Use an autoloading mechanism for this!

// widgets
if (!("disableSparkWidgets" in this)) {
    // base class
    use("spark/widgets.js");
    // concrete classes (inheritance-ordered)
    use("spark/widget/Transform.js");
    use("spark/widget/Switch.js");
    use("spark/widget/Stage.js");
    use("spark/widget/OffscreenStage.js");
    use("spark/widget/World.js");
    use("spark/widget/Window.js");
    use("spark/widget/SparkSceneTesterWindow.js");
    use("spark/widget/Model.js");
    use("spark/widget/Body.js");
    use("spark/widget/ResizableRectangle.js");
    use("spark/widget/Rectangle.js");
    use("spark/widget/Image.js");
    use("spark/widget/Text.js");
    use("spark/widget/Movie.js");
    use("spark/widget/Canvas.js");
    use("spark/widget/AtlasImage.js");
    use("spark/add-ons/PangoText.js");
    use("spark/add-ons/RoundedCornerQuad.js");
    use("spark/add-ons/RoundedCornerMovie.js");
    use("spark/add-ons/RoundedCornerImage.js");
    use("spark/add-ons/StretchyImage.js");
    use("spark/add-ons/StretchyAtlasImage.js");
    use("spark/add-ons/VideoCapture.js");
    use("spark/add-ons/Slider.js");
    use("spark/add-ons/MultiQuadText.js");
    use("spark/add-ons/GlowWidgets.js");
}
