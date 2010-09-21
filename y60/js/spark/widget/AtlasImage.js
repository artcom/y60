/*jslint nomen: false plusplus:false*/
/*globals spark, window, print, product, Vector3f, Exception*/

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
    var _realized = false;
    var _myUVTransformationMatrix = null;
    var _myOriginalImageSize = null;
    
    /////////////////////
    // Private Methods //
    /////////////////////
    
    function _applyAtlasTextureInformation() {
        if (_myAtlasPath && _mySubTextureName) {
            var myUVCoords = Protected.shape.find(".//*[@name='uvset']").firstChild.nodeValue;
            for (var i = 0; i < myUVCoords.length; i++) {
                myUVCoords[i] = product(_myOriginalUVCoords[i].xy0,
                                        _myUVTransformationMatrix).xy;
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
        _myAtlasPath      = Protected.getString("atlas", _myAtlasPath);
        _mySubTextureName = Protected.getString("subtexture", _mySubTextureName);
        _myUVTransformationMatrix = Public.root.textureAtlasManager.getUVMatrix(_mySubTextureName, _myAtlasPath);
        
        if (_mySubTextureName && _myAtlasPath) {
            Base.realize(_getMaterial(_mySubTextureName, _myAtlasPath));
        } else {
            throw new Exception("AtlasImage cannot be realized without having set a texture and atlas first via 'setTexture()'");
        }
        _storeOriginalUVCoords();
        _applyAtlasTextureInformation();
        
        _myOriginalImageSize = Public.root.textureAtlasManager.getSize(_mySubTextureName, _myAtlasPath);
        Public.width  = Protected.getNumber("width",  _myOriginalImageSize[0]);
        Public.height = Protected.getNumber("height", _myOriginalImageSize[1]);
        
        _realized = true;
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
    };
    
    Public.setTexture = function (theTextureName, theAtlasPath) {
        _myAtlasPath = theAtlasPath;
        _mySubTextureName = theTextureName;
        if (_realized) {
            _applyAtlasTextureInformation();
            Protected.material = _getMaterial(theTextureName, theAtlasPath);
            Protected.material.properties.blendfunction = "[src_alpha,one_minus_src_alpha,one,one_minus_src_alpha]";
            Public.size = Public.root.textureAtlasManager.getSize(_mySubTextureName, _myAtlasPath);
        }
    };
    
    Protected.__defineGetter__("uvTransformationMatrix", function() {
        return _myUVTransformationMatrix;
    });
    
    Protected.__defineGetter__("originalImageSize", function() {
        return _myOriginalImageSize;
    });
};
