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
    obj.getImageNode = function(theImageName, theEncoding) {
        var myImageNode = getDescendantByName(window.scene.images, theImageName, false);
        if (!myImageNode) {
            var myEncoding = (theEncoding != undefined) ? theEncoding.toUpperCase() : "RGB";
            myImageNode = Modelling.createImage(window.scene, 1,1,myEncoding);
            myImageNode.name = theImageName;
        }
        return myImageNode;
    }

    obj.createImage = function(theSource, theEncoding) {
        var myImageNode = Node.createElement("image");
        window.scene.images.appendChild(myImageNode);
        myImageNode.name = theSource;
        myImageNode.src = theSource;
        myImageNode.resize = "pad";
        return myImageNode;
    }

    obj.createMovie = function(theSource, theDecoderHint) {
        var myMovie = Node.createElement("movie");
        window.scene.images.appendChild(myMovie);
        myMovie.src = theSource;
        myMovie.name = theSource;
        if (theDecoderHint) {
            myMovie.decoderhint = theDecoderHint;
        }
        window.scene.loadMovieFrame(myMovie);
        return myMovie;
    }
}

function blurImage(theImageNode, theRadius, theSigma) {
    var mySize = getImageSize(theImageNode);
    if( theSigma == null ) {
        theSigma = 1.0;
    }
    applyImageFilter(theImageNode, "gaussianblur", [theRadius, mySize.x, mySize.y, theSigma]);
}

function getImageSize(theImage) {
    if (theImage.src=="" && theImage.childNodes.length == 0) {
        print("### ERROR: src attribute must be set, before getting size for image: "+ theImage.id);
    }
    var mySize = new Vector3f(theImage.width, theImage.height, 0);
    var myImageMatrix = new Matrix4f(theImage.matrix);
    myImageMatrix.setRow(3, [0,0,0,1]);
    mySize = product(mySize, myImageMatrix);
    return new Vector2i(Math.abs(Math.round(mySize.x)), Math.abs(Math.round(mySize.y)));
}
