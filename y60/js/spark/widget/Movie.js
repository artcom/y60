/**
 * A simple movie player.
 *
 */
spark.Movie = spark.ComponentClass("Movie");

spark.Movie.Constructor = function(Protected) {
    var Base = {};
    var Public = this;

    var _mySource = null;
    var _mySourceId = null;
    var _mySourceItem = null;

    var _myMovie = null;
    var _myTexture  = null;
    var _myDecoderHint  = "FFMpegDecoder2";
    var _myTargetPixelFormat = "RGB";

    this.Inherit(spark.ResizableRectangle);

    Base.initialize = Public.initialize;
    Public.initialize = function(theNode) {
        if (theNode && (("useYuv2RgbShader" in theNode && theNode.useYuv2RgbShader === "true") ||
            ("targetpixelformat" in theNode && theNode.targetpixelformat === "YUV420")))
        {
            theNode.targetpixelformat = "YUV420";
            Base.realizeResizableRectangle = Public.realize;
            use("YUV2RGBShader.js");
            this.Inherit(spark.YUV2RGBShader);
            Base.realizeYUV2RGBShader = Public.realize;
            Public.realize = Base.realizeResizableRectangle;
        }
        Base.initialize(theNode);
    }
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
            if(_myMovie.nodeName == "Movie" && _myMovie.playmode != "stop") {
                Public.stop();
            }
            _myMovie.parentNode.removeChild(_myMovie);
            _myMovie = null;
        }

        _myMovie = theNode;
        _myTexture.image = theNode.id;
        if(_myMovie.nodeName != "image") {
            ensureAspectRatio();
            initMovie();
        }
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
            _mySource = theSourceFile;
            Public.movie = spark.openMovie(theSourceFile, _myTargetPixelFormat, _myDecoderHint);
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

    Base.realize = Public.realize;
    Public.realize = function() {
        var myMovieSource = Protected.getString("src", "");
        var myMovieSourceId = Protected.getString("srcId", "");
        _myDecoderHint = Protected.getString("decoderhint", "FFMpegDecoder2");
        _myTargetPixelFormat = Protected.getString("targetpixelformat", "RGB");

        if(myMovieSource == "") {
            var myWidth  = Protected.getNumber("width", 1);
            var myHeight = Protected.getNumber("height", 1);
            _myMovie      = Modelling.createImage(window.scene, myWidth, myHeight, "BGR");
            _myMovie.name = Public.name + "_movieDummyImage";
            if(myMovieSourceId != "") {
                _mySourceId = myMovieSourceId;
            }
        } else {
            _myMovie = spark.openMovie(myMovieSource, _myTargetPixelFormat, _myDecoderHint);
            _mySource = myMovieSource;
        }

        _myTexture  = Modelling.createTexture(window.scene, _myMovie);
        _myTexture.name = Public.name + "-texture";
        _myTexture.wrapmode = "clamp_to_edge";

        var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene,
                _myTexture, Public.name + "-material", true);

        Base.realize(myMaterial);
        if(myMovieSource) {
            ensureAspectRatio();
            initMovie();
        }
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        if(_mySourceId) {
            attachToI18nItem(_mySourceId);
        }
        if ("realizeYUV2RGBShader" in Base && Base.realizeYUV2RGBShader) {
            Base.realizeYUV2RGBShader();
        }
        Base.postRealize();
    };

    function ensureAspectRatio () {
        var myHeight = _myMovie.height;
        var myWidth = Math.round(_myMovie.height * _myMovie.aspectratio);
        if (myWidth > _myMovie.width) {
            myWidth = _myMovie.width;
            myHeight = Math.round(myWidth / _myMovie.aspectratio);
        }
        Public.width  = myWidth;
        Public.height = myHeight;
    }

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

    function initMovie() {
        Public.loop = Protected.getBoolean("loop", false);
        Public.mode = Protected.getString("mode", "stop");
        Public.audio = Protected.getBoolean("audio", true);
        Public.volume = Protected.getNumber("volume", 1.0);
        Public.width = Protected.getNumber("width", Public.width);
        Public.height = Protected.getNumber("height", Public.height);
    }

    // XXX crude hack starts here

    Public.onMovieChanged getter = function() {
        return _myOnMovieChanged;
    };
    Public.onMovieChanged setter = function(f) {
        _myOnMovieChanged = f;
    };

    // XXX crude hack ends here
};
