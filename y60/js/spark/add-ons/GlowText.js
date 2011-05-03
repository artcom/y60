spark.GlowText = spark.ComponentClass("GlowText");

spark.GlowText.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.Text);

    var _myAnythingGlowsOrBlursFlag = false;
    
    /////////////////////
    // Protected Methods //
    /////////////////////    
    Base.render = Protected.render;
    Protected.render = function () {
        Base.render();
        if (_myAnythingGlowsOrBlursFlag && Public.width > 0 && Public.height >0) {
            Protected.material.properties.width = Public.width;
            Protected.material.properties.height = Public.height;
        }
    }
    
    /////////////////////
    // Public Methods //
    /////////////////////    
    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        
        Protected.material.enabled = false;        
        var myGlow = Protected.getNumber("glow", 0);
        var myBlur = Protected.getNumber("blur", 0);
        var myScale = Protected.getNumber("scale", 1);
                
        _myAnythingGlowsOrBlursFlag = true;
        if (myBlur >0 && myGlow > 0) {
            addMaterialRequirement(Protected.material, "effect", "[10[glowBlur]]");
            addMaterialProperty(Protected.material, "float", "blur",myBlur);     
        } else if (myGlow >0) {
            addMaterialRequirement(Protected.material, "effect", "[10[glow]]");
            addMaterialProperty(Protected.material, "float", "glow", "1");     
        } else  if (myBlur >0) {
            addMaterialRequirement(Protected.material, "effect", "[10[blur]]");
            addMaterialProperty(Protected.material, "float", "blur",myBlur);     
        } else {
            _myAnythingGlowsOrBlursFlag = false;
        }
            
        addMaterialProperty(Protected.material, "float", "scale", myScale);     
        
        Protected.material.enabled = true;        
        if (_myAnythingGlowsOrBlursFlag) {
            if (myGlow > 0){
                Protected.material.properties.offset = Protected.getVector2f("offset", "[0,0]");        
                Protected.material.properties.glow_radius = myGlow;        
                Protected.material.properties.glow_color = Protected.getVector4f("glow_color", "[1,1,1,1)]");
                Protected.material.properties.scale = myScale;
            }    
            if (myBlur >0) {
                Protected.material.properties.blur = Protected.getNumber("blur", 5);        
            }        
        }

    }

};



