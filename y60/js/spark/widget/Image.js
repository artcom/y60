/**
 * A simple Quad-image.
 *
 * NOTE: does not adjust it's size when image is changed. However, the size
 *       can be changed by setting the width, height and size properties.
 */
spark.Image = spark.ComponentClass("Image");

spark.Image.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.ResizableRectangle);

    var _mySource = null;
    var _mySourceId = null;
    var _mySourceItem = null;

    var _myImage = null;
    var _myImageOwned = false;

    var _myTexture  = null;
    var _myVertices = null;

    // XXX crude hack starts here
    var _myOnImageChanged = null;
    // XXX crude hack ends here

    Public.image getter = function() {
        return _myImage;
    };

    Public.image setter = function(theNode) {
        if(_myImageOwned) {
            _myImage.parentNode.removeChild(_myImage);
            _myImageOwned = false;
            _myImage = null;
        }
        _myImage = theNode;
        _myTexture.image = theNode.id;
        Public.width  = Protected.getNumber("width", _myImage.raster.width);
        Public.height = Protected.getNumber("height", _myImage.raster.height);

        // XXX crude hack starts here
        if(_myOnImageChanged) {
            _myOnImageChanged();
        }
        // XXX crude hack ends here
    };

    Public.src getter = function() {
        return _mySource;
    };

    Public.src setter = function(theSourceFile) {
        Public.image = spark.getCachedImage(theSourceFile);
        _mySource = theSourceFile;
    };

    Public.srcId getter = function() {
        return _mySourceId;
    };

    Public.srcId setter = function(theValue) {
        _mySourceId = theValue;
        attachToI18nItem(theValue);
    };
    Public.i18nItem getter = function() { return Public.srcId; }
    Public.i18nItem setter = function(i) { Public.srcId = i; }


    // XXX: this should not exist.
    Public.texture getter = function() {
        return _myTexture;
    };

    // XXX: this should not exist.
    //    Public.textureId setter = function(theTextureId) {
    //        _myMaterial.childNode("textureunits").firstChild.texture = theTextureId;
    //    };

    Base.realize = Public.realize;
    Public.realize = function(theCachedMaterial) {
        var myImageSource = Protected.getString("src", "");
        var myImageSourceId = Protected.getString("srcId", "");

        var myWidth = 0;
        var myHeight = 0;
        if(myImageSource == "") {
            myWidth = Protected.getNumber("width", 1);
            myHeight = Protected.getNumber("height", 1);
            _myImage      = Modelling.createImage(window.scene, myWidth, myHeight, "BGRA");
            _myImageOwned = true;
            if(myImageSourceId != "") {
                _mySourceId = myImageSourceId;
            }
        } else {
            _myImage = spark.getCachedImage(myImageSource);
            _mySource = myImageSource;
            myWidth = Protected.getNumber("width", _myImage.raster.width);
            myHeight = Protected.getNumber("height", _myImage.raster.height);
        }

        _myTexture  = Modelling.createTexture(window.scene, _myImage);
        _myTexture.name = Public.name + "-texture";
        _myTexture.wrapmode = "clamp_to_edge";
        
        var myMaterial = theCachedMaterial;
        if (myMaterial == undefined) {
            myMaterial = Modelling.createUnlitTexturedMaterial(window.scene,
                    _myTexture, Public.name + "-material", true);
        }

        Base.realize(myMaterial);

        Public.width = myWidth;
        Public.height = myHeight;
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        if(_mySourceId) {
            attachToI18nItem(_mySourceId);
        }
        Base.postRealize();
    };

    function handleI18nLanguage(e) {
        Public.src = e.src;
    }

    function attachToI18nItem(theItemId) {
        if(_mySourceItem) {
            _mySourceItem.removeEventListener(spark.I18nEvent.LANGUAGE,
                                              handleI18nLanguage);
            _mySourceItem = null;
        }
        _mySourceItem = Public.getI18nItemByName(theItemId);
        if(!_mySourceItem) {
            Logger.fatal("no i18n item named " + theItemId);
        }
        _mySourceItem.addEventListener(spark.I18nEvent.LANGUAGE,
                                       handleI18nLanguage);
        Public.src = _mySourceItem.src;
    };

    // XXX crude hack starts here

    Public.onImageChanged getter = function() {
        return _myOnImageChanged;
    };
    Public.onImageChanged setter = function(f) {
        _myOnImageChanged = f;
    };

    // XXX crude hack ends here
};
