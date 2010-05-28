/**
 * A simple movie player.
 *
 * NOTE: does not adjust it's size when image is changed. However, the size
 *       can be changed by setting the width, height and size properties.
 */
spark.NewMovie = spark.ComponentClass("NewMovie");

spark.NewMovie.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    this.Inherit(spark.ResizableRectangle);

    var _mySource = null;
    var _mySourceId = null;
    var _mySourceItem = null;

    var _myMovie = null;
    var _myMovieOwned = false;

    var _myTexture  = null;
    var _myVertices = null;

    // playback control

    Public.play = function() {
    	if(_myMovie.nodeName == "movie") {
    		_myMovie.playmode = "play";
    	}
    };

    Public.stop = function() {
    	if(_myMovie.nodeName == "movie") {
    		_myMovie.playmode = "stop";
    	}
    };

    Public.pause = function() {
    	if(_myMovie.nodeName == "movie") {
    		_myMovie.playmode = "pause";
    	}
    };

    Public.mode getter = function() { return _myMovie.playmode; };
    Public.mode setter = function(thePlaymode) { _myMovie.playmode = thePlaymode };

    Public.loop getter = function() {
        return _myMovie.loopcount == 0;
    };

    Public.loop setter = function(theFlag) {
        _myMovie.loopcount = theFlag ? 0 : 1;
    };

    // frames

    Public.frameCount getter = function() { return _myMovie.framecount; };

    Public.currentFrame getter = function() { return _myMovie.currentframe; };
    Public.currentFrame setter = function(f) {
        _myMovie.currentframe = f;
    };

    // audio

    Public.audio getter = function() { return _myMovie.audio == 1; };
    Public.audio setter = function(theFlag) {
        _myMovie.audio = theFlag ? 1 : 0;
    };

    Public.volume getter = function() { return _myMovie.volume; };
    Public.volume setter = function(theVolume) {
        _myMovie.volume = theVolume;
    };

    Public.hasAudio getter = function() { return _myMovie.has_audio;};


    // XXX crude hack starts here
    var _myOnMovieChanged = null;
    // XXX crude hack ends here

    Public.movie getter = function() {
        return _myMovie;
    };

    Public.movie setter = function(theNode) {
        if(_myMovie) {
            if(_myMovie.playmode != "stop") {
                Public.stop();
            }
            _myMovie.parentNode.removeChild(_myMovie);
            _myMovie = null;
        }

        _myMovie = theNode;
        _myTexture.image = theNode.id;
        var myHeight = _myMovie.height;
        var myWidth = Math.round(_myMovie.height * _myMovie.aspectratio);
        if (myWidth > _myMovie.width) {
            myWidth = _myMovie.width;
            myHeight = Math.round(myWidth / _myMovie.aspectratio);
        }
        Public.width  = myWidth;
        Public.height = myHeight;

        // XXX crude hack starts here
        if(_myOnMovieChanged) {
            _myOnMovieChanged();
        }
        // XXX crude hack ends here
    };

    Public.src getter = function() {
        return _mySource;
    };

    Public.src setter = function(theSourceFile) {
        if(_mySource != theSourceFile) {
            Public.movie = spark.openMovie(theSourceFile);
            _mySource = theSourceFile;
        } else {
            if(_myMovie.playmode != "stop") {
                Public.stop();
            }
        }
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
    Public.realize = function() {
        var myMovieSource = Protected.getString("src", "");
        var myMovieSourceId = Protected.getString("srcId", "");

        if(myMovieSource == "") {
            var myWidth  = Protected.getNumber("width", 1);
            var myHeight = Protected.getNumber("height", 1);
            _myMovie      = Modelling.createImage(window.scene, myWidth, myHeight, "BGR");
            if(myMovieSourceId != "") {
                _mySourceId = myMovieSourceId;
            }
        } else {
            _myMovie = spark.openMovie(myMovieSource);
            _mySource = myMovieSource;
        }

        _myTexture  = Modelling.createTexture(window.scene, _myMovie);
        _myTexture.name = Public.name + "-texture";
        _myTexture.wrapmode = "clamp_to_edge";

        var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene,
                _myTexture, Public.name + "-material", true);

        Base.realize(myMaterial);
        if(myMovieSource) {
            Public.src = myMovieSource;
        }
        if(_myMovie.nodeName != "image") {
            Public.loop = Protected.getBoolean("loop", false);
            Public.mode = Protected.getString("mode", "stop");
            Public.audio = Protected.getBoolean("audio", true);
            Public.volume = Protected.getNumber("volume", 1.0);
        }
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

    Public.onMovieChanged getter = function() {
        return _myOnMovieChanged;
    };
    Public.onMovieChanged setter = function(f) {
        _myOnMovieChanged = f;
    };

    // XXX crude hack ends here
};
