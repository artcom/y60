/**
 * A simple Quad-image.
 *
 * NOTE: does not adjust it's size when image is changed. However, the size
 *       can be changed by setting the width, height and size properties.
 */
 
/*jslint nomen: false*/
/*globals spark, window, Modelling, Logger*/

spark.Image = spark.ComponentClass("Image");

spark.Image.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.ResizableRectangle);

    /////////////////////
    // Private Members //
    /////////////////////

    var _mySource     = null;
    var _mySourceId   = null;
    var _mySourceItem = null;

    var _myImage      = null;
    var _myImageOwned = false;

    var _myTexture    = null;
    var _myUseCaching = true;
    var _myASyncLoad  = false;
    var _myLoadCB     = null;
    
    /////////////////////
    // Private Methods //
    /////////////////////
    
    Protected.handleI18nLanguage = function(e) {
        Public.src = e.src;
    }

    function attachToI18nItem(theItemId) {
        var mySrc = "";
        if(_mySourceItem) {
            _mySourceItem.removeEventListener(spark.I18nEvent.LANGUAGE,
                                              Protected.handleI18nLanguage);
            _mySourceItem = null;
        }
        if (theItemId) {
            _mySourceItem = Public.getI18nItemByName(theItemId);
            if(!_mySourceItem) {
                Logger.fatal("no i18n item named " + theItemId);
            }
            _mySourceItem.addEventListener(spark.I18nEvent.LANGUAGE,
                                           Protected.handleI18nLanguage);
            mySrc = _mySourceItem.src;
        }
        Public.src = mySrc;
    }

    ////////////////////
    // Public Methods //
    ////////////////////

    Public.__defineGetter__("image", function () {
        return _myImage;
    });

    Public.__defineSetter__("image", function (theNode) {
        if (!_myUseCaching || _myImageOwned) {
            _myImage.parentNode.removeChild(_myImage);
            _myImageOwned = false;
            _myImage = null;
        }
        _myImage = theNode;
        if (_myImage) {
            _myTexture.image = theNode.id;
            Public.width  = Protected.getNumber("width", _myImage.raster.width);
            Public.height = Protected.getNumber("height", _myImage.raster.height);
        } else {
            Logger.error("no image node to set");
        }
    });

    Public.__defineGetter__("src", function () {
        return _mySource;
    });

    Public.__defineSetter__("src", function (theSourceFile) {
        _mySource = theSourceFile;
        var myCachedImageFlag = false;
        if (_myUseCaching) {
            var myCacheInfo = spark.getCachedImage(_mySource, _myASyncLoad); 
            Public.image = myCacheInfo.image
            myCachedImageFlag = myCacheInfo.cached; 
        } else {
            if (_mySource === "") {
                Public.image = Modelling.createImage(window.scene, Public.width, Public.height, "BGRA");
                _myImageOwned = true;
            } else {
                Public.image = Modelling.createImage(window.scene, _mySource, _myASyncLoad);
            }
        }
        if (_myASyncLoad && !myCachedImageFlag) {
            bindOnSetCB();        
        } else {
            onLoad();
        }        
    });

    Public.__defineGetter__("srcId", function () {
        return _mySourceId;
    });
    Public.__defineGetter__("source", function () {
        return _mySource;
    });
    Public.__defineSetter__("srcId", function (theValue) {
        _mySourceId = theValue;
        attachToI18nItem(theValue);
    });
    
    Public.__defineGetter__("i18nItem", function () {
        return Public.srcId;
    });
    Public.__defineSetter__("i18nItem", function (i) {
        Public.srcId = i;
    });
    Public.__defineSetter__("onLoadCB", function (i) {
        _myLoadCB = i;
    });
    Public.__defineGetter__("onLoadCB", function () {
        return _myLoadCB;
    });
    Public.__defineGetter__("loadasync", function () {
        return _myASyncLoad;
    });
    Public.__defineSetter__("loadasync", function (i) {
        _myASyncLoad = i;
    });
    // XXX: this should not exist.
    Public.__defineGetter__("texture", function () {
        return _myTexture;
    });

    Public.__defineGetter__("useCaching", function () {
        return _myUseCaching;
    });
    
    Public.__defineSetter__("useCaching", function (theFlag) {
        _myUseCaching = !!theFlag;
    });

    Base.realize = Public.realize;
    Public.realize = function () {
        var myImageSource = Protected.getString("src", "");
        var myImageSourceId = Protected.getString("srcId", "");
        _myASyncLoad = Protected.getBoolean("loadasync", false);
        _myUseCaching = Protected.getBoolean("useCaching", true);

        var myWidth = 0;
        var myHeight = 0;
        var myCachedImageFlag = false;        
        if (myImageSource === "") {
            myWidth = Protected.getNumber("width", 1);
            myHeight = Protected.getNumber("height", 1);
            _myImage      = Modelling.createImage(window.scene, myWidth, myHeight, "BGRA");
            _myImage.name = Public.name + "_ImagedummyImage";
            _myImageOwned = true;
            if (myImageSourceId !== "") {
                _mySourceId = myImageSourceId;
            }
        } else {
            if (_myUseCaching) {
                var myCacheInfo = spark.getCachedImage(myImageSource, _myASyncLoad);                 
                _myImage = myCacheInfo.image;
                myCachedImageFlag = myCacheInfo.cached;                 
            } else {
                _myImage = Modelling.createImage(window.scene, myImageSource, _myASyncLoad);
            }
            _mySource = myImageSource;
            myWidth = Protected.getNumber("width", _myImage.raster.width);
            myHeight = Protected.getNumber("height", _myImage.raster.height);
        }

        _myTexture  = Modelling.createTexture(window.scene, _myImage);
        _myTexture.name = Public.name + "-texture";
        _myTexture.wrapmode = "clamp_to_edge";
        
        var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene,
                _myTexture, Public.name + "-material", true);
        Base.realize(myMaterial);

        Public.width = myWidth;
        Public.height = myHeight;
        if (_myASyncLoad && !myCachedImageFlag) {
            bindOnSetCB();        
        } else {
            onLoad();
        }        
    };
    function bindOnSetCB() {
        registerImageOnLoadCallBack(_myImage, Public, function(theAttribNode) {onLoad();});
    };
    function onLoad() {
        if (_myImage) {                                
            Public.width = Protected.getNumber("width", _myImage.raster.width);
            Public.height = Protected.getNumber("height", _myImage.raster.height);
            _myLoadCB ? _myLoadCB(Public):null
        }
    }
    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        if (_mySourceId) {
            attachToI18nItem(_mySourceId);
        }
        Base.postRealize();
    };

};
var ourImageOnLoadCallBackMap = [];
function registerImageOnLoadCallBack(theImage, theSparkObject, theCallBack) {
    if (!(theImage.id in ourImageOnLoadCallBackMap)) {
        utils.dom.bindOnSetNodeValue(theImage, "loaded", imageOnLoadCallBackDispatcher);
        ourImageOnLoadCallBackMap[theImage.id] = [];
    }
    ourImageOnLoadCallBackMap[theImage.id].push({object: theSparkObject, callback: theCallBack});    
}

function imageOnLoadCallBackDispatcher(theAttribNode) {    
    var myObjects = ourImageOnLoadCallBackMap[theAttribNode.parentNode.id];
    for (var i = 0; i < myObjects.length; i++) {
        myObjects[i].callback(theAttribNode);
    }    
}