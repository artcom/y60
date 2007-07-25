
function MaterialQuadBody(theScene, theParentNode, theImage, theCoords) {
    this.Constructor(this, theScene, theParentNode, theImage, theCoords);
}

var ourMaterialsHash = {};

MaterialQuadBody.prototype.Constructor = function(Public, theScene, theParentNode, theImage, theCoords) {

    var _myMaterial;

    if (!theParentNode || !("nodeName" in theParentNode)) {
	var errorMessage = "need a DOM node to create a MaterialQuadBody!";
	Logger.error(errorMessage);
	throw new Exception(errorMessage);
    }

    if (theImage == undefined) {
        var errorstring = "theImage is undefined";
        Logger.error(errorstring);
        throw new Exception(errorstring);
    } else if (fileExists(theImage)) {
        if (theImage in ourMaterialsHash) {
            _myMaterial = Modelling.createUnlitTexturedMaterial(theScene, ourMaterialsHash[theImage]);
            _myMaterial.properties.surfacecolor = new Vector4f(1, 1, 1, 1);
        } else {
            _myMaterial = Modelling.createUnlitTexturedMaterial(theScene, theImage, "hallo", true);
            _myMaterial.properties.surfacecolor = new Vector4f(1, 1, 1, 1);
            ourMaterialsHash[theImage] = _myMaterial.childNode("textures", 0).firstChild.image;
        }
    } else if (("id" in theImage) && theParentNode.getElementById(theImage.id)) {
        _myMaterial = Modelling.createUnlitTexturedMaterial(theScene, theImage);
        _myMaterial.properties.surfacecolor = new Vector4f(1, 1, 1, 1);
    } else if (theParentNode.getElementById(theImage)) {
        _myMaterial = Modelling.createUnlitTexturedMaterial(theScene, theParentNode.getElementById(theImage));
        _myMaterial.properties.surfacecolor = new Vector4f(1, 1, 1, 1);
    } else {
        _myMaterial = Modelling.createColorMaterial(theScene, theImage);
    }

    var _myQuadShape = Modelling.createQuad(theScene, _myMaterial.id, 
                                            theCoords[0],
                                            theCoords[1]);

    var _myBody = Modelling.createBody(theParentNode, _myQuadShape.id);


    Public.destruct = function() {
        _myBody.parentNode.removeChild(_myBody);
        _myQuadShape.parentNode.removeChild(_myQuadShape);
        _myMaterial.parentNode.removeChild(_myMaterial);
    }

    Public.material getter = function() {
        return _myMaterial;
    }

    Public.shape getter = function() {
        return _myQuadShape;
    }

    Public.quad getter = function() {
        return _myQuadShape;
    }

    Public.body getter = function() {
        return _myBody;
    }
}

