//=============================================================================
// Copyright (C) 2009, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

/*jslint nomen:false, plusplus:false*/
/*globals use spark, window, Modelling, Node, addMaterialRequirement,
          TextureApplyMode*/

use("BuildUtils.js");

spark.YUV2RGBShader = spark.ComponentClass("YUV2RGBShader");
spark.YUV2RGBShader.Constructor = function (Protected) {
    var Base   = {};
    var Public = this;
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myTextures = [];
    
    /////////////////////
    // Private Methods //
    /////////////////////
    
    function feature_n_times(theWeight, theToken, n) {
        var f = "[" + theWeight + "[";
        for (var i = 0; i < n ; ++i) {
            f += (i ? "," : "") + theToken;
        }
        f += "]]";
        return f;
    }

    function ensureShader() {
        var myRequirement = Protected.material.find(".//*[@name='option']");
        if (myRequirement && myRequirement.childNode("#text").nodeValue.toString() === "[10[yuv2rgb]]") {
            return;
        }
        // YUV targetrasterformat allows us to use a shader to convert YUV2RGB, 
        // loadMovieFrame created 3 rasters for us, therefore we need 3 textures
        Protected.material.enabled = false;
        for (var i = 1; i < Public.movie.childNodesLength(); i++) {
            var myTextureUnit = Protected.material.childNode("textureunits").appendChild(Node.createElement("textureunit"));
            myTextureUnit.applymode = TextureApplyMode.modulate;
            var myTexture = Modelling.createTexture(window.scene, Public.movie);
            myTexture.image = Public.movie.id;
            myTexture.image_index = i;
            myTextureUnit.texture   = myTexture.id;
            _myTextures.push(myTexture);
        }

        Protected.material.requires.textures = feature_n_times(120, "paint", Public.movie.childNodesLength());

        if (Public.movie.childNodesLength() === 3) {
            addMaterialRequirement(Protected.material, "option", "[10[yuv2rgb]]");
        }
        Protected.material.enabled = true;
    }
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    var specializedClassOnMovieChanged = Protected.onMovieChanged;
    Protected.onMovieChanged = function (theFullInitFlag) {
        specializedClassOnMovieChanged(theFullInitFlag);
        ensureShader();
        for (var i = 1; i < Public.movie.childNodesLength(); i++) {
            _myTextures[i - 1].image = Public.movie.id;
            _myTextures[i - 1].image_index = i;
        }
    };
};

