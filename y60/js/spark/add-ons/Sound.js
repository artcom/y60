//=============================================================================
// Copyright (C) 2011, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

plug("Sound");

spark.Sound = spark.ComponentClass("Sound");
spark.Sound.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.Container);
    Public.Inherit(spark.EventDispatcher);
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _mySource     = null;
    var _mySourceId   = null;
    var _mySourceItem = null;
    var _mySound      = null;
    var _myLoopFlag   = false;
    var _myUseCaching = true;
    var _myVolumes    = [1,1];

    /////////////////////
    // Private Methods //
    /////////////////////
    
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

    ///////////////////////
    // Protected Methods //
    ///////////////////////

    Protected.handleI18nLanguage = function(e) {
        Public.src = e.src;
    };

    ////////////////////
    // Public Methods //
    ////////////////////
    
    // playback control

    Public.play = function() {
        if (_mySound) {
            _mySound.play();
        }
    };

    Public.stop = function() {
        if (_mySound) {
            _mySound.stop();
        }
    };

    Public.pause = function() {
        if (_mySound) {
            _mySound.pause();
        }
    };

    Public.fadeToVolume = function(theVolume, theDuration) {
        if (_mySound) {
            theDuration = theDuration || 0.5;
            _mySound.fadeToVolume(theVolume, theDuration);
        }
    };

    Public.seek = function(theTime) {
        if (_mySound) {
            _mySound.seek(theTime);
        }
    };

    Public.seekRelative = function(theTime) {
        if (_mySound) {
            _mySound.seekRelative(theTime);
        }
    };

    Public.__defineGetter__("duration", function () {
        return _mySound.duration;
    });
    Public.__defineGetter__("currentTime", function () {
        return _mySound.time;
    });
    Public.__defineGetter__("playing", function () {
        return _mySound.playing;
    });
    Public.__defineGetter__("loop", function () {
        return _mySound.looping;
    });
    Public.__defineSetter__("loop", function (theFlag) {
        _mySound.looping = theFlag;
        _myLoopFlag = theFlag;
    });

    Public.__defineGetter__("volume", function() {
        return _mySound.volume;
    });
    Public.__defineSetter__("volume", function(theVolume) {
        _mySound.volume = theVolume;
        _myVolumes[0] = theVolume;
        _myVolumes[1] = theVolume;
    });
    Public.__defineSetter__("volumes", function(theVolumes) {
        _mySound.setVolumes(theVolumes[0], theVolumes[1]);
        _myVolumes[0] = theVolumes[0];
        _myVolumes[1] = theVolumes[1];
    });

    Public.__defineGetter__("sound", function() {
        return _mySound;
    });

    Public.__defineSetter__("sound", function(theSound) {
        if (_mySound) {            
            Public.stop();
            _mySound = null;
        }
        _mySound = theSound;
    });

    Public.__defineGetter__("src", function() {
        return _mySource;
    });
    
    Public.__defineSetter__("src", function (theSourceFile) {
        if(_mySource !== theSourceFile) {
            _mySource = theSourceFile;   
            if (theSourceFile) {
                Public.sound = new Sound(theSourceFile, _myLoopFlag, _myUseCaching);
                Public.volumes = _myVolumes;
            }
        } else {
            if(_mySound.playing) {
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

    Base.realize = Public.realize;
    Public.realize = function() {
        Base.realize();
        var mySource = Protected.getString("src", "");
        var mySourceId = Protected.getString("srcId", "");
        _myVolumes = Protected.getVector2f("volumes", _myVolumes);
        _myLoopFlag = Protected.getBoolean("loop", _myLoopFlag);
        _myUseCaching = Protected.getBoolean("useCaching", _myUseCaching);
        if(mySource === "") {
            if(mySourceId !== "") {
                _mySourceId = mySourceId;
            }
        } else {
            Public.src = mySource;
        }
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        if(_mySourceId) {
            attachToI18nItem(_mySourceId);
        }
        Base.postRealize();
    };
    //XXX: duplicate code from widgets.js, yikes
    // INTERNATIONALISATION HOOKS
    var _myI18nContext = null;

    Public.__defineGetter__("i18nContext", function () {
        if (_myI18nContext) {
            return _myI18nContext;
        } else {
            if (Public.parent) {
                return Public.parent.i18nContext;
            } else {
                return null;
            }
        }
    });

    Public.__defineSetter__("i18nContext", function (theValue) {
        _myI18nContext = theValue; // XXX: re-trigger i18n events? how?
    });

    Public.__defineGetter__("i18nContexts", function () {
        var myContexts = [];
        var myCurrent = Public;
        while (myCurrent) {
            if (myContexts.length > 0) {
                var myContext = myCurrent.i18nContext;
                if (myContexts[myContexts.length - 1] !== myContext) {
                    myContexts.push(myCurrent.i18nContext);
                }
            } else {
                myContexts.push(myCurrent.i18nContext);
            }
            myCurrent = myCurrent.parent;
        }
        return myContexts;
    });

    Public.getI18nItemByName = function (theName) {
        var myContexts = Public.i18nContexts;
        for (var i = 0; i < myContexts.length; i++) {
            var myContext = myContexts[i];
            var myItem = myContext.getChildByName(theName);
            if (myItem) {
                return myItem;
            }
        }
        return null;
    };

};



