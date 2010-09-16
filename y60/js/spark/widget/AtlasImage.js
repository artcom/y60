/*jslint nomen: false*/
/*globals spark, window*/

spark.AtlasImage = spark.ComponentClass("AtlasImage");

spark.AtlasImage.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.ResizableRectangle);

    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myAtlasPath   = null;
    var _myTextureName = null;
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    Base.realize = Public.realize;
    Public.realize = function (theMaterial) {
        _myAtlasPath = Protected.getString("atlas");
        _myTextureName = Protected.getString("name");
        
        /*
        print(Public.root.textureAtlasManager.getUVTranslation(_myTextureName, _myAtlasPath));
        print(Public.root.textureAtlasManager.getMaterial(_myAtlasPath));
        */
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
    };
};