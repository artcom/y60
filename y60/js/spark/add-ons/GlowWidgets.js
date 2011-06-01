//=============================================================================
// Copyright (C) 2011, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

spark.GlowPlug = spark.AbstractClass("GlowPlug");
spark.GlowPlug.Constructor = function (Protected) {
    var Base = {};
    var Public = this;

    var _myAnythingGlowsOrBlursFlag = false;
    var _myMaterial = null;

    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        _myMaterial = Protected.material;
        _myMaterial.enabled = false;
        var myGlow = Protected.getNumber("glow", 0);
        var myBlur = Protected.getNumber("blur", 0);
        var myScale = Protected.getNumber("glow_scale", 1);
        var myGlowIntensity = Protected.getNumber("glow_intensity", 1);
        _myAnythingGlowsOrBlursFlag = true;
        if (myBlur >0 && myGlow > 0) {
            addMaterialRequirement(_myMaterial, "effect", "[10[glowBlur]]");
            addMaterialProperty(_myMaterial, "float", "blur",myBlur);
        } else if (myGlow >0) {
            addMaterialRequirement(_myMaterial, "effect", "[10[glow]]");
            addMaterialProperty(_myMaterial, "float", "glow", "1");
        } else  if (myBlur >0) {
            addMaterialRequirement(_myMaterial, "effect", "[10[blur]]");
            addMaterialProperty(_myMaterial, "float", "blur",myBlur);
        } else {
            _myAnythingGlowsOrBlursFlag = false;
        }

        addMaterialProperty(_myMaterial, "float", "scale", myScale);
        addMaterialProperty(_myMaterial, "float", "glow_intensity", myGlowIntensity);
        if (_myAnythingGlowsOrBlursFlag) {
            if (myGlow > 0){
                _myMaterial.enabled = true;
                _myMaterial.properties.offset = Protected.getVector2f("offset", "[0,0]");
                _myMaterial.properties.glow_radius = myGlow;
                _myMaterial.properties.glow_color = Protected.getVector4f("glow_color", "[1,1,1,1]");
                _myMaterial.properties.scale = myScale;
            }
            if (myBlur >0) {
                _myMaterial.properties.blur = Protected.getNumber("blur", 5);
            }
        }
    };

    Protected.GlowOrBlurFlag getter = function () {
        return _myAnythingGlowsOrBlursFlag;
    };
    Protected.GlowOrBlurFlag setter = function (theFlag) {
        _myAnythingGlowsOrBlursFlag = theFlag;
    };

    Public.glow setter = function (theGlow) {
        _myMaterial.properties.glow_radius = theGlow;
    };

    Public.glowAlpha setter = function (theAlpha) {
        _myMaterial.properties.glow_color[3] = theAlpha;
    };
    Public.glowIntensity setter = function (theIntensity) {
        _myMaterial.properties.glow_intensity = theIntensity;
    };
    Public.glowIntensity getter = function () {
        return _myMaterial.properties.glow_intensity;
    };

};

spark.GlowText = spark.ComponentClass("GlowText");
spark.GlowText.Constructor = function (Protected) {
    var Base = {};
    var Public = this;

    Public.Inherit(spark.Text);
    Public.Inherit(spark.GlowPlug);

    /////////////////////
    // Protected Methods //
    /////////////////////
    Base.render = Protected.render;
    Protected.render = function () {
        Base.render();
        if (Protected.GlowOrBlurFlag && Public.width > 0 && Public.height >0) {
            Protected.material.properties.width = Public.width;
            Protected.material.properties.height = Public.height;
        }
    };

    /////////////////////
    // Public Methods //
    /////////////////////
    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
    };

};

spark.GlowImage = spark.ComponentClass("GlowImage");
spark.GlowImage.Constructor = function (Protected) {
    var Base = {};
    var Public = this;

    Public.Inherit(spark.Image);
    Public.Inherit(spark.GlowPlug);

    /////////////////////
    // Public Methods //
    /////////////////////
    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        Public.SetterOverride("width", function (theWidth, theBaseSetter) {
            theBaseSetter(theWidth);
            if (Protected.GlowOrBlurFlag) {
                Protected.material.properties.width = theWidth;
            }
        });
        Public.SetterOverride("height", function (theHeight, theBaseSetter) {
            theBaseSetter(theHeight);
            if (Protected.GlowOrBlurFlag) {
                Protected.material.properties.height = theHeight;
            }
        });
        Public.size = Public.size;
    };
};

spark.GlowMovie = spark.ComponentClass("GlowMovie");
spark.GlowMovie.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    
    Public.Inherit(spark.Movie);
    Public.Inherit(spark.GlowPlug);
    
    /////////////////////
    // Public Methods //
    /////////////////////    
    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();     
        Protected.material.properties.width = Public.width;
        Protected.material.properties.height = Public.height;        
    }
};



