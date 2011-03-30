/*jslint nomen:false, forin:true*/
/*globals Modelling, window, TextureAtlas*/

function TextureAtlasManager() {
    this.Constructor(this);
}

TextureAtlasManager.prototype.Constructor = function (Public) {
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _atlases = {};
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    /* explicitely load a TextureAtlas*/
    Public.loadAtlas = function (theAtlasDefinitionFile, theTransparencyFlag, theScene) {
        var myNewTextureAtlas, myAtlasInfo;
        if (!(theAtlasDefinitionFile in _atlases)) {
            myNewTextureAtlas = new TextureAtlas(theAtlasDefinitionFile);
            _atlases[theAtlasDefinitionFile] = {};
            myAtlasInfo = _atlases[theAtlasDefinitionFile];
            myAtlasInfo.atlas = myNewTextureAtlas;
            myAtlasInfo.material = Modelling.
                                   createUnlitTexturedMaterial((theScene?theScene:window.scene),
                                                               myAtlasInfo.atlas.imagePath,
                                                               theAtlasDefinitionFile + "_atlasMaterial",
                                                               (theTransparencyFlag!==undefined)?theTransparencyFlag:true);
        }
        return _atlases[theAtlasDefinitionFile];
    };
    
    Public.getUVMatrix = function (theTextureName, theAtlasDefinitionFile) {
        var myAtlasInfo = Public.loadAtlas(theAtlasDefinitionFile);
        return myAtlasInfo.atlas.findTextureTranslation(theTextureName);
    };
    
    Public.getSize = function (theTextureName, theAtlasDefinitionFile) {
        var myAtlasInfo = Public.loadAtlas(theAtlasDefinitionFile);
        return myAtlasInfo.atlas.findTextureSize(theTextureName);
    };
    
    Public.getMaterial = function (theAtlasDefinitionFile) {
        var myAtlasInfo = Public.loadAtlas(theAtlasDefinitionFile);
        return myAtlasInfo.material;
    };
    
    //assumption: unique ids across atlases
    Public.getMaterialByTextureName = function (theTextureName) {
        for (var myDefinitionFile in _atlases) {
            if (_atlases[myDefinitionFile].atlas.containsTexture(theTextureName)) {
                return _atlases[myDefinitionFile].material;
            }
        }
        return null;
    };

    //assumption: unique ids across atlases
    Public.getUVMatrixByTextureName = function (theTextureName) {
        for (var myDefinitionFile in _atlases) {
            if (_atlases[myDefinitionFile].atlas.containsTexture(theTextureName)) {
                return _atlases[myDefinitionFile].atlas.findTextureTranslation(theTextureName);
            }
        }
        return null;
    };



    // XXX maybe unloadAtlas?
};
