//=============================================================================
// Copyright (C) 2011, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
use("JSON.js");

spark.CachedRectangle = spark.ComponentClass("CachedRectangle");
spark.CachedRectangle.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.Rectangle);

    Base.realize = Public.realize;
    Public.realize = function () {
        var myColor = Protected.getVector3f("color", new Vector3f(1,1,1));
        var myMaterial = null;
        if (!(myColor in spark.OSD.materialCache)) {
            myMaterial = Modelling.createColorMaterial(window.scene, myColor.rgb1);
            myMaterial.transparent = true;
            spark.OSD.materialCache[myColor] = myMaterial;
        } else {
            myMaterial = spark.OSD.materialCache[myColor];
        }
        Base.realize(myMaterial);
        Public.SetterOverride("color", function (theColor, theBaseSetter) {
            if (!(theColor in spark.OSD.materialCache)) {
                Protected.material = Modelling.createColorMaterial(window.scene, theColor.rgb1);
                Protected.material.transparent = true;
                Protected.shape.childNode("primitives").childNode("elements").material = Protected.material.id;
                spark.OSD.materialCache[theColor] = Protected.material;
            } else {
                Protected.material = spark.OSD.materialCache[theColor];
                Protected.shape.childNode("primitives").childNode("elements").material = Protected.material.id;
            }
        });
    };
};


spark.OSD = spark.ComponentClass("OSD");
spark.OSD.defaultSettingsFile = "CONFIG/OSDSettings.json";
spark.OSD.materialCache = {};
spark.OSD.Constructor = function (Protected) {
    var Base = {};
    var Public = this;
    Public.Inherit(spark.Canvas);

    var AUTOHIDE_DELAY = 10000;
    var MAX_NUMBER_SLIDERS_PER_COLUMN = 12;
    var GAP = 10;
    var TAB_GAP = 5;
    var FONT_SIZE = 13;
    var TEXT_COLOR = new Vector4f(1,1,1,1);
    var DEFAULT_COLOR = asColor("00659c");
    var HIGHLIGHT_COLOR = asColor("0095e9");
    var BG_COLOR = asColor("00334e");
    var DEFAULT_TAB = {"name" : "defaultTab", "label" : "Home"};
    var _mySliders = [];
    var _myOverlay = null;
    var _mySettingsFile = null;
    var _mySettings = null;
    var _myRestoredSettings = null;
    var _myFadeOutAnimation = null;
    var _myAutoHideFlag = false;
    var _myAutoSaveFlag = false;
    var _myTabs = [];
    var _myDefaultTab = DEFAULT_TAB.name;
    var _myUseTouchOsc = false;
    var _myNoGUIFlag = false;
    var _myTouchOscDispatcher = null;
    var _myHeight = 400;

    var _mySliderDefaults = {
        name        : null,
        range       : new Vector2f(0,1),
        start_value : 0,
        setter      : null,
        precision   : 3,
        width       : 300,
        height      : 15
    };

    Public.__defineGetter__("tabs", function() { return _myTabs;});
    Public.__defineGetter__("sliders", function() { return _mySliders;});

    Base.initialize = Public.initialize;
    Public.initialize = function(theNode) {
        Base.initialize(theNode);
    };

    function getCongruentPosition(theDepth, thePosition) {
        var myCamera = window.scene.canvases.firstChild.childNode("viewport").$camera;
        var myX = thePosition[0] - Public.stage.width / 2;
        var myY = thePosition[1] - Public.stage.height / 2;
        var myNewX = getPixelSizeForDepth(theDepth, myX) + myCamera.position.x;
        var myNewY = getPixelSizeForDepth(theDepth, myY) + myCamera.position.y;
        return new Vector3f(myNewX, myNewY, theDepth);
    }
    function getPixelSizeForDepth(theDepth, thePreferredSize){
        var myCamDepth = window.scene.canvases.firstChild.childNode("viewport").$camera.position.z;
        var myRatio = (myCamDepth - theDepth) / myCamDepth;
        return thePreferredSize * myRatio;
    }

    function extend(obj, source) {
        if (source) {
            for (var prop in source) {
                obj[prop] = source[prop];
            }
        }
        return obj;
    }

    Base.realize = Public.realize;
    Public.realize = function() {
        Public.name = Protected.getString("name", "OSD");
        Base.realize();
        Public.bind("RESIZE", function (theSize) {
            Public.camera.position.x = theSize.x/2;
            Public.camera.position.y = theSize.y/2;
            Public.camera.frustum.width = theSize.x;
            Public.camera.frustum.height = theSize.y;
        });
        Public.size = new Vector2f(Public.stage.width, _myHeight);
        _myNoGUIFlag = Protected.getBoolean("nogui", _myNoGUIFlag);
        _myUseTouchOsc = Protected.getBoolean("touchOsc", _myUseTouchOsc);
        _myAutoSaveFlag = Protected.getBoolean("autoSave", _myAutoSaveFlag);
        _myAutoHideFlag = Protected.getBoolean("autoHide", _myAutoHideFlag);
        if (_myAutoHideFlag) {
            _myFadeOutAnimation = new GUI.SequenceAnimation();
            _myFadeOutAnimation.add(new GUI.DelayAnimation(AUTOHIDE_DELAY));
            _myFadeOutAnimation.add(new GUI.PropertyAnimation(500, null, Public, "alpha", 1,0));
            _myFadeOutAnimation.onPlay = function () {
                Public.alpha = 1;
            }
            _myFadeOutAnimation.onFinish = function () {
                Public.visible = false;
            }
            _myFadeOutAnimation.onCancel = function () {
                Public.alpha = 1;
            }
        }
        _mySettingsFile = Protected.getString("settingsFile", spark.OSD.defaultSettingsFile);
        if (fileExists(_mySettingsFile)) {
            var myFileWithPath = searchFile(_mySettingsFile);
            _mySettings = readFileAsString(_mySettingsFile);
            _mySettings = JSON.parse(_mySettings);
        } else {
            if (!fileExists(getDirectoryPart(_mySettingsFile))) {
                createDirectory(getDirectoryPart(_mySettingsFile));
            }
            Logger.warning("OSDSettings file: " + _mySettingsFile + " does not exist, creating one");
            _mySettings = {};
            writeStringToFile(_mySettingsFile, JSON.stringify(_mySettings, null, "\t"));
        }
        _myRestoredSettings = extend({}, _mySettings);
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function() {
        Base.postRealize();
        var myBackground = new Node("<CachedRectangle name='background' width='" + Public.width + "' height='" + Public.height + "' color='[0.1,0.1,0.1]' alpha='0.8'/>");
        myBackground = spark.loadDocument(myBackground, Public);
        Public.world.appendChild(myBackground.sceneNode);
        myBackground.sceneNode.$shape.childNode("primitives").childNode("elements").$material.transparent = true;
        _myOverlay = new Node("<overlay name='Overlay_0' visible='1' material='" + Protected.material.id + "' alpha='1' width='" + Public.width + "' height='" + Public.height + "'/>").firstChild;
        if (!Public.stage.getActiveViewport().childNode("overlays")) {
            Public.stage.getActiveViewport().appendChild(new Node("<overlays/>").firstChild);
        }
        Public.stage.getActiveViewport().childNode("overlays").appendChild(_myOverlay);
        _myOverlay.visible = false;
        Public.visible = false;
        Public.render = false;
        Public.sceneNode.insensible = true;
        Public.stage.addEventListener(spark.KeyboardEvent.KEY_DOWN, handleKeys);
        Public.addEventListener(spark.SliderEvent.MOVE, updateProperty);
        Public.SetterOverride("visible", function (theFlag, theBaseSetter) {
            _myOverlay.visible = theFlag;
            Public.render = theFlag;
            Public.sceneNode.insensible = !theFlag;
            if (!theFlag) {
                if (_myAutoSaveFlag) {
                    writeStringToFile(_mySettingsFile, JSON.stringify(_mySettings, null, "\t"));
                }
            } else {
                checkFadeOut();
            }
        });
        Public.GetterOverride("visible", function (theBaseGetter) {
            return _myOverlay.visible;
        });
        Public.addEventListener(spark.CursorEvent.APPEAR, function (theEvent) {
            checkFadeOut();
        }, true);
        Public.addEventListener(spark.CursorEvent.VANISH, function (theEvent) {
            checkFadeOut();
        }, true);

        if (_myUseTouchOsc) {
            use("TouchOscDispatcher.js");
            _myTouchOscDispatcher = new spark.TouchOscDispatcher();
            _myTouchOscDispatcher.realize(Public);
            Public.stage.addEventListener(spark.StageEvent.FRAME, function(theEvent) {
                _myTouchOscDispatcher.update();
            });
            var myStageOnOsc = Public.stage.onOscEvent;
            Public.stage.onOscEvent = function (theEvent) {
                if (myStageOnOsc) {
                    myStageOnOsc(theEvent);
                }
                _myTouchOscDispatcher.onOscEvent(theEvent);
            };
        }
        adaptToCamera();
    };

    //override Canvas method
    Public.convertToCanvasCoordinates = function (theX, theY) {
        return new Vector2f(theX, theY);
    };
    function adaptToCamera() {
        var myCamera = window.scene.canvases.firstChild.childNode("viewport").$camera;
        myCamera.appendChild(Public.sceneNode);
        Public.origin = new Vector3f(Public.stage.width/2,-(Public.stage.height/2 -_myHeight), 0);
        if (myCamera.frustum.type == ProjectionType.perspective) {
            //Public.position = getCongruentPosition( -500, new Vector3f(0,0,0));
            //Public.scaleX = getPixelSizeForDepth(Public.z, Public.stage.width)/Public.stage.width;
            //Public.scaleY = getPixelSizeForDepth(Public.z, Public.stage.height)/Public.stage.height;
            Public.z = -BMW.utils.getCameraDistanceFromScreenLength(Public.stage.width, myCamera.frustum.hfov);
        } else {
            Public.z = -1;
        }
    };

    Public.changeTab = function (theTab) {
        if (theTab in _myTabs) {
            _myTabs[theTab].button.onClick();
        }
    };

    Public.addTab = function (theName, theOptions) {
        theOptions = theOptions || {};
        var myLabel = ("label" in theOptions) ? theOptions.label : theName;
        var myTransform = {name:theName};
        if (!_myNoGUIFlag) {
            myTransform = new Node("<Transform x='20' y='-70' z='1' name='" + theName + "' visible='false'/>").firstChild;
            myTransform = spark.loadDocument(myTransform, Public);
            Public.world.appendChild(myTransform.sceneNode);
        }
        if (theName in _myTabs) {
            dumpstack();
            Logger.warning("tab name '" + theName + "' already registered");
        }
        var myX = 20;
        for (tab in _myTabs) {
            myX += _myTabs[tab].button.width + TAB_GAP;
        }
        _myTabs[theName] = myTransform;
        var myButton = {};
        if(!_myNoGUIFlag) {
            myButton = new Node("<CachedRectangle x='" + myX + "' z='1' height='10' width='30' name='" + theName + "_button' visible='true'>"
                                    + "<Text z='1' name='" + theName + "_button_text' sensible='false' text='" + myLabel + "' textColor='"+TEXT_COLOR+"' fontSize='"+FONT_SIZE+"'/></CachedRectangle>");
            myButton = spark.loadDocument(myButton, Public);
            var myButtonLabel = myButton.getChildByName(theName + "_button_text");
            myButton.height = myButtonLabel.height + 10;
            myButton.width = myButtonLabel.width + 10;
            myButtonLabel.x = (myButton.width - myButtonLabel.width)/2;
            myButtonLabel.y = (myButton.height - myButtonLabel.height)/2;
            myButton.y = Public.height - myButton.height - 30;
            Public.world.appendChild(myButton.sceneNode);
        }
        myButton.tab = myTransform;
        myTransform.button = myButton;
        myTransform.label = myLabel;
        myTransform.slider_x_position = 0;
        myTransform.slider_label_maxwidth = 0;
        myTransform.groups = {};
        myButton.activate = function () {
            myButton.color = HIGHLIGHT_COLOR;
            myButton.tab.visible = true;
        };
        myButton.deactivate = function () {
            myButton.color = DEFAULT_COLOR;
            myButton.tab.visible = false;
        };
        myButton.onClick = function (theEvent) {
            this.activate();
            for (tab in _myTabs) {
                if (this.tab.name !== tab) {
                    _myTabs[tab].button.deactivate();
                }
            }
        }
        if ("make_default" in theOptions && theOptions.make_default) {
            _myDefaultTab = theName;
        }
        if(!_myNoGUIFlag) {
            myButton.deactivate();
            myButton.addEventListener(spark.CursorEvent.APPEAR, myButton.onClick, true);
            myButton.onClick();
        }
        appendPropertyToSettings(_myTabs[theName]);
    };

    Public.addGroup = function (theName, theOptions) {
        theOptions = theOptions || {};
        var myLabel = ("label" in theOptions) ? theOptions.label : theName;
        var tabname = ("tab" in theOptions) ? theOptions.tab : _myDefaultTab;
        var position = ("position" in theOptions) ? theOptions.position : null;
        var myTab;
        if (tabname in _myTabs) {
            myTab = _myTabs[tabname];
        } else {
            Logger.warning("forgot to addTab with name: " + tabname + " ?! added to defaulttab");
            if (!(_myDefaultTab in _myTabs)) {
                Public.addTab(_myDefaultTab, {label:DEFAULT_TAB.label, make_default:true});
            }
            myTab = _myTabs[_myDefaultTab];
        }
        var myTransform = {name:theName};
        if(!_myNoGUIFlag) {
            myTransform = new Node("<Transform z='1' y='-25' name='" + theName + "' visible='true'/>").firstChild;
            myTransform = spark.loadDocument(myTransform, myTab);
            var myLine = new Node("<CachedRectangle z='1' y='405' color='[1,1,1]' height='10' width='30' name='" + theName + "_line'>"
                                    + "<Text z='1' y='2' name='" + theName + "_group_text' sensible='false' text='" + myLabel + "' textColor='"+TEXT_COLOR+"' fontSize='"+FONT_SIZE+"'/></CachedRectangle>");
            myLine = spark.loadDocument(myLine, myTransform);
            myLine.height = 1;
            myLine.width = _mySliderDefaults.width;
            myTransform.slider_x_position = 0;
            myTransform.slider_label_maxwidth = 0;
            if (position) {
                myTransform.x = position.x;
                myTransform.y += position.y;
            } else {
                var counter = 0;
                for (g in myTab.groups) {
                    counter++;
                }
                myTransform.x = (_mySliderDefaults.width + 150) * counter;
            }
        }
        myTab.groups[theName] = myTransform;
        appendPropertyToSettings(myTab, myTransform);
    };

    Public.addSliderControl = function (theObject, theProperty, theOptions) {
        theOptions = theOptions || {};
        var myLabel = ("label" in theOptions) ? theOptions.label : theProperty;
        var tabname = ("tab" in theOptions) ? theOptions.tab : _myDefaultTab;
        var groupname = ("group" in theOptions) ? theOptions.group : null;
        var range   = ("range" in theOptions) ? theOptions.range : _mySliderDefaults.range;
        var setter  = ("setter" in theOptions) ? theOptions.setter : _mySliderDefaults.setter;
        var precision  = ("precision" in theOptions) ? theOptions.precision : _mySliderDefaults.precision;
        var myTab, myGroup;
        if (tabname in _myTabs) {
            myTab = _myTabs[tabname];
        } else {
            Logger.warning("forgot to addTab with name: " + tabname + " ?! added to defaulttab");
            if (!(_myDefaultTab in _myTabs)) {
                Public.addTab(_myDefaultTab, {label:DEFAULT_TAB.label, make_default:true});
            }
            myTab = _myTabs[_myDefaultTab];
        }
        if (myKey in _mySliders) {
            Logger.warning("slider name '" + myKey + "' already in use");
        }
        if (groupname && groupname in myTab.groups) {
            myGroup = myTab.groups[groupname];
        } else if (groupname) {
            Logger.warning("forgot to addGroup with name: " + groupname + " ?!");
        }
        var settingskey = myLabel;
        var myKey = myTab.name + "#" + (myGroup ? myGroup.name + "#" : "") + settingskey;
        _mySliders[myKey] = {};
        _mySliders[myKey].range = range;
        _mySliders[myKey].object = theObject;
        _mySliders[myKey].property = theProperty;
        _mySliders[myKey].setter = setter;
        _mySliders[myKey].tab = myTab;
        _mySliders[myKey].group = myGroup;
        _mySliders[myKey].settingskey = settingskey;
        _mySliders[myKey].getRelative = function (theValue) {
            return (theValue - this.range[0]) / (this.range[1] - this.range[0]);
        };
        _mySliders[myKey].updateValueText = function(theValue) {
            this.slider.value_text.text = theValue;
        };
        var myValue  = ("start_value" in theOptions) ? theOptions.start_value : _mySliders[myKey].range[0];
        if (_mySettings) {
            var myProperty;
            if (myGroup) {
                myProperty = _mySettings[myTab.name][myGroup.name][settingskey];
            } else {
                myProperty = _mySettings[myTab.name][settingskey];
            }
            if (myProperty) {
                myValue = parseFloat(myProperty);
            } else {
                appendPropertyToSettings(myTab, myGroup, settingskey, myValue.toFixed(precision));
            }
        }
        var min =  _mySliders[myKey].range[0] <  _mySliders[myKey].range[1] ?  _mySliders[myKey].range[0] :  _mySliders[myKey].range[1]
        var max =  _mySliders[myKey].range[0] <  _mySliders[myKey].range[1] ?  _mySliders[myKey].range[1] :  _mySliders[myKey].range[0]
        myValue = clamp(myValue,min,max);
        var mySlider = {};
        if (!_myNoGUIFlag) {
            mySlider = buildSlider(myTab, myGroup, theOptions);
            mySlider.name = myKey;
            mySlider.sceneNode.name = myKey;
            mySlider.label.text = myLabel;
            var myRelativePosition = _mySliders[myKey].getRelative(myValue);
            mySlider.setRelativeCursorPosition(myRelativePosition);
            mySlider.eventTarget = Public;
            _mySliders[myKey].slider = mySlider;
            setLayoutPosition(_mySliders[myKey]);
        } else {
            _mySliders[myKey].slider = mySlider;
            mySlider.setRelativeCursorPosition = function () {
            };
            _mySliders[myKey].updateValueText = function(theValue) {
            };
        }
        mySlider.precision = precision;
        setProperty(_mySliders[myKey], myValue);
    };

    var buildSlider = function (theTab, theGroup, theOptions) {
        var mySliderNode = new Node("<Slider z='1' name='noname' centered='false' sticky='true'/>");
        mySliderNode = mySliderNode.firstChild;
        var myLabel = new Node("<Text name='slider_label' z='3' sensible='false' textColor='" + TEXT_COLOR + "' fontSize='"+FONT_SIZE+"'/>").firstChild;
        mySliderNode.appendChild(myLabel);
        var myValue = new Node("<Text name='slider_value' z='3' sensible='false' maxWidth='200' textColor='" + TEXT_COLOR + "' fontSize='"+FONT_SIZE+"'/>").firstChild;
        mySliderNode.appendChild(myValue);

        var myBackground = new Node("<CachedRectangle name='background' color='"+BG_COLOR.rgb+"'/>").firstChild;
        mySliderNode.appendChild(myBackground);
        var myIdleCursor = new Node("<CachedRectangle name='idle-cursor' z='1' color='"+DEFAULT_COLOR.rgb+"'/>").firstChild;
        var myActiveCursor = new Node("<CachedRectangle name='active-cursor' z='1' color='"+HIGHLIGHT_COLOR.rgb+"'/>").firstChild;
        myBackground.appendChild(myIdleCursor);
        myBackground.appendChild(myActiveCursor);
        var mySlider;
        if (theGroup) {
            mySlider = spark.loadDocument(mySliderNode, theGroup);
        } else {
            mySlider = spark.loadDocument(mySliderNode, theTab);
        }
        mySlider.label = mySlider.getChildByName("slider_label");
        mySlider.value_text = mySlider.getChildByName("slider_value");
        mySlider.background = mySlider.getChildByName("background");
        var width  = ("width" in theOptions) ? theOptions.width : _mySliderDefaults.width;
        var height  = ("height" in theOptions) ? theOptions.height : _mySliderDefaults.height;
        mySlider.background.width = width;
        mySlider.background.height = height;
        mySlider.idleCursor.width = 10;
        mySlider.idleCursor.height = height;
        mySlider.activeCursor.width = 10;
        mySlider.activeCursor.height = height;
        return mySlider;
    };

    var setLayoutPosition = function(theSliderInfo) {
        var theSlider = theSliderInfo.slider;
        var myTransform = theSliderInfo.group ? theSliderInfo.group : theSliderInfo.tab;
        theSlider.x = myTransform.slider_x_position;
        var sliderCount = theSliderInfo.group ? theSliderInfo.group.children.length -1 : theSliderInfo.tab.children.length
        theSlider.y =  Public.height - (theSlider.background.height + GAP ) * ((sliderCount-1)%MAX_NUMBER_SLIDERS_PER_COLUMN +1);
        theSlider.label.x = theSlider.background.width + 10;
        theSlider.label.y = Math.floor((theSlider.background.height - theSlider.label.height)/2);
        theSlider.value_text.y = theSlider.label.y;
        if (theSlider.label.width > myTransform.slider_label_maxwidth) {
            myTransform.slider_label_maxwidth = theSlider.label.width;
        }
        if (sliderCount % MAX_NUMBER_SLIDERS_PER_COLUMN == 0) {
            myTransform.slider_x_position += myTransform.slider_label_maxwidth + theSlider.background.width +  GAP*5;
            myTransform.slider_label_maxwidth = 0;
        }
    };

    var updateProperty = function (theEvent) {
        var mySliderInfo = _mySliders[theEvent.id];
        if (!mySliderInfo) {
            Logger.error("could not find slider : " + theEvent.id);
        }
        var myValue = theEvent.sliderX;
        myValue = lerp(myValue, mySliderInfo.range[0], mySliderInfo.range[1]);
        setProperty(mySliderInfo, myValue);
    };

    var setProperty = function (theSliderInfo, theValue) {
        var myPrecision = theSliderInfo.slider.precision;
        theValue = theValue.toFixed(myPrecision);
        theSliderInfo.value = theValue;
        var mySlider = theSliderInfo.slider;
        mySlider.setRelativeCursorPosition(theSliderInfo.getRelative(theValue));
        if (theSliderInfo.setter) {
            theSliderInfo.setter(theSliderInfo.object, theSliderInfo.property, theValue);
        } else if (theSliderInfo.property) {
            theSliderInfo.object[theSliderInfo.property] = theValue;
        } else  {
            theSliderInfo.object = theValue;
        }
        theSliderInfo.updateValueText(theValue);
        updateSettings(theSliderInfo, theValue);
    };

    var appendPropertyToSettings = function (theTab, theGroup, theName, theValue) {
        var myTab = _mySettings[theTab.name];
        if(!myTab) {
            var myTab = {};
            _mySettings[theTab.name] = myTab;
        }
        if (theGroup) {
            var myGroup = myTab[theGroup.name];
            if(!myGroup) {
                var myGroup = {};
                myTab[theGroup.name] = myGroup;
            }
        }
        if (theName) {
            if (theGroup) {
                myGroup[theName] = theValue;
            } else {
                myTab[theName] = theValue;
            }
        }
        //writeStringToFile(_mySettingsFile, JSON.stringify(_mySettings, null, "\t"));
    };

    var updateSettings = function (theSliderInfo, theValue) {
        if (theSliderInfo.group) {
            _mySettings[theSliderInfo.tab.name][theSliderInfo.group.name][theSliderInfo.settingskey] = theValue;
        } else {
            _mySettings[theSliderInfo.tab.name][theSliderInfo.settingskey] = theValue;
        }
    };

    var checkFadeOut = function () {
        if (!Public.visible) {
            return;
        }
        if (!_myAutoHideFlag) {
            return;
        }
        if (_myFadeOutAnimation) {
            _myFadeOutAnimation.cancel();
        }
        playAnimation(_myFadeOutAnimation);
    }

    var handleKeys = function (theEvent) {
        if ((theEvent.modifiers & spark.Keyboard.CTRL) || (theEvent.modifiers & spark.Keyboard.SHIFT)) {
            return;
        }
        if (theEvent.key === "space") {
            Public.visible = !Public.visible;
        }
        if (!Public.visible) {
            return;
        }
        if (theEvent.key === "s") {
            Logger.warning("saving current settings to file: " + _mySettingsFile);
            writeStringToFile(_mySettingsFile, JSON.stringify(_mySettings, null, "\t"));
        }
        if (theEvent.key === "r") {
            writeStringToFile(_mySettingsFile, JSON.stringify(_myRestoredSettings, null, "\t"));
            _mySettings = extend(_mySettings, _myRestoredSettings);
            for (slider in _mySliders) {
                var s = _mySliders[slider];
                var myValue;
                if (s.group) {
                    myValue = parseFloat(_mySettings[s.tab.name][s.group.name][s.settingskey]);
                } else {
                    myValue = parseFloat(_mySettings[s.tab.name][s.settingskey]);
                }
                setProperty(s, myValue);
                var myRelativePosition = s.getRelative(myValue);
                s.slider.setRelativeCursorPosition(myRelativePosition);
            }
        }
    };

};
