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
    
    //theMaterial and the Scene are optional, theMaterial should have one textureunit
    Public.loadAtlas = function (theAtlasDefinitionFile, theMaterial, theScene) {
        var myNewTextureAtlas, myAtlasInfo;
        if (!(theAtlasDefinitionFile in _atlases)) {
            myNewTextureAtlas = new TextureAtlas(theAtlasDefinitionFile);
            _atlases[theAtlasDefinitionFile] = {};
            myAtlasInfo = _atlases[theAtlasDefinitionFile];
            myAtlasInfo.atlas = myNewTextureAtlas;
            var myScene = (theScene?theScene:window.scene);
            if (theMaterial) {
                myAtlasInfo.material = theMaterial;
                var myImg = Modelling.createImage(myScene, myAtlasInfo.atlas.imagePath); 
                var myTexture = Modelling.createTexture(myScene, myImg);
                var myTextureUnit;
                if (theMaterial.childNode("textureunits").childNodesLength() > 0) {
                    myTextureUnit = myAtlasInfo.material.childNode("textureunits").childNodes[0];
                    myTextureUnit.texture = myTexture.id;
                } else {
                    Logger.warning("material should have one texture unit");
                }
            } else {
                myAtlasInfo.material = Modelling.
                                       createUnlitTexturedMaterial(myScene,
                                                                   myAtlasInfo.atlas.imagePath,
                                                                   theAtlasDefinitionFile + "_atlasMaterial",
                                                                   true);
            }
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
