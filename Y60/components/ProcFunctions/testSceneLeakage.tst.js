plug("ProcFunctions");
use("Y60JSSL.js");
use("ImageManager.js");
use("Overlay.js");

const OBJECTS_PER_FRAME = 30;

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

var myShaderLibrary = "../../../../shader/shaderlibrary.xml";
if (myShaderLibrary) {
    Logger.info("loading shader lib:"+myShaderLibrary);
    if (operatingSystem() != "OSX") {
        GLResourceManager.loadShaderLibrary(myShaderLibrary,"vp40", "fp40");
    } else {
        GLResourceManager.loadShaderLibrary(myShaderLibrary,"arbvp1", "arbfp1");
    }
} else {
    Logger.warning("No Shaderlibrary found. Reload manually");
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
    Logger.info("intial = " + ourInitialMemory +" kb, current = " + currentUsage + ", delta = " + (currentUsage - ourInitialMemory) + ", ="+(currentUsage - ourInitialMemory)/1024 + " kb");
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
        var myMemoryIncrease = (myMemory - ourInitialMemory) / 1024;
        if (myMemoryIncrease > 550) {
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
