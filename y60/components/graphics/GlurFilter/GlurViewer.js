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

use("Overlay.js");
plug("GlurFilter");

if (arguments.length < 1) {
    print("Usage: GlurViewer filename");
    exit(1);
}

var myFilename = arguments[0];

if (!fileExists(myFilename)) {
    print("### ERROR: File does not exist: " + myFilename);
    exit(0);
}

function glurImage(theImageNode) {
    print("Gluring image: " + theImageNode.src);

    var a = [];

    // this actually is the diameter (must not be even!)
    // only half of the calculated values are actually used (quick hack)
    var radius = 3;  //65;
    for (var x = 0; x < radius; x++) {
        var z = ((x + 2)/(radius));
        var dev = 0.5 + (((radius * radius)) * 0.001); // you can adjust this factor to change the shape/center-weighting of the gaussian
        var y = Math.pow((1.0 / Math.sqrt(2.0 * 3.14159265359 * dev)), ((-(z - 1.0) * -(x - 1.0)) / (2.0 * dev)));
        if (x+1>radius/2) { // second half (including center value) is used only
            a.push( y );
        }
    }

    // make sure, all values sum up to 1.0 (play with this value!)
    var sum = 0;
    for (x = 0; x < a.length; x++) {
        sum += a[x];
    }

    for (x = 0; x < a.length; x++) {
        a[x] *= (1.0/sum);
    }

    theImageNode.filter = "glur";
    theImageNode.filter_params = "[1.0,1.0,1.0,1.0," + a + "]";
}


var window = new RenderWindow();

var myOverlay = new ImageOverlay(window.scene, myFilename, [0,0]);
myOverlay.position = [300, 100];
window.canvas.backgroundcolor = [0.5,0.5,0.5,1];
glurImage(myOverlay.image);


window.onStartMainLoop = function() {
    window.loadTTF("Arial", "${PRO}/testmodels/fonts/arial.ttf", 18);
    var myImageNode = window.scene.images.appendChild(new Node("<image/>").firstChild);
    glurImage(myImageNode);
    window.renderTextAsImage(myImageNode, "hallo welt", "Arial", new Node("<style/>"), 200, 100)
    var myTextOverlay = new ImageOverlay(window.scene, myImageNode, [100,100]);
    //saveImage(myImageNode, "test.png");
    print(myImageNode.filter_params);
    applyImageFilter(myImageNode, "glur", [1,1,1,1,0.547441,0.452559]);
}


window.go();
