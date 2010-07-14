use("UnitTest.js");
use("spark/spark.js");

// Widget

function CanvasUnitTest() {
    this.Constructor(this, "CanvasUnitTest");
}

CanvasUnitTest.prototype.Constructor = function (obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);

    function isIdinDom (theDom,theId) {
        var myElement = theDom.getElementById(theId);
        if (myElement) {
            return true;
        } else {
            return false;
        }
    }
    
    function testPrepareMerge() {

        var myOriginalDom = new Node();
        myOriginalDom.parseFile("fixtures/models/cube.x60");

        var myMergedDom = spark.Canvas.prepareMerge("fixtures/models/cube.x60");

        var myWorld = myMergedDom.find(".//world");
        ENSURE_EQUAL(false, isIdinDom(myOriginalDom,myWorld.id), "modified world Id is unique");

        var i,myChildNode;
        for(i=0;i<myWorld.childNodesLength();++i){
            myChildNode = myWorld.childNode(i);
            ENSURE_EQUAL(false, isIdinDom(myOriginalDom,myChildNode.id), "modified " + myChildNode.nodeName + " Id is unique");
        }

        var myCanvasId = myMergedDom.find(".//canvases/canvas").id;
        ENSURE_EQUAL(false, isIdinDom(myOriginalDom,myCanvasId), "modified canvas Id is unique");

        var myViewport = myMergedDom.find(".//viewport");
        ENSURE_EQUAL(false, isIdinDom(myOriginalDom,myViewport.id), "modified viewport Id is unique");

        var myViewportsCameraId = myViewport.camera;
        var myCameraId = myMergedDom.find(".//camera").id;
        ENSURE_EQUAL(myCameraId, myViewportsCameraId, "viewport is using the right camera");

        var imagesNode = myMergedDom.find(".//images");
        for (i = 0; i < imagesNode.childNodesLength(); ++i) {
            ENSURE_EQUAL(true, fileExists(imagesNode.childNodes[i].src), "modified image file path is correct");
        }
    }
    
    function testMergeScene() {
        var myScene = spark.loadFile("fixtures/simple.spark").getScene();

        var myMergeDom = spark.Canvas.prepareMerge("fixtures/models/cube.x60");

        var i,j,receivingNode,mycopyNodes;
        var myNodeList = [];
        for (i = 0; i < myMergeDom.firstChild.childNodesLength(); ++i) {
            mycopyNodes = myMergeDom.firstChild.childNode(i).childNodes;
            for (j = 0; j < mycopyNodes.length; ++j) {
                myNodeList.push( { nodeName : myMergeDom.firstChild.childNode(i).nodeName,
                                   myId     : mycopyNodes[j].id
                                 } );
            }
        }

        spark.Canvas.mergeScenes(myScene,myMergeDom);

        for (i=0; i<myNodeList.length;++i) {
            receivingNode = myScene.dom.getNodesByTagName(myNodeList[i].nodeName)[0];
            ENSURE_EQUAL(true, isIdinDom(receivingNode,myNodeList[i].myId), myNodeList[i].nodeName +
                    " were copied correctly");
        }

    }

    obj.run = function () { 
        testPrepareMerge();
        testMergeScene();
    };
};

// testSuite

try {
    // if (operatingSystem() === 'OSX') {}
    var myScriptPathSegments = __FILE__().split("/");
    var mySuiteName = myScriptPathSegments[myScriptPathSegments.length - 1];
    var mySuite = new UnitTestSuite(mySuiteName);
    
    mySuite.addTest(new CanvasUnitTest());
    mySuite.run();
    
    print(">> Finished test suite '" + mySuiteName + "', return status = " + mySuite.returnStatus() + "");
    exit(mySuite.returnStatus());
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(1);
}

