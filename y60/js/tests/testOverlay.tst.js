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

/*jslint */
/*globals use, UnitTest, Node, ENSURE, Vector4f, DPRINT, Vector2f,
          ImageOverlay, RenderWindow, Overlay, print, UnitTestSuite,
          exit, arrayToString*/

// TODO:
// - Test parent handling

use("UnitTest.js");
use("Overlay.js");
use("Y60JSSL.js");

var window = null;

function OverlayUnitTest() {
    this.Constructor(this, "OverlayUnitTest");
}

OverlayUnitTest.prototype.Constructor = function (obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.myDummyOverlay = null;
    obj.myScene     = null;

    obj.getAttribute = function (theNode, theAttributeName) {
        var myNode = new Node(theNode).firstChild;
        return myNode[theAttributeName];
        /*
        var myRegExp = new RegExp(theAttributeName + '=\\"(.\\w)\\"');
        myRegExp.exec(String(theNode));
        return RegExp.$1;*/
    };

    obj.getLastOverlay = function () {
        return obj.myScene.dom.find(".//overlays").lastChild;
    };

    function testCommonProperties(thePosition, theSize, theColor) {
        // Test toString operator
        obj.myOverlayString = String(obj.myOverlay);
        ENSURE('obj.myOverlayString.substr(0,9) == "<overlay "');
        ENSURE('obj.myOverlay.color == ' + theColor);

        // Test material convenience property
        obj.myMaterialNode = obj.myScene.dom.getElementById(obj.getAttribute(obj.myOverlay, "material"));
        ENSURE('obj.myOverlay.material.id == "' + obj.myMaterialNode.id + '"');
        obj.myOverlay.material = obj.myDummyOverlay.material;

        ENSURE('obj.myOverlay.material.id == obj.myDummyOverlay.material.id');
        ENSURE('obj.getAttribute(obj.myOverlay, "material") == obj.myDummyOverlay.material.id');
        ENSURE('obj.myOverlay.color == 1,1,1,1');
        obj.myOverlay.material = obj.myMaterialNode;

        // Test color convenience property
        obj.myOverlay.color = new Vector4f(4, 3, 2, 1);
        obj.myColor = obj.myOverlay.material.find(".//*[@name='surfacecolor']").firstChild.nodeValue;
        ENSURE('almostEqual(obj.myOverlay.color, obj.myColor)');

        // Test move to top
        ENSURE('String(obj.myOverlay) == String(obj.getLastOverlay())');
        obj.myDummyOverlay.moveToTop();
        ENSURE('String(obj.myOverlay) != String(obj.getLastOverlay())');
        ENSURE('String(obj.myDummyOverlay) == String(obj.getLastOverlay())');
        obj.myOverlay.moveToTop();

        // Paranoid touches test
        obj.thePosition = thePosition;
        obj.theSize = theSize;
        DPRINT('thePosition');
        DPRINT('theSize');
        DPRINT('obj.myOverlay');
        ENSURE('obj.myOverlay.touches(thePosition[0], thePosition[1])');
        ENSURE('!obj.myOverlay.touches(thePosition[0] - 1, thePosition[1])');
        ENSURE('!obj.myOverlay.touches(thePosition[0], thePosition[1] - 1)');
        ENSURE('obj.myOverlay.touches(thePosition[0] + theSize[0] - 1, thePosition[1] + theSize[1] - 1)');
        ENSURE('!obj.myOverlay.touches(thePosition[0] + theSize[0] - 1, thePosition[1] + theSize[1])');
        ENSURE('!obj.myOverlay.touches(thePosition[0] + theSize[0], thePosition[1] + theSize[1] - 1)');
        ENSURE('obj.myOverlay.touches(thePosition[0], thePosition[1], 0)');
        ENSURE('!obj.myOverlay.touches(thePosition[0] - 1, thePosition[1], 0)');
        ENSURE('!obj.myOverlay.touches(thePosition[0], thePosition[1] - 1, 0)');
        ENSURE('obj.myOverlay.touches(thePosition[0] + theSize[0] - 1, thePosition[1] + theSize[1] - 1, 0)');
        ENSURE('!obj.myOverlay.touches(thePosition[0] + theSize[0] - 1, thePosition[1] + theSize[1], 0)');
        ENSURE('!obj.myOverlay.touches(thePosition[0] + theSize[0], thePosition[1] + theSize[1] - 1, 0)');
        ENSURE('obj.myOverlay.touches(thePosition[0] - 1, thePosition[1] - 1, 1)');
        ENSURE('!obj.myOverlay.touches(thePosition[0] - 2, thePosition[1] - 1, 1)');
        ENSURE('!obj.myOverlay.touches(thePosition[0] - 1, thePosition[1] - 2, 1)');
        ENSURE('obj.myOverlay.touches(thePosition[0] + theSize[0], thePosition[1] + theSize[1], 1)');
        ENSURE('!obj.myOverlay.touches(thePosition[0] + theSize[0] + 1, thePosition[1] + theSize[1], 1)');
        ENSURE('!obj.myOverlay.touches(thePosition[0] + theSize[0], thePosition[1] + theSize[1] + 1, 1)');

        ///////////////////////////////////////////////////////////////////////////////////////
        //
        // Test attribute access
        //
        ///////////////////////////////////////////////////////////////////////////////////////

        obj.myOverlay.name = "nina";
        ENSURE('obj.myOverlay.name == "nina"');

        ENSURE('obj.myOverlay.visible');
        obj.myOverlay.visible = false;
        ENSURE('!obj.myOverlay.visible');

        ENSURE('almostEqual(obj.myOverlay.position, ' + arrayToString(thePosition) + ')');
        obj.myOverlay.position = new Vector2f(2, 1);
        ENSURE('almostEqual(obj.myOverlay.position, [2,1])');

        ENSURE('almostEqual(obj.myOverlay.srcorigin, [0,0])');
        obj.myOverlay.srcorigin = new Vector2f(1, 2);
        ENSURE('almostEqual(obj.myOverlay.srcorigin, [1,2])');

        ENSURE('almostEqual(obj.myOverlay.srcsize, [1,1])');
        obj.myOverlay.srcsize = new Vector2f(3, 4);
        ENSURE('almostEqual(obj.myOverlay.srcsize, [3,4])');

        ENSURE('obj.myOverlay.alpha == 1');
        obj.myOverlay.alpha = 0.5;
        ENSURE('obj.myOverlay.alpha == 0.5');

        ENSURE('obj.myOverlay.width == ' + theSize[0]);
        obj.myOverlay.width = 30;
        ENSURE('obj.myOverlay.width == 30');

        ENSURE('obj.myOverlay.height == ' + theSize[1]);
        obj.myOverlay.height = 10;
        ENSURE('obj.myOverlay.height == 10');

        ENSURE('almostEqual(obj.myOverlay.bordercolor, [0,0,0,1])');
        obj.myOverlay.bordercolor = new Vector4f(1, 2, 3, 4);
        ENSURE('almostEqual(obj.myOverlay.bordercolor, [1,2,3,4])');

        ENSURE('obj.myOverlay.borderwidth == 1');
        obj.myOverlay.borderwidth = 3;
        ENSURE('obj.myOverlay.borderwidth == 3');

        ENSURE('!obj.myOverlay.topborder');
        obj.myOverlay.topborder = true;
        ENSURE('obj.myOverlay.topborder');

        ENSURE('!obj.myOverlay.bottomborder');
        obj.myOverlay.bottomborder = true;
        ENSURE('obj.myOverlay.bottomborder');

        ENSURE('!obj.myOverlay.leftborder');
        obj.myOverlay.leftborder = true;
        ENSURE('obj.myOverlay.leftborder');

        ENSURE('!obj.myOverlay.rightborder');
        obj.myOverlay.rightborder = true;
        ENSURE('obj.myOverlay.rightborder');
    }

    function testRemoveFromScene() {
        ENSURE('obj.myScene.dom.getElementById(obj.myOverlay.material.id) != undefined');
        ENSURE('obj.myScene.dom.getElementById(obj.myOverlay.image.id) != undefined');
        ENSURE('String(obj.myOverlay) == String(obj.getLastOverlay())');
        obj.myOverlay.removeFromScene();
        ENSURE('obj.myScene.dom.getElementById(obj.myOverlay.material.id) == undefined');
        ENSURE('obj.myScene.dom.getElementById(obj.myOverlay.image.id) == undefined');
        ENSURE('String(obj.myOverlay) != String(obj.getLastOverlay())');
    }

    obj.run = function () {
        // Create empty scene
        window = new RenderWindow();

        obj.myScene        = window.scene;
        obj.myDummyOverlay = new ImageOverlay(obj.myScene, "fixtures/black.rgb");
        obj.myDummyImageId = obj.myScene.images.lastChild.id;
        obj.myOverlay      = new Overlay(obj.myScene, new Vector4f(1, 2, 3, 1), [10, 20], [100, 200]);
        //testCommonProperties([10,20], [100,200], [1,2,3,1]);

        obj.myOverlay = new ImageOverlay(obj.myScene, "fixtures/DiffuseRamp.png", [30, 40]);
        obj.myImageId = obj.myScene.images.lastChild.id;
        //testCommonProperties([30,40], [32,1], [1,1,1,1]);

        ///////////////////////////////////////////////////////////////////////////////////////
        //
        // Test multiple image support
        //
        ///////////////////////////////////////////////////////////////////////////////////////

        var mySources = ["fixtures/black.rgb", "fixtures/DiffuseRamp.png"];
        obj.myMultiOverlay = new ImageOverlay(obj.myScene, mySources);
        ENSURE('obj.myMultiOverlay.images.length == 2');

        obj.myMultiOverlay.position = new Vector2f(300, 300);

        var myImages = obj.myMultiOverlay.images;
        myImages.push(obj.myMultiOverlay.images[0].cloneNode(true));
        // the following wont work, we modified images in place
        // ENSURE('obj.myMultiOverlay.images.length == 2');
        obj.myMultiOverlay.images = myImages;
        ENSURE('obj.myMultiOverlay.images.length == 3');
        print("reset all images");
        myImages = [];
        myImages.push(obj.myMultiOverlay.images[0]);
        obj.myMultiOverlay.images = myImages;
        ENSURE('obj.myMultiOverlay.images.length == 1');

        ///////////////////////////////////////////////////////////////////////////////////////
        //
        // Test texture and image convenience property
        //
        ///////////////////////////////////////////////////////////////////////////////////////

        obj.myMaterialNode = obj.myScene.dom.getElementById(obj.getAttribute(obj.myOverlay, "material"));
        obj.myTextureUnitNode = obj.myMaterialNode.find(".//textureunit");
        obj.myTextureNode = obj.myMaterialNode.getElementById(obj.myTextureUnitNode.texture);

        ENSURE('obj.myTextureNode.image == obj.myOverlay.texture.image');
        ENSURE('obj.myTextureNode.image == obj.myOverlay.image.id');
    };
};

function main() {
    var myTestName = "testOverlay.tst.js";
    var mySuite = new UnitTestSuite(myTestName);

    mySuite.addTest(new OverlayUnitTest());
    mySuite.run();

    print(">> Finished test suite '" + myTestName + "', return status = " + mySuite.returnStatus() + "");
    return mySuite.returnStatus();
}

if (main() !== 0) {
    exit(1);
}