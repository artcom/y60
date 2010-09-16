/*jslint nomen: false plusplus:false*/
/*globals spark, window, print, product, Vector3f*/

spark.AtlasImage = spark.ComponentClass("AtlasImage");

spark.AtlasImage.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.ResizableRectangle);

    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myAtlasPath        = null;
    var _mySubTextureName   = null;
    var _myOriginalUVCoords = [];
    
    /////////////////////
    // Private Methods //
    /////////////////////
    
    function _applyAtlasTextureInformation() {
        if (_myAtlasPath && _mySubTextureName) {
            var myUVCoords = Protected.shape.find(".//*[@name='uvset']").firstChild.nodeValue;
            for (var i = 0; i < myUVCoords.length; i++) {
                myUVCoords[i] = product(_myOriginalUVCoords[i].xy0,
                                        Public.root.textureAtlasManager.getUVMatrix(_mySubTextureName, _myAtlasPath)).xy;
            }
        }
    }
    
    function _getMaterial(theTextureName, theAtlasPath) {
        return Public.root.textureAtlasManager.getMaterial(_myAtlasPath);
    }
    
    function _storeOriginalUVCoords() {
        var myOriginalUVCoords = Protected.shape.find(".//*[@name='uvset']").firstChild.nodeValue;
        for (var i = 0; i < myOriginalUVCoords.length; i++) {
            _myOriginalUVCoords.push(myOriginalUVCoords[i].clone());
        }
    }
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    Base.realize = Public.realize;
    Public.realize = function (theMaterial) {
        _myAtlasPath = Protected.getString("atlas");
        _mySubTextureName = Protected.getString("subtexture");
        
        Base.realize(_getMaterial(_mySubTextureName, _myAtlasPath));
        
        _storeOriginalUVCoords();
        _applyAtlasTextureInformation();
        
        var myTexturePixelSize = Public.root.textureAtlasManager.getSize(_mySubTextureName, _myAtlasPath);
        Public.width  = Protected.getNumber("width",  myTexturePixelSize[0]);
        Public.height = Protected.getNumber("height", myTexturePixelSize[1]);
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
    };
    
    Public.setTexture = function (theTextureName, theAtlasPath) {
        _myAtlasPath = theAtlasPath;
        _mySubTextureName = theTextureName;
        _applyAtlasTextureInformation();
        Public.size = Public.root.textureAtlasManager.getSize(_mySubTextureName, _myAtlasPath);
    };
};