/*jslint nomen:false*/
/*globals*/

function TextureAtlasManager() {
    this.Constructor(this);
}

TextureAtlasManager.prototype.Constructor = function(Public) {
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _atlasses = {}; // key: atlasDefinitionFile
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    /* explicitely load a TextureAtlas*/
    Public.loadAtlas = function(theAtlasDefinitionFile) {
        var myNewTextureAtlas, myAtlasInfo;
        if (!(theAtlasDefinitionFile in _atlasses)) {
            myNewTextureAtlas = new TextureAtlas(theAtlasDefinitionFile);
            _atlasses[theAtlasDefinitionFile] = {};
            myAtlasInfo = _atlasses[theAtlasDefinitionFile];
            
            
            print(myAtlasInfo.atlas.imagePath);
            
            
            myAtlasInfo.atlas = myNewTextureAtlas;
            myAtlasInfo.material = Modelling.
                                   createUnlitTexturedMaterial(window.scene,
                                                               myAtlasInfo.atlas.imagePath,
                                                               theAtlasDefinitionFile + "_atlasMaterial",
                                                               true); // transparencyFlag
        }
        return _atlasses[theAtlasDefinitionFile];
    };
    
    Public.getUVTranslation = function(theTextureName, theAtlasDefinitionFile) {
        // Lazy loading and preparing of material/texture/image in scenegraph
        var myAtlasInfo = Public.loadAtlas(theAtlasDefinitionFile);
        return myAtlasInfo.atlas.findTextureTranslation(theTextureName);
    };
    
    Public.getMaterial = function(theAtlasDefinitionFile) {
        var myAtlasInfo = Public.loadAtlas(theAtlasDefinitionFile);
        return myAtlasInfo.material;
    };
    
    // XXX maybe unloadAtlas?
};