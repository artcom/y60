//=============================================================================
// Copyright (C) 2009, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

/*jslint*/
/*globals use, spark, Vector2f*/

use("OffscreenRenderer.js");

spark.CoverFlow = spark.ComponentClass("CoverFlow");

spark.CoverFlow.Constructor = function (Protected) {
    var Base = {};
    var Public = this;

    var CACHED_IMAGES = 30;
    const MAX_RENDERED_IMAGES = 1;
    const MAX_DELETED_IMAGES = 2;

    const FORWARD = 1;
    const BACK = -1;
    const MAX_ANIMATION_DURATION = 2;
    const MIN_ANIMATION_DURATION = 0.2;
    const IDLE = "idle";
    const INCREASE = "increase";
    const DECREASE = "decrease";
    const SPEEDING = "speeding";
    const COVER_Z_POSITION = -1000;
    const PER_COVER_DISTANCE = 110;
    const COVER_ROTATION = 45;
    const CONTENT_WINDOW_SIZE_X = 690;
    const CONTENT_WINDOW_SIZE_Y = 690;
    const BODYCOUNT = 11;
    var CENTER_OFFSET = Math.floor(BODYCOUNT/2);
    const MAX_ANIM_ENTRIES = 20;

    var _myAlphaAnimationFlag = true;
    var _myCoverRotation = COVER_ROTATION;
    var _myPerCoverDistance = PER_COVER_DISTANCE;
    var _myCoverSize = new Vector2f(CONTENT_WINDOW_SIZE_X, CONTENT_WINDOW_SIZE_Y);
    var _myBodyCount = BODYCOUNT;
    var _myMinAnimationDuration = MIN_ANIMATION_DURATION;
    var _myMaxAnimationDuration = MAX_ANIMATION_DURATION;
    var _myMaxAnimatedEntries = MAX_ANIM_ENTRIES;
    var _myCoverPositionZ = COVER_Z_POSITION;
    var _myAnimationDuration = 1;
    var _myWidth = null;
    var _myHeight = null;
    var _myOffscreenTransform = null;
    var _myNormalTransform = null;
    var _myOffScreenImage = null;
    var _myOffScreenRenderer = null;
    var _myOffscreenBody = null;
    var _myOffscreenCam = null;
    var _myVisibility = true;

    var _myNumberOfEntries = 0;
    var _myBodyPool = [];
    var _myStartIndex = 0;
    var _myTargetIndex = 2;
    var _myRealTargetIndex = 2;
    var _myActionStartTime = -2;
    var _myEntryImages = {};
    var _myFirstPointer = 0;
    var _myLastPagePosition = 0;
    var _myEntries = null;

    var _myDirection = 1;
    var _myCurrentIndex = 0;
    var _mySkip = 0;
    var _myState = IDLE;
    var _myCurrentSpeed = 0;
    var _myMaxSpeed = 0;
    var _myStartSpeed = 0;
    var _myRestOfTheWorldNode = null;
    var _myEntrySrc = "";

    this.Inherit(spark.Transform);

    Public.currentindex getter = function() {
        return _myCurrentIndex;
    }

    Public.numberofentries getter = function() {
        return _myNumberOfEntries;
    }

    /* use it to define a groupnode for everything
       what should not be rendered offscreen from the Coverflow
     */
    Public.restoftheworld setter = function (theNode) {
        _myRestOfTheWorldNode = theNode;
    }

    Base.realize = Public.realize;
    Public.realize = function() {
        Base.realize();
        _myWidth = Protected.getNumber("width", window.width);
        _myHeight = Protected.getNumber("height", window.height);
        _myCoverRotation = Protected.getNumber("cover_rotation", COVER_ROTATION);
        _myPerCoverDistance = Protected.getNumber("per_cover_distance", PER_COVER_DISTANCE);
        _myAlphaAnimationFlag = Protected.getBoolean("animate_alpha", true);
        _myCoverSize = Protected.getVector2f("cover_size", new Vector2f(CONTENT_WINDOW_SIZE_X, CONTENT_WINDOW_SIZE_Y));
        _myBodyCount = Protected.getNumber("bodycount", BODYCOUNT);
        _myEntrySrc = Protected.getString("entry_src");
        _myCoverPositionZ = Protected.getNumber("cover_position_z", COVER_Z_POSITION);
        _myMinAnimationDuration = Protected.getNumber("min_animation_duration", MIN_ANIMATION_DURATION);
        _myMaxAnimationDuration = Protected.getNumber("max_animation_duration", MAX_ANIMATION_DURATION);
        _myMaxAnimatedEntries = Protected.getNumber("max_animated_entries", MAX_ANIM_ENTRIES);
        if (_myMaxAnimatedEntries%2 != 0) {
            Logger.warning("max_animated_entries has to be an even number");
            _myMaxAnimatedEntries -= 1;
        }

        CENTER_OFFSET = Math.floor(_myBodyCount/2);

        _myOffscreenTransform = new spark.Transform();
        Public.addChild(_myOffscreenTransform);
        _myOffscreenTransform.name = "OffscreenTransform";
        _myOffscreenTransform.realize();
        _myOffscreenTransform.visible = false;

        _myNormalTransform = new spark.Transform();
        Public.addChild(_myNormalTransform);
        _myNormalTransform.name = "NormalTransform";
        _myNormalTransform.realize();

    }

    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        Base.postRealize();
        _myOffscreenCam = window.camera.cloneNode(true);
        adjustNodeId(_myOffscreenCam, true);
        Public.sceneNode.appendChild(_myOffscreenCam);
        setupCamera3d(_myOffscreenCam, _myWidth, _myHeight);
        createOffScreenRenderer(_myOffscreenCam);

        if (_myBodyCount > CACHED_IMAGES) {
            Logger.warning("_myBodyCount > CACHED_IMAGES resizing image cache");
            CACHED_IMAGES = ((_myBodyCount+1)%2 == 1) ? _myBodyCount : _myBodyCount +1;
        }
        Public.loadEntries();
        createBodyPool();
        _myCurrentIndex = _myBodyPool[CENTER_OFFSET].imageidx;

        // ON FRAME
        Public.stage.addEventListener(spark.StageEvent.FRAME, function(theEvent) {

            if (_myNumberOfEntries == 0) {
                return;
            }
            addImageToCache();
            removeOldImageFromCache();

            positionEntries(theEvent.currenttime, theEvent.deltaT);
            if (Public.visible) {
                renderSceneOffScreen();
            }
            _myVisibility = _myNormalTransform.visible;
        });

        Public.stage.addEventListener(spark.StageEvent.PRE_RENDER, function(theEvent) {
            _myNormalTransform.visible = false;
            if (_myOffscreenBody) {
                _myOffscreenTransform.visible = _myVisibility;
            }
        });

        Public.stage.addEventListener(spark.StageEvent.POST_RENDER, function(theEvent) {
            _myNormalTransform.visible = _myVisibility;
            if (_myOffscreenBody) {
                _myOffscreenTransform.visible = false;
            }
        });

        Public.stage.addEventListener(spark.KeyboardEvent.KEY_DOWN, function(theEvent) {
            if (Public.visible) {
                if (theEvent.keyString == "right") {
                    Public.startAnimation((_myCurrentIndex+1)/(_myNumberOfEntries-1));
                }
                if (theEvent.keyString == "left") {
                    Public.startAnimation((_myCurrentIndex-1)/(_myNumberOfEntries-1));
                }
            }
        });

        Public.startAnimation(0.5);
    }

    Public.loadEntries = function() {
        _myEntries = new Node();
        _myEntries.parseFile(_myEntrySrc);
        _myEntries = _myEntries.firstChild;
        if (_myEntries) {
            _myNumberOfEntries = _myEntries.childNodes.length;
        }
        print("found " + _myNumberOfEntries + " entries in coverflow");

        var myCacheSize = Math.min(_myNumberOfEntries, CACHED_IMAGES);
        for (var i = 0; i < myCacheSize; ++i) {
            createEntryImageIndex(_myNumberOfEntries - myCacheSize + i);
        }
        _mySkip = 0;
    }

    function addImageToCache () {
        var myCounter = 0;
        for (var i = 0; i < CACHED_IMAGES; ++i) {
            var myIndex = _myRealTargetIndex - (CACHED_IMAGES/2 - i) * _myDirection;
            if (!(myIndex in _myEntryImages) && myIndex >= 0 && myIndex < _myNumberOfEntries) {
                createEntryImageIndex(myIndex);
                ++myCounter;
                if(myCounter >= MAX_RENDERED_IMAGES) {
                    return;
                }
            }
        }
    }

    function removeOldImageFromCache () {
        var myCounter = 0;
        for (imageidx in _myEntryImages) {
            if ((imageidx < _myCurrentIndex - CACHED_IMAGES/2 || imageidx > _myCurrentIndex + CACHED_IMAGES/2) &&
                (imageidx < _myRealTargetIndex - CACHED_IMAGES/2 || imageidx > _myRealTargetIndex + CACHED_IMAGES/2)) {
                window.scene.images.removeChild(window.scene.images.getElementById(_myEntryImages[imageidx]));
                _myEntryImages[imageidx] = undefined;
                delete _myEntryImages[imageidx];
                ++myCounter;
                if(myCounter >= MAX_DELETED_IMAGES) {
                    return;
                }
            }

        }
    }

    Public.fadeOutOffscreenBody = function() {
        var myClosureAnimation = new GUI.ClosureAnimation(FADE_TIME , unpenner(Easing.easeInQuad),
                                            function(theProgress) {
                                                Modelling.setAlpha(_myOffscreenBody, (1-theProgress));
                                            }
                                            );
        myClosureAnimation.onFinish = function() {
            Public.visible = false;
        }
        playAnimation(myClosureAnimation);
    }

    Public.fadeInOffscreenBody = function(theDelayFlag) {
        var mySequenceAnimation = new GUI.SequenceAnimation();
        if (theDelayFlag != undefined && theDelayFlag) {
            mySequenceAnimation.add(new GUI.DelayAnimation(FADE_TIME));
        }
        var myClosureAnimation = new GUI.ClosureAnimation(FADE_TIME , null,
                                            function(theProgress) {
                                                Modelling.setAlpha(_myOffscreenBody, theProgress);
                                            }
                                            );
       myClosureAnimation.onPlay = function(){
           Public.visible = true;
       }
       mySequenceAnimation.add(myClosureAnimation);
       playAnimation(mySequenceAnimation);
    }

    function createEntryImageIndex(theIndex) {
        var myEntry = _myEntries.childNode(theIndex);
        createEntryImage(theIndex, myEntry.src);
    }

    function createEntryImage(theIndex, theSrc) {
        _myEntryImages[theIndex] = Modelling.createImage(window.scene, theSrc).id;
    }

    function createBodyPool() {
        _myBodyPool = [];

        for (var i = _myNumberOfEntries - _myBodyCount; i < _myNumberOfEntries; ++i) {
            var myVisible = (i >= 0 && i < _myNumberOfEntries);
            var myImage = null;
            var myBody = null;
            var myEntry = null;
            if (myVisible) {
                myImage = window.scene.images.getElementById(_myEntryImages[String(i)]);
                var myImageSize = getImageSize(myImage);
                var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myImage, "gb_entry_material", 1);
                myMaterial.transparent = true;
                var myTexture = myMaterial.childNode("textureunits").firstChild.$texture;
                myTexture.mipmap = false;
                addMaterialProperty(myMaterial, "bool", "depthtest", false);
                var myShape = Modelling.createQuad(window.scene, myMaterial.id, new Vector3f(Math.round(-myImageSize.x/2),Math.round(-myImageSize.y/2),0), new Vector3f(Math.round(myImageSize.x/2),Math.round(myImageSize.y/2),0));
                var myBody = Modelling.createBody(_myNormalTransform.sceneNode, myShape.id);
                myBody.name = "coverbody_" + i;
                myBody.position.y = Math.round(myImageSize.y/2);
                myBody.scale.x = _myCoverSize.x/myImageSize.x;
                myBody.scale.y = _myCoverSize.y/myImageSize.y;
                myEntry = _myEntries.childNode(i);
            } else {
                myImage = Modelling.createImage(window.scene, 1,1,"RGBA");
                var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myImage, "gb_entry_material", 1);
                myMaterial.transparent = true;
                var myTexture = myMaterial.childNode("textureunits").firstChild.$texture;
                myTexture.mipmap = false;
                addMaterialProperty(myMaterial, "bool", "depthtest", false);
                var myShape = Modelling.createQuad(window.scene, myMaterial.id, new Vector3f(Math.round(-_myCoverSize.x/2),Math.round(-_myCoverSize.y/2),0), new Vector3f(Math.round(_myCoverSize.x/2),Math.round(_myCoverSize.y/2),0));
                var myBody = Modelling.createBody(_myNormalTransform.sceneNode, myShape.id);
                myBody.name = "coverbody_" + i;
                myBody.position.y = Math.round(_myCoverSize.y/2);
            }

            _myBodyPool.push({body: myBody, tex: myTexture, imageidx: i, visible: myVisible});
        }
    }
    /*
    Public.addEntry = function(theDate, theName, theCity, theText) {
        createEntryImage(_myNumberOfEntries, theDate, theName, theCity, theText);
        var myIndex = (_myFirstPointer+CENTER_OFFSET)%_myBodyCount;
        _myBodyPool[myIndex].tex.image = _myEntryImages[_myNumberOfEntries];
        _myBodyPool[myIndex].imageidx = _myNumberOfEntries;
        _myBodyPool[myIndex].body.position.x = 900;
        _myBodyPool[myIndex].body.position.z = 0;
        _myBodyPool[myIndex].visible = true;
        _myBodyPool[myIndex].body.visible = true;

        var myEntry = _myEntries.childNode(_myNumberOfEntries);
        _myBodyPool[myIndex].entry = myEntry;

        var myTargetRotation = Quaternionf.createFromEuler(new Vector3f(0,0,0));
        _myBodyPool[myIndex].body.orientation = myTargetRotation;
        var myScaleFactor = 0.96;
        var myClosureAnimation = new GUI.ClosureAnimation(FADE_TIME, unpenner(Easing.easeInQuad),
                                            function(theProgress) {
                                                Modelling.setAlpha(_myBodyPool[myIndex].body, theProgress);
                                                 myScaleFactor = 0.96+0.04*theProgress;
                                                _myBodyPool[myIndex].body.scale.x = myScaleFactor;
                                                _myBodyPool[myIndex].body.scale.y = myScaleFactor;
                                            }
                                            );
        myClosureAnimation.onFinish = function() {
            _mySlider.sensible = true;
            Modelling.setAlpha(_myBodyPool[myIndex].body, 1);
            _myBodyPool[myIndex].body.scale.x = 1;
            _myBodyPool[myIndex].body.scale.y = 1;
            ourShow.newentryopenflag = false;
        }
        playAnimation(myClosureAnimation);

        ourShow.entryWindow.visible = false;
        _myNumberOfEntries++;

    }*/

    function getCacheSize() {
        var myCount = 0;
        for(i in _myEntryImages) {
            ++myCount;
        }
        return myCount;
    }

    function printEntries() {
        for(i in _myEntryImages) {
            print(i)
        }
        print("---------------------")
    }


    function updateBodyPool(theEntryDifference) {
        for (var i = 0; i < Math.abs(theEntryDifference); ++i) {
            if (theEntryDifference > 0) { //increase page number
                var myNewIdx = _myBodyPool[(_myFirstPointer -1 + _myBodyCount)% _myBodyCount].imageidx + 1;
                var myNewSkipIdx = myNewIdx + _mySkip;
                if (myNewSkipIdx >= _myNumberOfEntries || myNewSkipIdx < 0) {
                    _myBodyPool[_myFirstPointer].visible = false;
                } else {
                    if(!(myNewSkipIdx in _myEntryImages)) {
                        Logger.warning("the needed image with idx " + myNewSkipIdx + " not found in image cache ->creating it!");
                        createEntryImageIndex(myNewSkipIdx);
                    }
                    var myEntry = _myEntries.childNode(myNewSkipIdx);
                    _myBodyPool[_myFirstPointer].tex.image = _myEntryImages[myNewSkipIdx];
                    _myBodyPool[_myFirstPointer].visible = true;
                }

                _myBodyPool[_myFirstPointer].imageidx = myNewIdx;
                _myFirstPointer = (++_myFirstPointer + _myBodyCount)% _myBodyCount;
            } else { //decrease page number
                var myNewIdx = _myBodyPool[_myFirstPointer].imageidx - 1;
                var myNewSkipIdx = myNewIdx + _mySkip;
                _myFirstPointer = (--_myFirstPointer + _myBodyCount)% _myBodyCount;
                if (myNewSkipIdx >= _myNumberOfEntries || myNewSkipIdx < 0) {
                    _myBodyPool[_myFirstPointer].visible = false;
                } else {
                    if(!(myNewSkipIdx in _myEntryImages)) {
                        Logger.warning("the needed image with idx " + myNewSkipIdx + " not found in image cache ->creating it!");
                        createEntryImageIndex(myNewSkipIdx);
                    }

                    var myEntry = _myEntries.childNode(myNewSkipIdx);
                    _myBodyPool[_myFirstPointer].tex.image = _myEntryImages[myNewSkipIdx];
                    _myBodyPool[_myFirstPointer].visible = true;
                }
                _myBodyPool[_myFirstPointer].imageidx = myNewIdx;
            }
        }
    }

    //fix indices after animation with skipped textures
    function fixSkip() {
        _myCurrentIndex += _mySkip;
        for (var i = 0; i < _myBodyCount; ++i) {
            _myBodyPool[i].imageidx += _mySkip;
        }
        _mySkip = 0;
    }

    Public.startAnimation = function(theTarget,theNewEntryFlag) {
        if (_mySkip != 0) {
            fixSkip();
        }
        if (theNewEntryFlag != undefined && theNewEntryFlag) {
            _myRealTargetIndex = _myNumberOfEntries;
        } else {
            _myRealTargetIndex = clamp(Math.floor(theTarget * (_myNumberOfEntries)),0,Math.max(_myNumberOfEntries-1,0));
        }
        _myStartIndex = _myCurrentIndex;
        _myActionStartTime = -1;
        _myDirection = (_myRealTargetIndex - _myCurrentIndex < 0) ? -1 : 1;

        if (Math.abs(_myRealTargetIndex - Math.floor(_myStartIndex)) >= _myMaxAnimatedEntries) {
            _myTargetIndex = Math.floor(_myStartIndex) + _myDirection * _myMaxAnimatedEntries;
        } else {
            _myTargetIndex = _myRealTargetIndex;
        }
        _myAnimationDuration = _myMinAnimationDuration + Math.abs(_myTargetIndex-_myCurrentIndex)/Math.max(_myMaxAnimatedEntries,_myBodyCount) * (_myMaxAnimationDuration-_myMinAnimationDuration);
        return _myAnimationDuration;
    }

    function changeState(theState) {
        _myState = theState;
        if (_myState == INCREASE) {
            _myLastPagePosition = _myStartIndex;
            _myMaxSpeed = ((_myTargetIndex - _myCurrentIndex)/ (_myAnimationDuration) - 0.1 * _myCurrentSpeed) / 0.8;
            _myStartSpeed = _myCurrentSpeed;
        } else if (_myState == DECREASE) {
        } else if (_myState == IDLE) {
            _myCurrentIndex = _myTargetIndex;
            _myCurrentSpeed = 0;
        }
    }

    function positionEntries(theTime, theDeltaT) {
        if (_myActionStartTime < -1) {
            return;
        }
        if (_myActionStartTime == -1) {
            _myActionStartTime = theTime;
            changeState(INCREASE);
        }
        var myDelta = (theTime - _myActionStartTime)/_myAnimationDuration;

        if ( (_myState != IDLE) && (_myState != DECREASE) && myDelta > 0.8) {
            changeState(DECREASE);
        }

        if (_myState == INCREASE) {
            if (myDelta > 0.2) {
                _myCurrentSpeed = _myMaxSpeed;
                changeState(SPEEDING);
            } else {
                _myCurrentSpeed = (myDelta / (0.2) * (_myMaxSpeed - _myStartSpeed) + _myStartSpeed);
            }
        } else if (_myState == DECREASE) {
            var myFraction = 0.5;
            _myCurrentSpeed = (_myTargetIndex - _myCurrentIndex) * myFraction/theDeltaT;
        } else if (_myState == SPEEDING) {

        } else if (_myState == IDLE) {
            return;
        }


        if (myDelta > 1) {
            changeState(IDLE);
        } else {
            _myCurrentIndex += _myCurrentSpeed * theDeltaT;
        }

        var mySpeed = _myCurrentSpeed * _myDirection/_myNumberOfEntries;
        var myPhase = 1.0-(_myCurrentIndex - Math.floor(_myCurrentIndex));
        var myCurrentPage = Math.floor(_myCurrentIndex);
        var myLastPage = Math.floor(_myLastPagePosition);
        var myEntryDifference = myCurrentPage-myLastPage;
        if (myEntryDifference) {
            _mySkip = 0;
            if ((Math.abs(_myRealTargetIndex - Math.floor(_myStartIndex)) >= _myMaxAnimatedEntries) &&
                Math.abs(myCurrentPage - Math.floor(_myStartIndex)) >= _myMaxAnimatedEntries/2) {
                var mySDist = Math.abs(_myRealTargetIndex - Math.floor(_myStartIndex)) - _myMaxAnimatedEntries;
                _mySkip += (myEntryDifference > 0 ? 1: -1) * mySDist;
            }
            updateBodyPool(myEntryDifference, _mySkip);
        }
        _myLastPagePosition = _myCurrentIndex;

        //print("delta "+myDelta);
        //print("pagepos "+_myCurrentIndex);
        //print("speed "+mySpeed);
        //print(_myTargetIndex,_myStartIndex,myCurrentPage,_myDirection,myPhase);

        for (var i = 0; i < _myBodyCount; ++i) {

            if (_myBodyPool[i].visible) {
                _myBodyPool[i].body.visible = true;
            } else {
                _myBodyPool[i].body.visible = false;
                continue;
            }

            var reali = ((i-_myFirstPointer+_myBodyCount)%_myBodyCount);

            //rotation
            var myTargetRotation = Quaternionf.createFromEuler(new Vector3f(0,0,0));
            if (reali == CENTER_OFFSET) {
                myTargetRotation = Quaternionf.createFromEuler(new Vector3f(0,radFromDeg(_myCoverRotation+Math.abs(mySpeed)+myPhase*-_myCoverRotation),0));
            } else if (reali == CENTER_OFFSET+1) {
                myTargetRotation = Quaternionf.createFromEuler(new Vector3f(0,radFromDeg(-_myCoverRotation-Math.abs(mySpeed)+(1.0-myPhase)*_myCoverRotation),0));
            } else if (reali < CENTER_OFFSET) {
                myTargetRotation = Quaternionf.createFromEuler(new Vector3f(0,radFromDeg(_myCoverRotation+Math.abs(mySpeed)),0));
            } else if (reali > CENTER_OFFSET) {
                myTargetRotation = Quaternionf.createFromEuler(new Vector3f(0,radFromDeg(-_myCoverRotation-Math.abs(mySpeed)),0));
            }
            _myBodyPool[i].body.orientation = myTargetRotation;

            //position
            if (reali == CENTER_OFFSET) {
                if (Math.abs(_myStartIndex-_myTargetIndex) <= 1 ||
                    (Math.abs(myCurrentPage-_myTargetIndex) < 1 && _myDirection < 0) ||
                    (Math.abs(myCurrentPage-_myTargetIndex) == 0 && _myDirection > 0) ||

                    (Math.abs(myCurrentPage-Math.floor(_myStartIndex)) == 0 && _myDirection > 0) ||
                    (Math.abs(myCurrentPage-Math.floor(_myStartIndex)) == 0 && _myDirection < 0) )

                {
                    _myBodyPool[i].body.position.z = (1.0-myPhase) * _myCoverPositionZ;
                } else {
                    _myBodyPool[i].body.position.z = _myCoverPositionZ;
                }
                var myMovement = (_myWidth/2 - ((CENTER_OFFSET-1-1) * _myPerCoverDistance - _myPerCoverDistance/2));
                if (myMovement < 0) {
                    Logger.warning("CoverFlow layout error - you should either show less covers at once or reduce the per cover distance");
                }
                _myBodyPool[i].body.position.x = _myWidth/2 - (1.0-Math.pow(myPhase,1.33333)) * myMovement;
            } else if (reali == CENTER_OFFSET+1) {
                if (Math.abs(_myStartIndex-_myTargetIndex) <= 1 ||
                   (Math.abs(myCurrentPage-_myTargetIndex) <= 1 && _myDirection > 0) ||

                   (Math.abs(myCurrentPage-Math.floor(_myStartIndex)) <= 1 && _myDirection < 0))
                {
                    _myBodyPool[i].body.position.z = myPhase * _myCoverPositionZ;
                } else {
                    _myBodyPool[i].body.position.z = _myCoverPositionZ;
                }
                var myMovement = ((_myWidth - ((_myBodyCount -1 - CENTER_OFFSET-1) * _myPerCoverDistance - _myPerCoverDistance - _myPerCoverDistance/2)) - _myWidth/2);
                if (myMovement < 0) {
                    Logger.warning("CoverFlow layout error - you should either show less covers at once or reduce the per cover distance");
                }
                _myBodyPool[i].body.position.x = _myWidth/2 + Math.pow(myPhase,0.75) * myMovement;
            } else {
                if (reali < CENTER_OFFSET) {
                    _myBodyPool[i].body.position.x = (reali-1) * _myPerCoverDistance + (myPhase -1) * _myPerCoverDistance - _myPerCoverDistance/2;
                } else {
                    _myBodyPool[i].body.position.x = _myWidth - ((_myBodyCount -1 - reali) * _myPerCoverDistance - myPhase * _myPerCoverDistance - _myPerCoverDistance/2);
                }

                //z-offset
                _myBodyPool[i].body.position.z = _myCoverPositionZ-Math.abs(reali-CENTER_OFFSET)*2;
            }

            //alpha
            if (_myAlphaAnimationFlag) {
                var myAlpha = 1.0;
                if (reali > CENTER_OFFSET+1) {
                    myAlpha = 1.0- Math.abs(reali+(-1.0+myPhase)-(CENTER_OFFSET+1))/(CENTER_OFFSET-1) * 0.75;
                } else if(reali <= CENTER_OFFSET-1) {
                    myAlpha = 1.0- Math.abs(reali+(-1.0+myPhase)-(CENTER_OFFSET-1))/(CENTER_OFFSET-1) * 0.75;
                }
                Modelling.setAlpha(_myBodyPool[i].body, myAlpha);
            }
        }

        if (_myState == IDLE && _mySkip != 0) {
            fixSkip();
        }
    }


    function createOffScreenRenderer (theCamera) {
        _myOffScreenImage = Modelling.createImage(window.scene, _myWidth, _myHeight, "RGBA");
        _myOffScreenImage.resize = "none";

        // Flip vertically since framebuffer content is upside-down
        var myMirrorMatrix = new Matrix4f;
        myMirrorMatrix.makeScaling(new Vector3f(1,-1,1));
        _myOffScreenImage.matrix.postMultiply(myMirrorMatrix);

        _myOffScreenRenderer = new OffscreenRenderer([_myWidth, _myHeight], theCamera, "RGB", _myOffScreenImage, undefined, true, 0);
        _myOffScreenRenderer.texture.wrapmode = "clamp_to_edge";
        _myOffScreenRenderer.setBackgroundColor([1,1,1,0]);
        var myMaterial = buildUnlitTextureMaterialNode("offscreen_material", [_myOffScreenRenderer.texture.id]);
        myMaterial.transparent = true;
        var myQuad = Modelling.createQuad(window.scene, myMaterial.id, new Vector3f(-_myWidth/2, -_myHeight/2, 0), new Vector3f(_myWidth/2, _myHeight/2, 0));
        var myNode = window.scene.world.find("//*[@name='OffscreenTransform']");
        _myOffscreenBody = Modelling.createBody(myNode, myQuad.id);
        _myOffscreenBody.name = "OffscreenBody";
        _myOffscreenBody.position = new Vector3f(_myWidth/2, _myHeight/2, 0);
        _myOffscreenBody.scale.y *=-1;
        _myOffscreenBody.insensible = true;
    }

    function renderSceneOffScreen() {
        if (_myRestOfTheWorldNode) {
            _myRestOfTheWorldNode.visible = false;
        }
        _myOffscreenTransform.visible = false;
        _myNormalTransform.visible = true;
        _myOffScreenRenderer.renderarea.renderToCanvas(false);
        if (_myRestOfTheWorldNode) {
            _myRestOfTheWorldNode.visible = true;
        }
    }

    function setupCamera3d(theCamera, theWidth, theHeight) {
        theCamera.frustum = new Frustum();
        theCamera.frustum.width = theWidth;
        theCamera.frustum.height = theHeight;
        theCamera.frustum.type = ProjectionType.perspective;
        theCamera.position.x = theWidth/2;
        theCamera.position.y = theHeight/2;
        var myHfov = 40;//90;
        theCamera.frustum.hfov = myHfov;
        theCamera.position.z = getCameraDistanceFromScreenLength(_myWidth, myHfov);
        theCamera.frustum.near = 0.1;
        theCamera.frustum.far  = 5000;
        theCamera.orientation = Quaternionf.createFromEuler(new Vector3f(0,0,0));
    }

    function getCameraDistanceFromScreenLength(theScreenLength, theHFov){
        var myAlpha = radFromDeg(theHFov / 2);
        var myDistance = 0.5 * theScreenLength / Math.abs(Math.tan(myAlpha));
        return myDistance;
    }

}
