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

plug("ProcessFunctions");
use("Y60JSSL.js");
use("Overlay.js");

const OBJECTS_PER_FRAME = 30;
const ALLOWED_MEMORY_INCREASE = 2500*2; // in KB, doubled due to debug execution (vs)
function createObjects(theTime) {
    var myPos = Math.sin(theTime) * 3;
    for (var i = 0; i < OBJECTS_PER_FRAME; ++i) {
        ourCounter++;
        ourObjects[i] = {};
        ourObjects[i].material = Modelling.createColorMaterial(window.scene, [Math.sin(theTime),Math.cos(theTime),1,1]);
        ourObjects[i].shape    = Modelling.createQuad(window.scene, ourObjects[i].material.id, [-1 + myPos,-1,-10], [1 + myPos,1,-10]);
        ourObjects[i].body     = Modelling.createBody(window.scene.world, ourObjects[i].shape.id);

        //ourObjects[i].image = Node.createElement("image");
        //window.scene.images.appendChild(ourObjects[i].image);
        //ourObjects[i].image.src = "testfiles/DiffuseRamp.png";
        //ourObjects[i].overlay = new ImageOverlay(window.scene, "testfiles/DiffuseRamp.png", [0, 0]);
    }
}

function removeObjects() {
    for (var i = 0; i < OBJECTS_PER_FRAME; ++i) {
        ourObjects[i].body.parentNode.removeChild(ourObjects[i].body);
        ourObjects[i].shape.parentNode.removeChild(ourObjects[i].shape);
        ourObjects[i].material.parentNode.removeChild(ourObjects[i].material);
        //ourObjects[i].image.parentNode.removeChild(ourObjects[i].image);
        //ourObjects[i].overlay.removeFromScene();
    }
}

var window = new RenderWindow();

window.onKey = function(theKey) {
    if (theKey == "escape") {
        ourExitFlag = true;
    }
}
window.onFrame = function(theTime) {
    removeObjects();
    createObjects(theTime);

    ourMemoryStater--;
    if (!ourInitialMemory && ourMemoryStater == 0) {
        ourInitialMemory = getProcessMemoryUsage();
    }

    var currentUsage = getProcessMemoryUsage();
    Logger.info("intial = " + ourInitialMemory +" kb, current = " + currentUsage + ", delta = " + (currentUsage - ourInitialMemory) + ", ="+(currentUsage - ourInitialMemory)/1024 + " MB");
    if (ourMemoryStater < -50 || ourExitFlag) {
        var myMemory = getProcessMemoryUsage();
        removeObjects();
        print("Objects created: " + ourCounter);
        var myNodeCount = countNodes(window.scene.dom);
        print("myNodeCount=" + myNodeCount);
        if (myNodeCount != ourInitialNodeCount) {
            Logger.error("Nodecount increased by: " + (myNodeCount - ourInitialNodeCount));
            exit(1);
        }
        var myMemoryIncrease = (myMemory - ourInitialMemory);
        if (myMemoryIncrease > ALLOWED_MEMORY_INCREASE) {
            Logger.error("Memory usage increased by: " + myMemoryIncrease + " KB");
            exit(1);
        } else {
            Logger.trace(window.scene.dom);
            print("Memory usage increased by: " + myMemoryIncrease + " KB");
        }
        exit(0);
    }

    Logger.trace("Memory Usage: " + asMemoryString(getProcessMemoryUsage()) + ", Nodecount: " + countNodes(window.scene.dom) + " fps: " + Number(window.fps).toFixed(0));
}

var ourObjects          = [];
var ourMemoryStater     = 50;
var ourInitialNodeCount = countNodes(window.scene.dom);
var ourInitialMemory    = null;
var ourCounter          = 0;
var ourExitFlag         = false;

createObjects(0);
window.go();
