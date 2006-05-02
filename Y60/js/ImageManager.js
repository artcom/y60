//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

function ImageManager(theBaseViewer) {
    this.Constructor(this, theBaseViewer);
}

ImageManager.prototype.Constructor = function(obj, theBaseViewer) {

    // private members
    var _myBaseViewer = theBaseViewer;

    // public methods
    obj.getImageNode = function(theImageName) {
        var myImageNode = getDescendantByName(_myBaseViewer.getImages(), theImageName, false);
        if (!myImageNode) {
            myImageNode = Node.createElement("image");
            _myBaseViewer.getImages().appendChild(myImageNode);
            myImageNode.name = theImageName;
            myImageNode.resize = "pad";
        }
        return myImageNode;
    }

    obj.createImage = function(theSource) {
        var myImageNode = Node.createElement("image");
        _myBaseViewer.getImages().appendChild(myImageNode);
        myImageNode.name = theSource;
        myImageNode.src = theSource;
        myImageNode.resize = "pad";
        return myImageNode;
    }

    obj.createMovie = function(theSource) {
        var myMovie = Node.createElement("movie");
        _myBaseViewer.getImages().appendChild(myMovie);
        myMovie.src = theSource;
        myMovie.name = theSource;
        window.scene.loadMovieFrame(myMovie);
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
