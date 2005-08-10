//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: ImageManager.js,v $
//   $Author: david $
//   $Revision: 1.7 $
//   $Date: 2005/04/04 14:40:15 $
//
//
//=============================================================================


function ImageManager(theImagesNode, theScene) {
    this.Constructor(this, theImagesNode, theScene);
}

ImageManager.prototype.Constructor = function(obj, theImagesNode, theScene) {

    // private members
    var _myImagesNode = theImagesNode;
    var _myScene = theScene ? theScene : window.scene;

    // public methods
    obj.getImageNode = function(theImageName) {
        var myImageNode = getDescendantByName(_myImagesNode, theImageName, false);
        if (!myImageNode) {
            myImageNode = Node.createElement("image");
            _myImagesNode.appendChild(myImageNode);
            myImageNode.name = theImageName;
            myImageNode.resize = "pad";
        }
        return myImageNode;
    }

    obj.createImage = function(theSource) {
        var myImageNode = Node.createElement("image");
        _myImagesNode.appendChild(myImageNode);
        myImageNode.name = theSource;
        myImageNode.src = theSource;
        myImageNode.resize = "pad";
        return myImageNode;
    }

    obj.createMovie = function(theSource) {
        var myMovie = Node.createElement("movie");
        _myImagesNode.appendChild(myMovie);
        myMovie.src = theSource;
        myMovie.name = theSource;
        window.loadMovieFrame(myMovie);
        return myMovie;
    }
}

function getImageSize(theImage) {
    var myImageMatrix = new Matrix4f(theImage.matrix);
    myImageMatrix.setRow(3, [0,0,0,1]);
    var mySize = new Vector3f(theImage.width, theImage.height, 0);
    mySize = product(mySize, myImageMatrix);
    return new Vector2i(Math.abs(Math.round(mySize.x)), Math.abs(Math.round(mySize.y)));
}


