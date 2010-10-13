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
use("SceneViewer.js");
use("Overlay.js");

plug("Cairo");

var ourShow = new SceneViewer(arguments);
var _myImage = null;

ourShow.SceneViewer = [];

ourShow.SceneViewer.setup = ourShow.setup;
ourShow.setup = function() {
    ourShow.SceneViewer.setup();
    window.resize(1024, 768);
    window.canvas.backgroundcolor = new Vector4f(0.8, 0.7, 0.9, 1.0);

    var myImage = Modelling.createImage(window.scene, 1024, 768, "BGRA");
    myImage.resize = "none";

    var myTestOverlay = new ImageOverlay(window.scene, myImage, [0,0]);
    myTestOverlay.width = 1024;
    myTestOverlay.height = 768;

    var xc = 128.0;
    var yc = 128.0;
    var radius = 100.0;
    var angle1 = radFromDeg(45.0);
    var angle2 = radFromDeg(180.0);

    var mySurface = new Cairo.Surface(myImage);
    var cairoContext = new Cairo.Context(mySurface);
    cairoContext.setAntialias(Cairo.ANTIALIAS_NONE);
    cairoContext.setSourceRGB(255,0,0);
    cairoContext.setLineWidth(10.0);
    cairoContext.arc(xc, yc, radius, angle1, angle2);
    cairoContext.stroke();

    // now some txt rendering
    var myLayout = new Pango.Layout(cairoContext);
    var myFontDesc = new Pango.FontDescription("Tele-Antiqua Regular 100");
    myLayout.font_description = myFontDesc;
    myLayout.text = "\u1E78berl\u00E4nge";
    myLayout.update_from_cairo_context(cairoContext);
    myLayout.show_in_cairo_context(cairoContext);
}


try {
    ourShow.setup();
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}
