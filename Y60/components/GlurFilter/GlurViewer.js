//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

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
    window.renderTextAsImage(myImageNode, "hallo welt", "Arial", 200, 100)
    var myTextOverlay = new ImageOverlay(window.scene, myImageNode, [100,100]);
    //saveImage(myImageNode, "test.png");
    print(myImageNode.filter_params);
    applyImageFilter(myImageNode, "glur", [1,1,1,1,0.547441,0.452559]);
}


window.go();
