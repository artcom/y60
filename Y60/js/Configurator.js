//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Configurator.js,v $
//   $Author: martin $
//   $Revision: 1.20 $
//   $Date: 2005/03/30 17:45:57 $
//
//
//=============================================================================

function Configurator(theSceneViewer, theSettingsFile) {
    this.Constructor(this, theSceneViewer, theSettingsFile);
}       

Configurator.prototype.Constructor = function(obj, theSceneViewer, theSettingsFile) {
    const DISPLAY_DURATION  = 3;
    const FADE_DURATION     = 1;
    const BOX_SIZE          = [600,100];
    
    obj.getSettings = function() {
        if (_myMergedSettings) {
            return _myMergedSettings;
        } else {
            throw new Exception("No settings found", fileline());
        }
    }

    obj.getSettingsFile = function() {
        if (_myCommonSettingsFile) {
            return _myCommonSettingsFile;
        } else {
            throw new Exception("No settingsfile found", fileline());
        }
    }

    obj.getHostSettingsFile = function() {
        if (_myHostSettingsFile) {
            return _myHostSettingsFile;
        } else {
            throw new Exception("No host specific settingsfile found", fileline());
        }
    }

    obj.setSettingsFile = function(theSettingsFile) {
        setup(obj, theSceneViewer, theSettingsFile);
    }

    obj.setSetting = function(theSection, theSetting, theValue, theHostFlag) {
        var mySection = _myMergedSettings.childNode(theSection);
        if (!mySection) {
            mySection = _myMergedSettings.appendChild(Node.createElement(theSection));
        }

        var mySetting = mySection.childNode(theSetting);
        if (!mySetting) {
            mySetting = mySection.appendChild(Node.createElement(theSetting));
        }

        mySetting.nodeValue = theValue;

        if (theHostFlag) {
            mySetting.override = "hostname";
        }
    }

    obj.saveSettings = function(theAdditionalCommonFile, theAdditionalHostFile, theCommonFlag, theHostFlag) {
        if (theCommonFlag == undefined) {
            theCommonFlag = true;
        }

        if (theHostFlag == undefined) {
            theHostFlag = true;
        }

        var myCommonSettings = _myMergedSettings.cloneNode(true);
        var myHostSettings   = _myMergedSettings.cloneNode(true);

        for (var i = _myMergedSettings.childNodesLength() - 1; i >= 0; --i) {
            var mySection         = _myMergedSettings.childNode(i);
            var mySectionOverride = "none";
            if ("override" in mySection) {
                mySectionOverride = mySection.override;
            }

            // Don't inherit sections from the host specific settings
            if (!_myOriginalCommonSettings.childNode(mySection.nodeName)) {
                var myCommonSection = myCommonSettings.childNode(i);
                myCommonSettings.removeChild(myCommonSection);
                continue;
            }

            for (var j = mySection.childNodesLength() - 1; j >= 0; --j) {
                var mySetting = mySection.childNode(j);
                var mySettingOverride = "none";
                if ("override" in mySetting) {
                    mySettingOverride = mySetting.override;
                } else {
                    mySettingOverride = mySectionOverride;
                }

                // Remove the override attribute
                var myHostSection = myHostSettings.childNode(i);
                var myHostSetting = myHostSection.childNode(j);
                myHostSetting     = removeAttributeByName(myHostSetting, "override");

                if (mySettingOverride == "hostname") {
                    var myOldCommonSection = myCommonSettings.childNode(mySection.nodeName);
                    var myOldCommonSetting = myOldCommonSection.childNode(mySetting.nodeName);
                    var myNewCommonSection = _myOriginalCommonSettings.childNode(mySection.nodeName);
                    var myNewCommonSetting = myNewCommonSection.childNode(mySetting.nodeName);
                    myOldCommonSection.replaceChild(myNewCommonSetting.cloneNode(true), myOldCommonSetting);
                } else if (mySettingOverride == "none") {
                    var myHostSection = myHostSettings.childNode(mySection.nodeName);
                    var myHostSetting = myHostSection.childNode(mySetting.nodeName);
                    myHostSection.removeChild(myHostSetting);
                } else {
                    Logger.warning("Unknown override setting '" + mySettingOverride + "'");
                }
            }

            // Remove empty sections
            var myCommonSection = myCommonSettings.childNode(i);
            if (myCommonSection.childNodesLength() == 0) {
                myCommonSettings.removeChild(myCommonSection);
            }
            var myHostSection = myHostSettings.childNode(i);
            if (myHostSection.childNodesLength() == 0) {
                myHostSettings.removeChild(myHostSection);
            } else {
                // Remove the override attribute
                removeAttributeByName(myHostSection, "override");
            }
        }

        if (theCommonFlag) {
            myCommonSettings.saveFile(_myCommonSettingsFile);
        }

        if (theHostFlag) {
            myHostSettings.saveFile(_myHostSettingsFile);
        }

        if (theAdditionalCommonFile) {
            myCommonSettings.saveFile(theAdditionalCommonFile);
        }

        if (theAdditionalHostFile) {
            myHostSettings.saveFile(theAdditionalHostFile);
        }
    }

    obj.removeListener = function(theListener) {
        for (var i=_myListeners.length-1; i >= 0; --i) {
            if (theListener == _myListeners[i].obj) {
                _myListeners.splice(i, 1);
            }
        }
    }
    
    obj.addListener = function(theListener, theSection) {
        if (!_myMergedSettings) {
            return;
        }
        
        if (theSection) {
            var mySection = _myMergedSettings.childNode(theSection);
            if (!mySection) {
                throw new Exception("Section " + theSection + " does not exist.", fileline());
            }
            
            theListener.onUpdateSettings(mySection);
        } else {
            theListener.onUpdateSettings(_myMergedSettings);
        }
        _myListeners.push({obj:theListener, section:theSection});
    }

    obj.onFrame = function(theTime) {
        if (_myKeyDown && (theTime - _myKeyDown.time) > 0.2) {
            obj.onKey(_myKeyDown.key, true, _myKeyDown.shift);
        }
    }

    obj.onKey = function(theKey, theKeyState, theShiftFlag) {
        if (!_myMergedSettings) {
            return;
        }
        if (!theKeyState) {
            _myKeyDown = null;
            return;
        }

        _myKeyDown = { key: theKey, shift: theShiftFlag, time: _mySceneViewer.getCurrentTime()};

        switch (theKey) {
            case "up":
                _myCurrentSetting.increment(theShiftFlag);
                notifyListeners();
                updateOnScreenDisplay();
                break;
            case "down":
                _myCurrentSetting.decrement(theShiftFlag);
                notifyListeners();
                updateOnScreenDisplay();
                break;
            case "right":
                _myCurrentSetting.next();
                updateOnScreenDisplay();
                break;
            case "left":
                _myCurrentSetting.previous();
                updateOnScreenDisplay();
                break;
            case "\\":
            case "page up":
                _myCurrentSection = previousNode(_myCurrentSection);
                _myCurrentSetting = new Setting(_myCurrentSection.firstChild);
                updateOnScreenDisplay();
                break;
            case "]":
            case "page down":
                _myCurrentSection = nextNode(_myCurrentSection);
                _myCurrentSetting = new Setting(_myCurrentSection.firstChild);
                updateOnScreenDisplay();
                break;
            case "$":
            case "tab":
                if (theShiftFlag) {
                    restoreSettings();
                    displayMessage("Original Settings restored.");
                } else {
                    obj.saveSettings();
                    displayMessage("Settings saved.");
                }
                break;
            case "h":
            case "H":
                printHelp();
                break;
        }
    }

    function Setting(theNode) {
        var _myNode      = theNode;
        var _myArrayPos  = null;
        var _myValue     = null;
        var _myArrayFlag = null;
        var _myArray     = null;

        function setup() {
            _myArrayPos  = 0;
            _myValue     = _myNode.firstChild.nodeValue;
            _myArrayFlag = (_myValue[0] == "[");
            _myArray     = stringToArray(_myValue);
        }
        
        this.name = function() {
            var myName = _myNode.nodeName
            if (_myArrayFlag) {
                myName += "[" + _myArrayPos + "]";
            }
            return myName;
        }
        
        this.value = function() {
            if (_myArrayFlag) {
                return Number(_myArray[_myArrayPos]);
            } else {
                return Number(_myValue);
            }
        }
        
        this.help = function() {
            if ("help" in _myNode) {
                return _myNode.help;   
            } else {
                return "";
            }            
        }

        this.next = function() {
            if (_myArrayFlag && _myArrayPos < _myArray.length - 1) {
                _myArrayPos++;
            } else {
                _myNode = nextNode(_myNode);
                setup();
            }
        }

        this.previous = function() {
            if (_myArrayFlag && _myArrayPos > 0) {
                _myArrayPos--;
            } else {
                _myNode = previousNode(_myNode);
                setup();
                if (_myArrayFlag) {
                    _myArrayPos = _myArray.length - 1;
                }
            }
        }

        this.increment = function(theFastFlag) {
            if (theFastFlag == undefined) {
                theFastFlag = false;
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
            print("step: " + myStep);
            print("myValue: " + myStep);
            
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
        }

        this.decrement = function(theFastFlag) {
            if (theFastFlag == undefined) {
                theFastFlag = false;
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
        }

        if (!isValidNode(_myNode)) {
            _myNode = nextNode(_myNode);
        } else {
            setup();
        }
    }

    function isValidNode(theNode) {
        if (!theNode) {
            return false;
        }
        return (theNode.nodeType == Node.ELEMENT_NODE);
    }

    function nextNode(theNode) {
        var myNode = theNode;
        do {
            if (myNode.nextSibling) {
                myNode = myNode.nextSibling;
            } else {
                myNode = myNode.parentNode.firstChild;
            }
            if (isValidNode(myNode)) {
                return myNode;
            }
        } while (myNode != theNode);
        return theNode;
    }

    function previousNode(theNode) {
        var myNode = theNode;
        do {
            if (myNode.previousSibling) {
                myNode = myNode.previousSibling;
            } else {
                myNode = myNode.parentNode.lastChild;
            }
            if (isValidNode(myNode)) {
                return myNode;
            }
        } while (myNode != theNode);
        return theNode;
    }

    function setup(obj, theSceneViewer, theSettingsFile) {
        _myCommonSettingsFile = theSettingsFile;
        if (fileExists(_myCommonSettingsFile)) {
            Logger.info("Parsing settings from '" + _myCommonSettingsFile + "'");
            var mySettingsDom = new Node();
            mySettingsDom.parseFile(_myCommonSettingsFile);
            _myOriginalCommonSettings = mySettingsDom.firstChild;

            // build hostname specific settings filename
            var myFileparts  = theSettingsFile.split(".");
            var myFileprefix = "";
            var myFilesuffix = "";
            if (myFileparts.length == 1) {
                myFileprefix = myFileparts[0];
            } else {
                myFileprefix = myFileparts.slice(0, myFileparts.length - 1).join("");
                myFilesuffix = "." + myFileparts[myFileparts.length - 1];
            }
            var myHostname      = hostname().split(".")[0]; // make sure we don't get a FQ name
            _myHostSettingsFile = myFileprefix + "-" + myHostname + myFilesuffix;

            _myMergedSettings         = mySettingsDom.firstChild.cloneNode(true);
            _myOriginalMergedSettings = _myMergedSettings.cloneNode(true);
            if (fileExists(_myHostSettingsFile)) {
                mergeHostSpecificSettings();
            }
    
            _myCurrentSection = _myMergedSettings.firstChild;
            _myCurrentSetting = new Setting(_myCurrentSection.firstChild);
        }
    }

    function notifyListeners() {
        for (var i = 0; i < _myListeners.length; ++i) {
            var myListener = _myListeners[i];
            if (myListener.section) {
                if (myListener.section == _myCurrentSection.nodeName) {
                    myListener.obj.onUpdateSettings(_myMergedSettings.childNode(myListener.section));
                }
            } else {
                myListener.obj.onUpdateSettings(_myMergedSettings);
            }
        }
    }

    function restoreSettings() {
        for (var i = 0; i < _myMergedSettings.childNodes.length; ++i) {
            var myChild = _myMergedSettings.childNode(i);
            for (var j = 0; j < myChild.childNodes.length; ++j) {
                myChild.childNode(j).firstChild.nodeValue = _myOriginalSettings.childNode(i).childNode(j).firstChild;
            }
        }
        notifyListeners();
    }

    function updateOnScreenDisplay() {
        displayMessage(_myCurrentSection.nodeName, _myCurrentSetting.name() + " = " + _myCurrentSetting.value(), _myCurrentSetting.help());
    }

    function displayMessage(theFirstLine, theSecondLine, theThirdLine) {
        if (!theSecondLine) {
            theSecondLine = "";
        }
        
        if (!theThirdLine) {
            theThirdLine = "";
        }
        
        _mySceneViewer.setMessage("", 0);
        _mySceneViewer.setMessage(theFirstLine, 1);
        _mySceneViewer.setMessage(theSecondLine, 2);
        _mySceneViewer.setMessage(theThirdLine, 3);
    }

    function printHelp() {
         print("Setting Manager Keys:");
         print("  left   next Setting");
         print("  right  previous Setting");
         print("   up    increase Setting value");
         print("  down   decrease Setting value");
         print("  pageup previous section");
         print("  pagedown next section");
         print("   tab   save Settings to file");
         print("shift-tab restore original Settings");
    }

    function mergeHostSpecificSettings() {
        Logger.info("Using host-specific settings from '" + _myHostSettingsFile + "'");

        var mySettingsDom = new Node();
        mySettingsDom.parseFile(_myHostSettingsFile);
        _myOriginalHostSettings = mySettingsDom.firstChild;

        for (var i = 0; i < _myOriginalHostSettings.childNodesLength(); ++i) {
            var myHostSection   = _myOriginalHostSettings.childNode(i);
            var myCommonSection = _myMergedSettings.childNode(myHostSection.nodeName);

            if (!myCommonSection) {
                myCommonSection = Node.createElement(myHostSection.nodeName);
                _myMergedSettings.appendChild(myCommonSection);
            }

            // Merge sections
            mergeHostSpecificSection(myCommonSection, myHostSection);
        }
    }

    function mergeHostSpecificSection(theCommonSection, theHostSection) {
        var myChildNodesLength = theHostSection.childNodesLength();
        for (var i = myChildNodesLength - 1; i >= 0; --i) {
            var myHostNode = theHostSection.childNode(i);

            myHostNode = removeAttributeByName(myHostNode, "override");

            var myCommonNode = theCommonSection.childNode(myHostNode.nodeName);
            if (myCommonNode) {
                // Use the attributes from the common node
                var myAttributes = myCommonNode.attributes;
                for (var j = 0; j < myAttributes.length; ++j) {
                    myHostNode[myAttributes[j].nodeName] = myCommonNode[myAttributes[j].nodeName];
                }

                // Replace the common node with host specific node
                theCommonSection.replaceChild(myHostNode.cloneNode(true), myCommonNode);
            } else {
                // Add node to config if it does not exist
                theCommonSection.appendChild(myHostNode.cloneNode(true));
            }
        }
    }

    var _mySceneViewer            = theSceneViewer;
    var _myMergedSettings         = null;
    var _myOriginalMergedSettings = null;
    var _myOriginalCommonSettings = null;
    var _myOriginalHostSettings   = null;
    var _myCommonSettingsFile     = null;
    var _myHostSettingsFile       = null;
    var _myCurrentSection         = null;
    var _myCurrentSetting         = null;
    var _myListeners              = [];
    var _myKeyDown                = null;

    setup(obj, theSceneViewer, theSettingsFile);
}

//
// A generic XML-to-JavaScript converter.
//
// Usage:
// (1) Setup: 
// self.registerSettingsListener(ourSettings, "MySettingsNode");
// (2) Access: 
// var myValue = ourSettings.value;
//

function Settings() {
    this.onUpdateSettings = function(theNode) {
        for (var i = 0; i < theNode.childNodes.length; ++i) {
            var myChild = theNode.childNodes[i];
            this[myChild.nodeName] = myChild.firstChild.nodeValue;                
        }
    }
}

var ourSettings = (ourSettings == undefined) ? new Settings() : ourSettings;
