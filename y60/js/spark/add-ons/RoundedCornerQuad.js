//=============================================================================
// Copyright (C) 2009, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

/*jslint nomen:false plusplus:false*/
/*globals spark, use, addMaterialRequirement*/

use("BuildUtils.js");

spark.RoundedCornerQuad = spark.ComponentClass("RoundedCornerQuad");
spark.RoundedCornerQuad.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    
    /*                       (1,0)
        c2 ---------------- c3
        |                   |
        |                   |
        c0 ---------------- c1
       (0,1)
    */
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    Public.__defineSetter__("cornerDistance", function (theDistance) {
        Protected.material.properties.cornerDistance = theDistance / Public.width;
    });
    
    Public.__defineGetter__("radius", function () {
        return Protected.material.properties.radius * Public.width;
    });
    Public.__defineSetter__("radius", function (theRadius) {
        Protected.material.properties.radius = theRadius / Public.width;
    });
    
    Public.__defineGetter__("cornerToggle", function () {
        return Protected.material.properties.cornerToggle;
    });
    Public.__defineSetter__("cornerToggle", function (theVector) {
        Protected.material.properties.cornerToggle = theVector;
    });
    
    Public.__defineSetter__("cropRightCorners", function (theFlag) {
        if (theFlag) {
            Protected.material.properties.cornerToggle = [1, 1, 1, 1];
        } else {
            Protected.material.properties.cornerToggle = [1, 0, 1, 0];
        }
    });
    
    Public.realize = function () {
        Protected.material.transparent = true;
        addMaterialRequirement(Protected.material, "vertexparams", "[10[color]]");
    };

    Public.postRealize = function () {
        Protected.material.enabled = false;
        var myOptionNode = Protected.material.requires.getNodesByTagName("feature");
        var myIsYUVMovie = false;
        for (var i = 0; i < myOptionNode.length; ++i) {
            if (myOptionNode[i].name === "option") {
                if (myOptionNode[i].childNode("#text").nodeValue === "[10[yuv2rgb]]") {
                    Protected.material.requires.removeChild(myOptionNode[i]);
                    myIsYUVMovie = true;
                }
            }
        }
        if (!myIsYUVMovie) {
            addMaterialRequirement(Protected.material, "option", "[10[RoundedCorner]]");
        } else {
            addMaterialRequirement(Protected.material, "option", "[10[yuv2rgb_round]]");
        }
        Protected.material.enabled = true;
        Public.update();
    }
    
    Public.updateAspectRatio = function() {
        Protected.material.properties.aspectRatio = Public.width/Public.height;
    }
    
    Public.update = function() {
        Public.updateAspectRatio();
        Public.radius = Protected.getNumber("radius", 17); 
        Public.cornerDistance = Protected.getNumber("cornerDistance", 17); 
        Public.cornerToggle = Protected.getVector4f("cornerToggle", [1,1,1,1]);
    };
    
};
