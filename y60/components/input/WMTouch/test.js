/*jslint plusplus:false*/
/*globals use, spark, Node, Frustum, ProjectionType, Vector3f, Quaternionf,
          radFromDeg, print, Planef, TrackballMover, exit*/

plug("WMTouch");
use("spark/spark.js");

var ourShow = spark.loadFile("fixtures/testAlphaVideo.spark");
ourShow.parseArguments(arguments);

ourShow.getRenderWindow().addExtension(new WMTouch());

var _ = {};

_.initialize = function () {
    _.cursorMaterial = Modelling.createUnlitTexturedMaterial(ourShow.getRenderWindow().scene, "IMAGES/cursor.png", "cursorMaterial", true, false);
    var myTexture = ourShow.getRenderWindow().scene.dom.getElementById(_.cursorMaterial.find("./textureunits/textureunit").texture);
    var myImage = ourShow.getRenderWindow().scene.dom.getElementById(myTexture.image);
    _.cursorSize = myImage.raster.size;
};

_.addCursor = function (theId, theScreenPos) {
    var myOverlay = Node.createElement("overlay");
    myOverlay.material = _.cursorMaterial.id;
    myOverlay.height = _.cursorSize.y;
    myOverlay.width = _.cursorSize.x;
    myOverlay.position = difference(theScreenPos, product(_.cursorSize, 0.5));
    myOverlay.id = "cursor_" + theId;
    ourShow.getActiveViewport().childNode("overlays").appendChild(myOverlay);
};
_.updateCursor = function (theId, theScreenPos) {
    var myOverlay = ourShow.getRenderWindow().scene.dom.getElementById("cursor_" + theId);
    if (myOverlay) {
        myOverlay.position = difference(theScreenPos, product(_.cursorSize, 0.5));
    }
};
_.removeCursor = function (theId) {
    var myOverlay = ourShow.getRenderWindow().scene.dom.getElementById("cursor_" + theId);
    if (myOverlay) {
        myOverlay.parentNode.removeChild(myOverlay);
    }
};


ourShow.onWMTouch = function(e) {
    if (e.type == "down") {
        _.addCursor(e.id, e.position);
    } else if (e.type == "move") {
        _.updateCursor(e.id, e.position);
    } else if (e.type == "up") {
        _.removeCursor(e.id);
    }
};

try {
    _.initialize();
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}

