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

/*jslint nomen:false white:false*/
/*globals window, Node, Modelling*/

function ImageManager(theBaseViewer) {
    this.Constructor(this, theBaseViewer);
}

ImageManager.prototype.Constructor = function(obj, theBaseViewer) {

    // private members
    //var _myBaseViewer = theBaseViewer; // unused

    // public methods
    obj.getImageNode = function(theImageName, theEncoding) {
        var myImageNode = window.scene.images.find("image[@name = '" + theImageName + "']");
        if (!myImageNode) {
            var myEncoding = (theEncoding !== undefined) ? theEncoding.toUpperCase() : "RGB";
            myImageNode = Modelling.createImage(window.scene, 1, 1, myEncoding);
            myImageNode.name = theImageName;
        }
        return myImageNode;
    };

    obj.createImage = function(theSource, theEncoding) {
        var myImageNode = Node.createElement("image");
        window.scene.images.appendChild(myImageNode);
        myImageNode.name = theSource;
        myImageNode.src = theSource;
        myImageNode.resize = "pad";
        return myImageNode;
    };

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
    };
};