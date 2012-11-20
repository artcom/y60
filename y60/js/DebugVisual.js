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
//
//   $RCSfile: DebugVisual.js,v $
//   $Author: christian $
//   $Revision: 1.7 $
//   $Date: 2005/04/20 16:49:06 $
//
//
//=============================================================================

use("ShapeBuilder.js");

// DebugVisual Constructor
function DebugVisual(theWorld, theSceneViewer) {
    this.Constructor(this, theWorld, theSceneViewer);
}

DebugVisual.prototype.Constructor = function(obj, theWorld, theSceneViewer) {
    var _myWorld        = theWorld;
    var _isEnabled      = false;
    var _mySceneViewer  = theSceneViewer;
    var _myShapeNode    = null;
    var _myMaterial     = null;

    obj.setup = function() {
        if(window.scene.materials.getElementById("mCoordinateSystem") == null) {
            _myMaterial = Modelling.createColorMaterial(_mySceneViewer.getScene(),[1,1,1,1]);
            var myVertexparamsFeatures = new Node('<feature name="vertexparams">[0[]]</feature>\n').firstChild;
            _myMaterial.requires.appendChild(myVertexparamsFeatures);
            _myMaterial.requires.vertexparams = "[100[color]]";

            var myShapeBuilder = new ShapeBuilder("s_CoordinateSystem");
            myShapeBuilder.appendNormal([0,0,0]);

            var myElement = myShapeBuilder.appendElement("lines", _myMaterial.id);
            myShapeBuilder.appendColor("[1,0,0,1]");
            myShapeBuilder.appendLineElement(myElement, [0,0,0], [1,0,0]);
            myShapeBuilder.appendColor("[0,1,0,1]");
            myShapeBuilder.appendLineElement(myElement, [0,0,0], [0,1,0]);
            myShapeBuilder.appendColor("[0,0,1,1]");
            myShapeBuilder.appendLineElement(myElement, [0,0,1], [0,0,0]);

            _myShapeNode = myShapeBuilder.buildNode();
            window.scene.shapes.appendChild(_myShapeNode);
        }
    }

    obj.showAllCoordinates = function(theFlag) {

        // Lazy creation
        if (!_myShapeNode) {
            obj.setup();
        }
        if (_isEnabled != theFlag) {
            showCoordinate(_myWorld, theFlag);
            for (var i = 0; i < _myWorld.childNodes.length; ++i) {
               var myChildNode = _myWorld.childNodes[i];
               if (myChildNode.nodeName == "body" && myChildNode.shape!="s_CoordinateSystem") {
                   showCoordinate(myChildNode, theFlag);
                }
            }
            _isEnabled = theFlag;
        }
    }

    obj.onKey = function(theKey, theKeyState, theShiftFlag) {
        if (!theKeyState) {
            return;
        }
        switch (theKey) {
            case 'g':
                if (theShiftFlag) {
                    obj.showAllCoordinates(!_isEnabled);
                }
                break;
            case 'h':
            case 'H':
                printHelp();
                break;

        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // private members
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    function showCoordinate(theNode, theFlag) {
        if (theFlag) {
            if (!theNode.boundingbox.isEmpty) {
                var myScaleFactor = distance(theNode.boundingbox[0], theNode.boundingbox[1]);
                var myBodyNode = new Node('<body/>').firstChild;
                myBodyNode.name = "b_coord_" + theNode.id;
                myBodyNode.shape = _myShapeNode.id;
                myBodyNode.scale = new Vector3f(myScaleFactor, myScaleFactor, myScaleFactor);
                theNode.appendChild(myBodyNode);
            }
        } else {
            var myCoordNode = theNode.find(".//*[@name = 'b_coord_" + theNode.id + "']");
            if (myCoordNode) {
                theNode.removeChild(myCoordNode);
            }
        }
    }

    function printHelp() {
         print("Debug:");
         print("    G    toggle coordinate system");
    }
}
