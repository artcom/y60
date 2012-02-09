/*jslint nomen:false white:false plusplus:false, bitwise:false*/
/*globals Node, print, Logger, Exception, fileline, stringToArray*/

var BaseConfigurator = function BaseConfigurator(theSceneViewer) {
    this.Constructor(this, {}, theSceneViewer);
};

BaseConfigurator.create = function(theSceneViewer, theOptionsString) {
    return new BaseConfigurator(theSceneViewer);
};

BaseConfigurator.prototype.Constructor = function (Public, Protected, theSceneViewer) {
    var _ = {};
    
    ///////////////////////
    // Private Variables //
    ///////////////////////
    
    _.listeners = [];
    _.currentSection = null;
    _.currentSetting = null;
    _.myKeyDown        = null;
    _.mySceneViewer    = theSceneViewer;
    
    //////////////////////
    // Public Variables //
    //////////////////////

    /////////////////////
    // Private Methods //
    /////////////////////
    
    _.nextNode = function (theNode) {
        var myNode = theNode;
        do {
            if (myNode.nextSibling) {
                myNode = myNode.nextSibling;
            } else {
                myNode = myNode.parentNode.firstChild;
            }
            if (_.isValidNode(myNode)) {
                return myNode;
            }
        } while (myNode !== theNode);
        return theNode;
    };

    _.previousNode = function (theNode) {
        var myNode = theNode;
        do {
            if (myNode.previousSibling) {
                myNode = myNode.previousSibling;
            } else {
                myNode = myNode.parentNode.lastChild;
            }
            if (_.isValidNode(myNode)) {
                return myNode;
            }
        } while (myNode !== theNode);
        return theNode;
    };
    
    _.displayMessage = function (theFirstLine, theSecondLine, theThirdLine) {
        if (!theSecondLine) {
            theSecondLine = "";
        }

        if (!theThirdLine) {
            theThirdLine = "";
        }

        _.mySceneViewer.setMessage("", 0);
        _.mySceneViewer.setMessage(theFirstLine, 1);
        _.mySceneViewer.setMessage(theSecondLine, 2);
        _.mySceneViewer.setMessage(theThirdLine, 3);
    };

    _.updateOnScreenDisplay = function () {
        _.displayMessage(_.currentSection.nodeName,
                         _.currentSetting.name() + " = " + _.currentSetting.value(),
                         _.currentSetting.help());
    };
    
    _.isValidNode = function (theNode) {
        if (!theNode) {
            return false;
        }
        if ((theNode.nodeType === Node.ELEMENT_NODE)) { /* && theNode.firstChild && (theNode.firstChild.nodeType == Node.TEXT_NODE)*/
            return true;
        } else {
            return false;
        }
    };

    _.printHelp = function () {
        print("Setting Manager Keys:");
        print("  left   next Setting");
        print("  right  previous Setting");
        print("   up    increase Setting value");
        print("  down   decrease Setting value");
        print("  pageup previous section");
        print("  pagedown next section");
        print("   tab   save Settings to file");
        print("shift-tab restore original Settings");
    };
    
    // Inner Class
    _.Setting = function Setting(theNode) {
        var _myNode       = theNode;
        var _myArrayPos   = null;
        var _myValue      = null;
        var _myArrayFlag  = null;
        var _myArray      = null;
        var _myStringFlag = null;

        this.setup = function () {
            _myArrayPos  = 0;
            var myValue;
            if (_myNode.firstChild) {
                if (_myNode.firstChild.nodeType !== Node.TEXT_NODE) {
                    _myValue = null;
                    Logger.warning("Node " + _myNode.nodeName + " has no value");
                    return;
                }
                _myValue      = _myNode.firstChild.nodeValue;
                _myArrayFlag  = (_myValue[0] === "[");
                _myArray      = stringToArray(_myValue);
                if (_myArrayFlag) {
                    _myStringFlag = false;
                    for (myValue in _myArray) {
                        _myStringFlag |= isNaN(Number(_myArray[0]));
                    }
                } else {
                    _myStringFlag = isNaN(Number(_myValue));
                }
            }
        };

        this.name = function () {
            var myName = _myNode.nodeName;
            if (_myArrayFlag) {
                myName += "[" + _myArrayPos + "]";
            }
            return myName;
        };

        this.value = function () {
            if (_myArrayFlag) {
                return Number(_myArray[_myArrayPos]);
            } else {
                if (_myStringFlag) {
                    return _myValue;
                } else {
                    return Number(_myValue);
                }
            }
        };

        this.help = function () {
            if ("help" in _myNode) {
                return _myNode.help;
            } else {
                return "";
            }
        };

        this.next = function () {
            if (_myArrayFlag && _myArrayPos < _myArray.length - 1) {
                _myArrayPos++;
            } else {
                _myNode = _.nextNode(_myNode);
                this.setup();
            }
        };

        this.previous = function () {
            if (_myArrayFlag && _myArrayPos > 0) {
                _myArrayPos--;
            } else {
                _myNode = _.previousNode(_myNode);
                this.setup();
                if (_myArrayFlag) {
                    _myArrayPos = _myArray.length - 1;
                }
            }
        };

        this.increment = function (theFastFlag) {
            theFastFlag = !!theFastFlag;
            if (_myStringFlag) {
                return;
            }

            var myValue = Number(_myValue);
            if (_myArrayFlag) {
                myValue = Number(_myArray[_myArrayPos]);
            }
            var myStep = 1;
            if ("step" in _myNode) {
                myStep  = Number(_myNode.step);
            }
            if (theFastFlag) {
                myStep *= 5;
            }

            myValue += myStep;

            if ("max" in _myNode) {
                var myMax   = Number(_myNode.max);
                if (myValue > myMax) {
                    myValue = myMax;
                }
            }
            myValue = myValue.toFixed(5);
            if (_myArrayFlag) {
                _myArray[_myArrayPos] = myValue;
                myValue = "[" + _myArray.join(",") + "]";
            } else {
                _myValue = myValue;
            }
            _myNode.firstChild.nodeValue = myValue;
        };

        this.decrement = function (theFastFlag) {
            theFastFlag = !!theFastFlag;

            if (_myStringFlag) {
                return;
            }

            var myValue = Number(_myValue);
            if (_myArrayFlag) {
                myValue = Number(_myArray[_myArrayPos]);
            }
            var myStep = 1;
            if ("step" in _myNode) {
                myStep  = Number(_myNode.step);
            }
            if (theFastFlag) {
                myStep *= 5;
            }
            myValue -= myStep;
            if ("min" in _myNode) {
                var myMin   = Number(_myNode.min);
                if (myValue < myMin) {
                    myValue = myMin;
                }
            }
            myValue = myValue.toFixed(5);
            if (_myArrayFlag) {
                _myArray[_myArrayPos] = myValue;
                myValue = "[" + _myArray.join(",") + "]";
            } else {
                _myValue = myValue;
            }
            _myNode.firstChild.nodeValue = myValue;
        };

        if (!_.isValidNode(_myNode)) {
            _myNode = _.nextNode(_myNode);
        } else {
            this.setup();
        }
    };
    
    /////////////////////////
    // Protected Variables //
    /////////////////////////
    
    Protected.settings = null; // TODO rename
    
    ///////////////////////
    // Protected Methods //
    ///////////////////////
    
    Protected.restoreSettings = function () {
        Logger.warning("<BaseConfigurator::restoreSettings> -- implement me");
    };
    
    Protected.saveSettings = function () {
        Logger.warning("<BaseConfigurator::saveSettings> -- implement me");
    };
    
    Protected.notifyListeners = function () {
        var i, myListener;
        for (i = 0; i < _.listeners.length; ++i) {
            myListener = _.listeners[i];
            if (myListener.section) {
                if (myListener.section === _.currentSection.nodeName) {
                    myListener.obj.onUpdateSettings(Protected.settings.childNode(myListener.section));
                }
            } else {
                myListener.obj.onUpdateSettings(Protected.settings);
            }
        }
    };
    
    Protected.setFirstSetting = function() {
        _.currentSection = Protected.settings.firstChild;
        _.currentSetting = new _.Setting(_.currentSection.firstChild);
    };
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    Public.addListener = function (theListener, theSection) {
        if (!Protected.settings) {
            return;
        }
        var mySection;
        if (theSection) {
            mySection = Protected.settings.childNode(theSection);
            if (!mySection) {
                Logger.warning("Section " + theSection + " does not exist.");
                return;
            }
            theListener.onUpdateSettings(mySection);
        } else {
            theListener.onUpdateSettings(Protected.settings);
        }
        _.listeners.push({
            obj     : theListener,
            section : theSection
        });
    };
    
    Public.removeListener = function (theListener) {
        var i;
        for (i = _.listeners.length - 1; i >= 0; --i) {
            if (theListener === _.listeners[i].obj) {
                _.listeners.splice(i, 1);
            }
        }
    };
    
    Public.onFrame = function (theTime) {
        if (_.myKeyDown && (theTime - _.myKeyDown.time) > 0.2) {
            Public.onKey(_.myKeyDown.key, true, _.myKeyDown.shift, _.myKeyDown.ctrl, _.myKeyDown.alt);
        }
    };

    Public.onKey = function (theKey, theKeyState, theShiftFlag, theCtrlFlag, theAltFlag) {
        if (!Protected.settings) {
            return;
        }
        if (!theKeyState) {
            _.myKeyDown = null;
            return;
        }
        
        _.myKeyDown = {  key: theKey, 
                        shift: theShiftFlag, 
                        ctrl: theCtrlFlag, 
                        alt: theAltFlag, 
                        time: _.mySceneViewer.getCurrentTime()};
        
        if (!theCtrlFlag) {
            return;
        }
        
        switch (theKey) {
        case "up":
            _.currentSetting.increment(theShiftFlag);
            Protected.notifyListeners();
            _.updateOnScreenDisplay();
            break;
        case "down":
            _.currentSetting.decrement(theShiftFlag);
            Protected.notifyListeners();
            _.updateOnScreenDisplay();
            break;
        case "right":
            _.currentSetting.next();
            _.updateOnScreenDisplay();
            break;
        case "left":
            _.currentSetting.previous();
            _.updateOnScreenDisplay();
            break;
        case "\\":
        case "page up":
            _.currentSection = _.previousNode(_.currentSection);
            _.currentSetting = new _.Setting(_.currentSection.firstChild);
            _.updateOnScreenDisplay();
            break;
        case "]":
        case "page down":
            _.currentSection = _.nextNode(_.currentSection);
            _.currentSetting = new _.Setting(_.currentSection.firstChild);
            _.updateOnScreenDisplay();
            break;
        case "$":
        case "tab":
            if (theShiftFlag) {
                Protected.restoreSettings();
                _.displayMessage("Original Settings restored.");
            } else {
                Protected.saveSettings();
                _.displayMessage("Settings saved.");
            }
            break;
        case "h":
        case "H":
            _.printHelp();
            break;
        default:
            break;
        }
    };
    
    Public.getSettings = function () {
        if (Protected.settings) {
            return Protected.settings;
        } else {
            throw new Exception("No settings found", fileline());
        }
    };
};