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
    var _myUseCaching        = false;    
    var _myTexture           = null;
    var _myStartFrame        = 0; //movie should start at this frame after setting src
    var _myDecoderHint;
    var _myTargetPixelFormat = "RGB";
    var _myCacheSize         = 8;
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

    function attachToI18nItem(theItemId) {
        if(_mySourceItem) {
            _mySourceItem.removeEventListener(spark.I18nEvent.LANGUAGE,
                                              Protected.handleI18nLanguage);
            _mySourceItem = null;
        }
        _mySourceItem = Public.getI18nItemByName(theItemId);
        if(!_mySourceItem) {
            Logger.fatal("no i18n item named " + theItemId);
        }
        _mySourceItem.addEventListener(spark.I18nEvent.LANGUAGE,
                                       Protected.handleI18nLanguage);
        Public.src = _mySourceItem.src;
    }

    function initMovie(theFullInitFlag) {
        if (theFullInitFlag) {
            _myMovie.currentframe = _myStartFrame;            
            Public.mode = Protected.getString("mode", "stop");
            var myVolumes = Protected.getArray("volumes", []);
            if (myVolumes.length === 0) {
                Public.volume = Protected.getNumber("volume", 1.0);
            } else {
                Public.volumes = myVolumes;
            }
        }
        Public.loop = Protected.getBoolean("loop", false);
        Public.playSpeed = Protected.getNumber("playspeed", 1);
        Public.width = Protected.getNumber("width", Public.width);
        Public.height = Protected.getNumber("height", Public.height);
        _myCacheSize = Protected.getNumber("cachesize", _myCacheSize);
    }
    
    
    ///////////////////////
    // Protected Methods //
    ///////////////////////

    Protected.handleI18nLanguage = function(e) {
        Public.src = e.src;
    };

    ////////////////////
    // Public Methods //
    ////////////////////

    
    Base.initialize = Public.initialize;
    Public.initialize = function(theNode) {
        if (theNode && (("useYuv2RgbShader" in theNode && theNode.useYuv2RgbShader === "true") ||
            ("targetpixelformat" in theNode && theNode.targetpixelformat === "YUV420")))
        {
            theNode.targetpixelformat = "YUV420";
            use("YUV2RGBShader.js");
            Public.Inherit(spark.YUV2RGBShader);
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

    Public.__defineGetter__("cachesize", function () {
        return _myCacheSize;
    });
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
    Public.__defineGetter__("fps", function() {
        return _myMovie.fps;
    });

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
        _myMovie.volume = new Vector2f(theVolume, theVolume);
    });
    Public.__defineSetter__("volumes", function(theVolumes) {
        _myMovie.volume = new Vector2f(theVolumes[0], theVolumes[1]);
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
            if (!_myUseCaching) {
                _myMovie.parentNode.removeChild(_myMovie);                
            }
            _myMovie = null;
        }
        _myMovie = theNode;
        _myTexture.image = theNode.id;
        Protected.onMovieChanged(true);
    });

    Public.__defineGetter__("src", function() {
        return _mySource;
    });
    
    Public.__defineSetter__("src", function (theSourceFile) {
        var myName, myCachedMovie;
        if(_mySource !== theSourceFile) {
            _mySource = theSourceFile;   
            if (_mySetSourceWithoutChangingImageNode) {
                if (_myMovie.nodeName === "image") {
                    if (_myUseCaching) {
                        Public.movie = spark.getCachedMovie(theSourceFile, _myTargetPixelFormat, _myDecoderHint, Protected.getBoolean("audio", true), _myStartFrame, _myCacheSize);
                    } else {                
                        Public.movie = spark.openMovie(theSourceFile, _myTargetPixelFormat, _myDecoderHint, Protected.getBoolean("audio", true), _myStartFrame, _myCacheSize);
                    }
                } else {
                    var myFullInitFlag = true;
                    if (_myUseCaching) {
                        // check cache
                        myName = spark.getMovieCacheKey(theSourceFile);
                        myCachedMovie = spark.getNode(myName);
                        if (myCachedMovie) {
                            _myMovie = myCachedMovie;
                            _myTexture.image = myCachedMovie.id;
                            // do not do a full init movie when we are using a cached video, which is already in use                            
                            myFullInitFlag = false;
                        } else {
                            _myMovie.src = theSourceFile;
                        }
                    } else {
                        _myMovie.src = theSourceFile;
                    }
                    if (myFullInitFlag) {
                        _myMovie.currentframe = _myStartFrame;
                    }
                    Protected.onMovieChanged(myFullInitFlag);
                }
            } else {
                if (_myUseCaching) {
                    myName = spark.getMovieCacheKey(theSourceFile);
                    myCachedMovie = spark.getNode(myName);
                    if (myCachedMovie) {
                        _myMovie = myCachedMovie;
                        _myTexture.image = myCachedMovie.id;
                        // do not do a full init movie when we are using a cached video, which is already in use                                                    
                        Protected.onMovieChanged(false);
                    } else {
                        Public.movie = spark.getCachedMovie(theSourceFile, _myTargetPixelFormat, _myDecoderHint, Protected.getBoolean("audio", true), _myStartFrame,_myCacheSize);
                    }
                } else {                
                    Public.movie = spark.openMovie(theSourceFile, _myTargetPixelFormat, _myDecoderHint, Protected.getBoolean("audio", true), _myStartFrame, _myCacheSize);
                }
            }
        } else {
            if(_myMovie.playmode !== "stop") {
                Public.stop();
            }
        }
    });

    Public.__defineGetter__("useCaching", function () {
        return _myUseCaching;
    });
    
    Public.__defineSetter__("useCaching", function (theFlag) {
        _myUseCaching = !!theFlag;
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
        _myCacheSize = Protected.getNumber("cachesize", _myCacheSize);        
        _myDecoderHint = Protected.getString("decoderhint", _myDecoderHint);
        _myTargetPixelFormat = Protected.getString("targetpixelformat", _myTargetPixelFormat);
        _mySetSourceWithoutChangingImageNode = Protected.getBoolean("setSourceWithoutChangingImageNode", _mySetSourceWithoutChangingImageNode);
        _myUseCaching = Protected.getBoolean("useCaching", _myUseCaching);

        if (myMovieSource === "") {
            var myWidth  = Protected.getNumber("width", 1);
            var myHeight = Protected.getNumber("height", 1);
            _myMovie      = Modelling.createImage(window.scene, myWidth, myHeight, "BGR");
            _myMovie.name = Public.name + "_movieDummyImage";
            if(myMovieSourceId !== "") {
                _mySourceId = myMovieSourceId;
            }
        } else {
            if (_myUseCaching) {
                _myMovie = spark.getCachedMovie(myMovieSource, _myTargetPixelFormat, _myDecoderHint, Protected.getBoolean("audio", true), _myStartFrame, _myCacheSize);
            } else {                
                _myMovie = spark.openMovie(myMovieSource, _myTargetPixelFormat, _myDecoderHint, Protected.getBoolean("audio", true), _myStartFrame, _myCacheSize);
            }
            
            _mySource = myMovieSource;
        }

        _myTexture  = Modelling.createTexture(window.scene, _myMovie);
        _myTexture.name = Public.name + "-texture";
        _myTexture.wrapmode = "clamp_to_edge";

        var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene,
                _myTexture, Public.name + "-material", true);

        Base.realize(myMaterial);
        Protected.onMovieChanged(true);
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        if (_mySourceId) {
            attachToI18nItem(_mySourceId);
        }
        Base.postRealize();
    };

    Protected.onMovieChanged = function(theFullInitFlag) {
        if (_myMovie.nodeName !== "image") {
            ensureAspectRatio();
            initMovie(theFullInitFlag);
        }
    };
};
