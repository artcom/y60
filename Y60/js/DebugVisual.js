//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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
        if (getDescendantById(_mySceneViewer.getMaterials(), "mCoordinateSystem") == null) {
            _myMaterial = _mySceneViewer.getScene().createColorMaterial([1,1,1,1]);
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
            _mySceneViewer.getShapes().appendChild(_myShapeNode);
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

    obj.onKey = function(theKey, theKeyState, theX, theY) {
        if (!theKeyState) {
            return;
        }
        switch (theKey) {
            case 'g':
                obj.showAllCoordinates(!_isEnabled);
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
            var myCoordNode = getDescendantByName(theNode, "b_coord_" + theNode.id, true);
            if (myCoordNode) {
                theNode.removeChild(myCoordNode);
            }
        }
    }

    function printHelp() {
         print("Debug:");
         print("    g    toggle coordinate system");
    }
}
