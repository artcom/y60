spark.GlowPlug = spark.AbstractClass("GlowPlug");
spark.GlowPlug.Constructor = function (Protected) {
    var Base = {};
    var Public = this;

    var _myAnythingGlowsOrBlursFlag = false;    
    var _myMaterial = null;
    
    Public.realize = function (theMaterial) {
        _myMaterial = theMaterial;
        theMaterial.enabled = false;        
        var myGlow = Protected.getNumber("glow", 0);
        var myBlur = Protected.getNumber("blur", 0);
        var myScale = Protected.getNumber("glow_scale", 1);
        var myGlowIntensity = Protected.getNumber("glow_intensity", 1);
        _myAnythingGlowsOrBlursFlag = true;
        if (myBlur >0 && myGlow > 0) {
            addMaterialRequirement(theMaterial, "effect", "[10[glowBlur]]");
            addMaterialProperty(theMaterial, "float", "blur",myBlur);     
        } else if (myGlow >0) {
            addMaterialRequirement(theMaterial, "effect", "[10[glow]]");
            addMaterialProperty(theMaterial, "float", "glow", "1");     
        } else  if (myBlur >0) {
            addMaterialRequirement(theMaterial, "effect", "[10[blur]]");
            addMaterialProperty(theMaterial, "float", "blur",myBlur);     
        } else {
            _myAnythingGlowsOrBlursFlag = false;
        }
            
        addMaterialProperty(theMaterial, "float", "scale", myScale);     
        addMaterialProperty(theMaterial, "float", "glow_intensity", myGlowIntensity);     
        
        theMaterial.enabled = true;        
        if (_myAnythingGlowsOrBlursFlag) {
            if (myGlow > 0){
                theMaterial.properties.offset = Protected.getVector2f("offset", "[0,0]");        
                theMaterial.properties.glow_radius = myGlow;        
                theMaterial.properties.glow_color = Protected.getVector4f("glow_color", "[1,1,1,1)]");
                theMaterial.properties.scale = myScale;
            }    
            if (myBlur >0) {
                theMaterial.properties.blur = Protected.getNumber("blur", 5);        
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

};

spark.GlowText = spark.ComponentClass("GlowText");
spark.GlowText.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    
    Public.Inherit(spark.Text);
    Base.realizeText = Public.realize;
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
    }
    
    /////////////////////
    // Public Methods //
    /////////////////////    
    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realizeText();
        Base.realize(Protected.material);      
    }

};

spark.GlowImage = spark.ComponentClass("GlowImage");
spark.GlowImage.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    
    Public.Inherit(spark.Image);
    Base.realizeImage = Public.realize;
    Public.Inherit(spark.GlowPlug);
    
    /////////////////////
    // Public Methods //
    /////////////////////    
    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realizeImage();
        Base.realize(Protected.material);     
        Protected.material.properties.width = Public.width;
        Protected.material.properties.height = Public.height;        
    }
};



