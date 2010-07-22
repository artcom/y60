/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2009, ART+COM AG Berlin, Germany <www.artcom.de>
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
// Description: Image and texture caching infrastructure
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

spark.dummyImage = null;

spark.getDummyImage = function() {
    if(spark.dummyImage == null) {
        spark.dummyImage = Modelling.createImage(window.scene, 16, 16, "BGRA");
        spark.dummyImage.name = "spark-dummy-image";
    }
    return spark.dummyImage;
};

spark.cachedImages = {};

spark.getCachedImage = function(thePath) {
    var myName = "spark-cached-image-" + thePath;
    var myImage = spark.getNode(myName);

    if(!myImage) {
        myImage = Modelling.createImage(window.scene, thePath);
        myImage.name = myName;

        spark.registerNode(myName, myImage);
    }

    return myImage;
};

spark.getCachedTexture = function(thePath) {
    var myName = "spark-cached-texture-" + thePath;
    var myTexture = spark.getNode(myName);

    if(!myTexture) {
        var myImage = spark.getCachedImage(thePath);

        myTexture = Modelling.createTexture(window.scene, myImage);
        myTexture.name = myName;

        spark.registerNode(myName, myTexture);
    }

    return myTexture;
};

spark.ourMovieCounter = 0;

spark.openMovie = function(thePath, theTargetPixelFormat, theDecoderHint) {
    var myMovie = Node.createElement("movie");

    window.scene.images.appendChild(myMovie);

    myMovie.src = thePath;
    myMovie.name = "spark-movie-" + spark.ourMovieCounter++;
    myMovie.resize = "none";
    myMovie.loopcount = "1";
    myMovie.targetpixelformat = (theTargetPixelFormat) ? theTargetPixelFormat :"RGB";
    myMovie.decoderhint = (theDecoderHint) ? theDecoderHint : "FFMpegDecoder2";

    //always load first frame
    myMovie.playmode = "pause";
    window.scene.loadMovieFrame(myMovie);
    myMovie.playmode = "stop";

    return myMovie;
};
