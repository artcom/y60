/*jslint nomen:false, white: false*/
/*globals spark, use, Logger, Modelling, window*/

/**
 * A simple movie player.
 *
 */
spark.Movie = spark.ComponentClass("Movie");

spark.Movie.Constructor = function(Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.ResizableRectangle);
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _mySource     = null;
    var _mySourceId   = null;
    var _mySourceItem = null;

    var _myMovie             = null;
    var _myTexture           = null;
    var _myStartFrame        = 0; //movie should start at this frame after setting src
    var _myDecoderHint       = "FFMpegDecoder2";
    var _myTargetPixelFormat = "RGB";
    var _mySetSourceWithoutChangingImageNode = false;

    /////////////////////
    // Private Methods //
    /////////////////////
    
    function ensureAspectRatio() {
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
    }

    function initMovie() {
        Public.loop = Protected.getBoolean("loop", false);
        Public.mode = Protected.getString("mode", "stop");
        Public.playSpeed = Protected.getNumber("playspeed", 1);
        Public.volume = Protected.getNumber("volume", 1.0);
        Public.width = Protected.getNumber("width", Public.width);
        Public.height = Protected.getNumber("height", Public.height);
    }

    ////////////////////
    // Public Methods //
    ////////////////////

    
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
    };
    
    // playback control

    Public.play = function() {
        if(_myMovie.nodeName === "movie") {
            _myMovie.playmode = "play";
        }
    };

    Public.stop = function() {
        if(_myMovie.nodeName === "movie") {
            _myMovie.playmode = "stop";
        }
    };

    Public.pause = function() {
        if(_myMovie.nodeName === "movie") {
            _myMovie.playmode = "pause";
        }
    };

    Public.__defineGetter__("mode", function () {
        return _myMovie.playmode;
    });
    Public.__defineSetter__("mode", function (thePlaymode) {
        _myMovie.playmode = thePlaymode;
    });

    Public.__defineGetter__("loop", function () {
        return _myMovie.loopcount === 0;
    });
    Public.__defineSetter__("loop", function (theFlag) {
        _myMovie.loopcount = theFlag ? 0 : 1;
    });

    Public.__defineGetter__("playSpeed", function () {
        return _myMovie.playspeed;
    });
    Public.__defineSetter__("playSpeed", function (theSpeed) {
        _myMovie.playspeed = theSpeed;
    });

    // frames
    Public.__defineGetter__("frameCount", function() {
        return _myMovie.framecount;
    });

    Public.__defineGetter__("currentFrame", function() {
        return _myMovie.currentframe;
    });
    Public.__defineSetter__("currentFrame", function(f) {
        _myMovie.currentframe = f;
    });

    Public.__defineGetter__("startFrame", function () {
        return _myStartFrame;
    });
    Public.__defineSetter__("startFrame", function (theValue) {
        _myStartFrame = theValue;
    });

    // audio
    Public.__defineGetter__("audio", function() {
        return _myMovie.audio === 1;
    });
    /* disabled for now, audio can only configured when opening theMovie
    Public.__defineSetter__("audio", function(theFlag) {
        _myMovie.audio = theFlag ? 1 : 0;
    });*/

    Public.__defineGetter__("volume", function() {
        return _myMovie.volume;
    });
    Public.__defineSetter__("volume", function(theVolume) {
        _myMovie.volume = theVolume;
    });

    Public.__defineGetter__("hasAudio", function() {
        return _myMovie.has_audio;
    });

    Public.__defineGetter__("movie", function() {
        return _myMovie;
    });

    Public.__defineSetter__("movie", function(theNode) {
        if (_myMovie) {
            if (_myMovie.nodeName === "movie" && _myMovie.playmode !== "stop") {
                Public.stop();
            }
            _myMovie.parentNode.removeChild(_myMovie);
            _myMovie = null;
        }

        _myMovie = theNode;
        _myTexture.image = theNode.id;
        Public.onMovieChanged();
    });

    Public.__defineGetter__("src", function() {
        return _mySource;
    });
    Public.__defineSetter__("src", function (theSourceFile) {
        if(_mySource !== theSourceFile) {
            _mySource = theSourceFile;
            if (_mySetSourceWithoutChangingImageNode) {
                if (_myMovie.nodeName === "image") {
                    Public.movie = spark.openMovie(theSourceFile, _myTargetPixelFormat, _myDecoderHint, Protected.getBoolean("audio", true), _myStartFrame);
                } else {
                    _myMovie.src = theSourceFile;
                    window.scene.loadMovieFrame(_myMovie, _myStartFrame);
                    Public.onMovieChanged();
                }
            } else {
                Public.movie = spark.openMovie(theSourceFile, _myTargetPixelFormat, _myDecoderHint, Protected.getBoolean("audio", true), _myStartFrame);
            }
        } else {
            if(_myMovie.playmode !== "stop") {
                Public.stop();
            }
        }
    });

    Public.__defineGetter__("srcId", function() {
        return _mySourceId;
    });
    Public.__defineSetter__("srcId", function(theValue) {
        _mySourceId = theValue;
        attachToI18nItem(theValue);
    });
    Public.__defineGetter__("i18nItem", function() {
        return Public.srcId;
    });
    Public.__defineSetter__("i18nItem", function(i) {
        Public.srcId = i;
    });

    // XXX: this should not exist.
    Public.__defineGetter__("texture", function() {
        return _myTexture;
    });

    Base.realize = Public.realize;
    Public.realize = function() {
        var myMovieSource = Protected.getString("src", "");
        var myMovieSourceId = Protected.getString("srcId", "");
        _myDecoderHint = Protected.getString("decoderhint", "FFMpegDecoder2");
        _myTargetPixelFormat = Protected.getString("targetpixelformat", "RGB");
        _mySetSourceWithoutChangingImageNode = Protected.getBoolean("setSourceWithoutChangingImageNode",false);

        if(myMovieSource === "") {
            var myWidth  = Protected.getNumber("width", 1);
            var myHeight = Protected.getNumber("height", 1);
            _myMovie      = Modelling.createImage(window.scene, myWidth, myHeight, "BGR");
            _myMovie.name = Public.name + "_movieDummyImage";
            if(myMovieSourceId !== "") {
                _mySourceId = myMovieSourceId;
            }
        } else {
            _myMovie = spark.openMovie(myMovieSource, _myTargetPixelFormat, _myDecoderHint, Protected.getBoolean("audio", true), _myStartFrame);
            _mySource = myMovieSource;
        }

        _myTexture  = Modelling.createTexture(window.scene, _myMovie);
        _myTexture.name = Public.name + "-texture";
        _myTexture.wrapmode = "clamp_to_edge";

        var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene,
                _myTexture, Public.name + "-material", true);

        Base.realize(myMaterial);
        Public.onMovieChanged();
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

    Public.onMovieChanged = function() {
        if (_myMovie.nodeName !== "image") {
            ensureAspectRatio();
            initMovie();
        }
    };
};
