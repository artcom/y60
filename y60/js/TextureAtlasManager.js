/*jslint nomen:false*/
/*globals Modelling, window, TextureAtlas*/

function TextureAtlasManager() {
    this.Constructor(this);
}

TextureAtlasManager.prototype.Constructor = function (Public) {
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _atlasses = {};
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    /* explicitely load a TextureAtlas*/
    Public.loadAtlas = function (theAtlasDefinitionFile) {
        var myNewTextureAtlas, myAtlasInfo;
        if (!(theAtlasDefinitionFile in _atlasses)) {
            myNewTextureAtlas = new TextureAtlas(theAtlasDefinitionFile);
            _atlasses[theAtlasDefinitionFile] = {};
            myAtlasInfo = _atlasses[theAtlasDefinitionFile];
            myAtlasInfo.atlas = myNewTextureAtlas;
            myAtlasInfo.material = Modelling.
                                   createUnlitTexturedMaterial(window.scene,
                                                               myAtlasInfo.atlas.imagePath,
                                                               theAtlasDefinitionFile + "_atlasMaterial",
                                                               true); // transparencyFlag
        }
        return _atlasses[theAtlasDefinitionFile];
    };
    
    Public.getUVMatrix = function (theTextureName, theAtlasDefinitionFile) {
        var myAtlasInfo = Public.loadAtlas(theAtlasDefinitionFile);
        return myAtlasInfo.atlas.findTextureTranslation(theTextureName);
    };
    
    Public.getSize = function (theTextureName, theAtlasDefinitionFile) {
        var myAtlasInfo = Public.loadAtlas(theAtlasDefinitionFile);
        //return myAtlasInfo.atlas.getPixelSize(theTextureName);
        return new Vector2i(288, 50);
    };
    
    Public.getMaterial = function (theAtlasDefinitionFile) {
        var myAtlasInfo = Public.loadAtlas(theAtlasDefinitionFile);
        return myAtlasInfo.material;
    };
    
    // XXX maybe unloadAtlas?
};