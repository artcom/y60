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
    // XXX would even be nice to have this in the components.
    // e.g. "bool getter 'realized'"
    
    /////////////////////
    // Private Methods //
    /////////////////////
    
    function _getMaterial() {
        return Public.root.textureAtlasManager.getMaterial(_myAtlasPath);
    }
    
    ///////////////////////
    // Protected Methods //
    ///////////////////////
    
    Protected.applyAtlasTextureInformation = function () {
        if (_myAtlasPath && _mySubTextureName) {
            var myUVCoords = Protected.shape.find(".//*[@name='uvset']").firstChild.nodeValue;
            for (var i = 0; i < myUVCoords.length; i++) {
                myUVCoords[i] = product(_myOriginalUVCoords[i].xy0,
                                        Public.root.textureAtlasManager.getUVMatrix(_mySubTextureName,
                                                                                    _myAtlasPath)).xy;
            }
        }
    };
    
    Protected.storeOriginalUVCoords = function () {
        var myOriginalUVCoords = Protected.shape.find(".//*[@name='uvset']").firstChild.nodeValue;
        _myOriginalUVCoords = [];
        for (var i = 0; i < myOriginalUVCoords.length; i++) {
            _myOriginalUVCoords.push(myOriginalUVCoords[i].clone());
        }
    };
    
    Protected.__defineGetter__("uvTransformationMatrix", function () {
        return Public.root.textureAtlasManager.getUVMatrix(_mySubTextureName,
                                                           _myAtlasPath);
    });
    
    Protected.__defineGetter__("originalImageSize", function () {
        return Public.root.textureAtlasManager.getSize(_mySubTextureName,
                                                       _myAtlasPath);
    });
    
    Protected.__defineGetter__("atlas", function () {
        return Public.root.textureAtlasManager.getAtlas(_myAtlasPath);
    });

    ////////////////////
    // Public Methods //
    ////////////////////
    
    Public.__defineSetter__("src", function (theTextureName) {
        Public.setTexture(theTextureName, _myAtlasPath);
    });

    Base.realize = Public.realize;
    Public.realize = function () {
        _myAtlasPath      = Protected.getString("atlas", _myAtlasPath);
        _mySubTextureName = Protected.getString("subtexture", _mySubTextureName);
        
        if (_myAtlasPath) {
            Base.realize(_getMaterial());
        } else {
            throw new Exception("AtlasImage \"" + Public.name + "\" cannot be realized without having set a texture and atlas first", fileline());
        }
        Protected.storeOriginalUVCoords();
        _realized = true;

        if (_mySubTextureName) {
            Public.setTexture(_mySubTextureName, _myAtlasPath);
        }
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
    };
    
    Public.setTexture = function (theTextureName, theAtlasPath) {
        _myAtlasPath      = theAtlasPath;
        _mySubTextureName = theTextureName;
        if (_realized) {
            Protected.applyAtlasTextureInformation();
            Protected.material = _getMaterial();
            Protected.material.properties.blendfunction = "[src_alpha,one_minus_src_alpha,one,one_minus_src_alpha]";
            Public.width  = Protected.getNumber("width",
                                                Public.root.textureAtlasManager.getSize(_mySubTextureName,
                                                                                        _myAtlasPath)[0]);
            Public.height = Protected.getNumber("height",
                                                Public.root.textureAtlasManager.getSize(_mySubTextureName,
                                                                                        _myAtlasPath)[1]);
        }
    };
};
