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
//
//   $RCSfile: DemoRendererExtension.js,v $
//   $Author: christian $
//   $Revision: 1.6 $
//   $Date: 2004/10/31 15:58:17 $
//
//
//=============================================================================

if (__main__ == undefined) var __main__ = "VideoProcessingExtension";

use("ImageViewer.js");
plug("VideoProcessing");


function VideoProcessingExtensionApp(theArguments) {
    this.Constructor(this, theArguments);
}

VideoProcessingExtensionApp.prototype.Constructor = function(self, theArguments) {

    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    ImageViewerApp.prototype.Constructor(self, theArguments);
    var Base = [];
    var _myVideoProcessingExtension = new VideoProcessingExtension();
    var _myOverlay = null;
    var _myPalette = null;

    //////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    //////////////////////////////////////////////////////////////////////

    // setup
    Base.setup = self.setup;
    self.setup = function(theWidth, theHeight, theTitle) {
        Base.setup(theWidth, theHeight, false, theTitle);
        window.addExtension(_myVideoProcessingExtension);
        _myVideoProcessingExtension.init(window.scene);
        createOverlay();

        
        var myConfig = Node.createElement("algorithms");
        var myAlgorithmNode = Node.createElement("algorithm");
        myAlgorithmNode.name = "shot-detection";
        myConfig.appendChild(myAlgorithmNode);
        var myImage = window.scene.images.getElementById(_myOverlay.material.childNode("textures").firstChild.image);
        myAlgorithmNode.appendChild(createProperty("sourceimage", myImage.id));
        myAlgorithmNode.appendChild(createProperty("threshold", 0.95))
        myAlgorithmNode.appendChild(createProperty("minimal_length", 0.5))
        _myVideoProcessingExtension.configuration = myConfig;
        
    }

    Base.onKey = self.onKey;
    self.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        if (theState) {
            switch (theKey) {
                case "t":
                    //self.getMovieNode().playmode = "pause";
                    print(_myVideoProcessingExtension.result);

                    break;
                default:
                    Base.onKey(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
            }
        }
    }

    var _myShotLength = 0;
    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        var myResult = _myVideoProcessingExtension.result;
        if( myResult.childNode(0) && myResult.childNode(0).childNodesLength() > 0 ) {
            print(myResult.childNode(0).childNode(0).childNode("histogram"));
        }
        //var myLength = myResult.childNodes.length;

//         if (myLength > _myShotLength) {
//             showHistogram();
//             showShotInfo();
//             _myShotLength = myLength;
//         }
        Base.onFrame(theTime);

    }


    ////////////// private members ////////////

    function createPalette(nbins) {
        var bpc = Math.round(Math.pow(nbins,1./3));
        _myPalette = [];
        for(var r = 0; r < bpc; ++r) {
            for(var g = 0; g < bpc; ++g) {
                for(var b = 0; b < bpc; ++b) {
                    var myColor = new Vector3f( r/(bpc-1), g/(bpc-1), b/(bpc-1));
                    _myPalette.push(myColor);
                }
            }
        }
    }

    function createProperty(theName, theValue) {
        var myProperty = Node.createElement("property");
        myProperty.name = theName;
        myProperty.value = theValue;        
        return myProperty;
    }

    function showShotInfo() {
        var myResult = _myVideoProcessingExtension.result;
        var myLast = myResult.childNodes.length - 1;

        var myString = "";
        myString += "Total Shots " + myResult.childNodes.length + "\n";
        myString += "Current shot begin : " + myResult.childNode(myLast).start + "\n";
        if (myLast > 0) {
            myString += "Last shot length : " + myResult.childNode(myLast-1).length + "\n";
        }
        self.setMessage(myString);
    }

    function showHistogram() {

        var myResult = _myVideoProcessingExtension.result;
        var myLast = myResult.childNodes.length - 1;

        var myNode = myResult.childNode(myLast).childNode("histogram").childNode(0);
        var myHistogram = stringToArray(myNode.nodeValue);

        if (_myPalette == null) {
            createPalette(myHistogram.length);
        }

        var myImage = self.getImageManager().getImageNode("histogram");
        var myRasterData = myImage.childNode(0).childNode(0).nodeValue;

        var myCellWidth = 20;
        var myCellHeight = 20;
        var myCellsPerLine = 40;

        var myMaximum = 0;
        for(var k = 0;k < myHistogram.length; ++k) {
            myHistogram[k] = Number(myHistogram[k]);
            if (myHistogram[k] > myMaximum) {
                myMaximum = myHistogram[k];
            }
        }
        for(k = 0;k < myHistogram.length; ++k) {
            var j = Math.floor(k % myCellsPerLine);
            var i = Math.floor(k / myCellsPerLine);
            var x = 10 + j*(myCellWidth+5);
            var y = 10 + i*(myCellHeight+5);

            var myColor = _myPalette[k];
            var myAlpha = Math.log(myHistogram[k]) / Math.log(myMaximum);
            if (myAlpha < 0) {
                myAlpha = 0;
            }
            //print(" k " + k  + " max " + myMaximum + "hist " + myHistogram[k] + " alhpa " + myAlpha);
            myRasterData.fillRect(x,y,x+myCellWidth,y+myCellHeight,
                                  255*myColor[0],255*myColor[1],255*myColor[2],255*myAlpha);

        }

        _myOverlay.visible = true;
    }
    function createOverlay() {

        var myImage = self.getImageManager().getImageNode("histogram");
        myImage.src = "overlay.jpg";

        _myOverlay = new ImageOverlay(window.scene, myImage);
        _myOverlay.width  = myImage.width;
        _myOverlay.height = myImage.height;
        _myOverlay.color = new Vector4f(1,1,1,1.0);
        _myOverlay.visible = false;

    }

}

//
// main
//

if (__main__ == "VideoProcessingExtension") {
    print("VideoProcessingExtension");
    try {
        var ourApp = new VideoProcessingExtensionApp(arguments);

        ourApp.setup(1024, 800, "VideoProcessingExtension");
        ourApp.go();
    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
        exit(1);
    }
}


