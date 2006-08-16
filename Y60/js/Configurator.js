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
    
    function Setting(theNode) {
        var _myNode = theNode;
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

        function isValidNode(theNode) {
            return (theNode.nodeType == Node.ELEMENT_NODE);
        }

        function nextNode() {
            var myNode = null;
            while (!myNode || (!isValidNode(myNode) && !myNode.isSameNode(_myNode))) {
                if (!myNode) {
                    myNode = _myNode;
                }
                if (myNode.nextSibling) {
                    myNode = myNode.nextSibling;
                } else {
                    myNode = myNode.parentNode.firstChild;
                }
            }
            return myNode;
        }
        function previousNode() {
            var myNode = null;
            while (!myNode || (!isValidNode(myNode) && !myNode.isSameNode(_myNode))) {
                if (!myNode) {
                    myNode = _myNode;
                }
                if (myNode.previousSibling) {
                    myNode = myNode.previousSibling;
                } else {
                    myNode = myNode.parentNode.lastChild;
                }
            }
            return myNode;
        }

        this.next = function() {
            if (_myArrayFlag && _myArrayPos < _myArray.length - 1) {
                _myArrayPos++;
            } else {
                _myNode = nextNode();
                setup();
            }
        }

        this.previous = function() {
            if (_myArrayFlag && _myArrayPos > 0) {
                _myArrayPos--;
            } else {
                _myNode = previousNode();
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
            _myNode = nextNode();
        } else {
            setup();
        }
    }

    var _mySceneViewer      = theSceneViewer;
    var _mySettings         = null;
    var _mySettingsFile     = null;
    var _myOriginalSettings = null;
    var _myCurrentSection   = null;
    var _myCurrentSetting   = null;
    var _myListeners        = [];
    var _myKeyDown          = null;

    setup(obj, theSceneViewer, theSettingsFile);


    function setup(obj, theSceneViewer, theSettingsFile) {
        _mySettingsFile = theSettingsFile;
        if (fileExists(_mySettingsFile)) {
            print("Parsing settings from '" + _mySettingsFile + "'");
            var mySettingsDom = new Node();
            mySettingsDom.parseFile(_mySettingsFile);
            _mySettings = mySettingsDom.firstChild;
            var myHostSettingsFile = "settings-" + hostname() + ".xml";
            if (fileExists(myHostSettingsFile)) {
                print("Merging settings with '" + myHostSettingsFile + "'");
                mergeHostSpecificSettings(myHostSettingsFile);
            }
    
            _myCurrentSection   = _mySettings.firstChild;
            _myCurrentSetting   = new Setting(_myCurrentSection.firstChild);
            _myOriginalSettings = _mySettings.cloneNode(true);
        }
    }

    obj.getSettings = function() {
        if (_mySettings) {
            return _mySettings;
        } else {
            throw new Exception("No settings found (Configurator.js)", fileline());
        }
    }

    obj.setSettingsFile = function(theSettingsFile) {
        setup(obj, theSceneViewer, theSettingsFile);
    }

    obj.removeListener = function(theListener) {
        for (var i=_myListeners.length-1; i >= 0; --i) {
            if (theListener == _myListeners[i].obj) {
                _myListeners.splice(i, 1);
            }
        }
    }
    
    obj.addListener = function(theListener, theSection) {
        if (!_mySettings) {
            return;
        }
        
        if (theSection) {
            var mySection = _mySettings.childNode(theSection);
            if (!mySection) {
                throw new Exception("Section " + theSection + " does not exist in " + _mySettingsFile, fileline());
            }
            
            theListener.onUpdateSettings(mySection);
        } else {
            theListener.onUpdateSettings(_mySettings);
        }
        _myListeners.push({obj:theListener, section:theSection});
    }

    obj.onFrame = function(theTime) {
        if (_myKeyDown && (theTime - _myKeyDown.time) > 0.2) {
            obj.onKey(_myKeyDown.key, true, _myKeyDown.shift);
        }
    }

    obj.onKey = function(theKey, theKeyState, theShiftFlag) {
        if (!_mySettings) {
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
                if (_myCurrentSection.previousSibling) {
                    _myCurrentSection = _myCurrentSection.previousSibling;
                } else {
                    _myCurrentSection = _myCurrentSection.parentNode.lastChild;
                }
                _myCurrentSetting = new Setting(_myCurrentSection.firstChild);
                updateOnScreenDisplay();
                break;
            case "]":
            case "page down":
                if (_myCurrentSection.nextSibling) {
                    _myCurrentSection = _myCurrentSection.nextSibling;
                } else {
                    _myCurrentSection = _myCurrentSection.parentNode.firstChild;
                }
                _myCurrentSetting = new Setting(_myCurrentSection.firstChild);
                updateOnScreenDisplay();
                break;
            case "$":
            case "tab":
                if (theShiftFlag) {
                    restoreSettings();
                } else {
                    _mySettings.saveFile(_mySettingsFile);
                    displayMessage("Settings saved to " + _mySettingsFile);
                }
                break;
            case "h":
            case "H":
                printHelp();
                break;
        }
    }

    function notifyListeners() {
        for (var i = 0; i < _myListeners.length; ++i) {
            var myListener = _myListeners[i];
            if (myListener.section) {
                if (myListener.section == _myCurrentSection.nodeName) {
                    myListener.obj.onUpdateSettings(_mySettings.childNode(myListener.section));
                }
            } else {
                myListener.obj.onUpdateSettings(_mySettings);
            }
        }
    }

    function restoreSettings() {
        for (var i = 0; i < _mySettings.childNodes.length; ++i) {
            var myChild = _mySettings.childNode(i);
            for (var j = 0; j < myChild.childNodes.length; ++j) {
                myChild.childNode(j).firstChild.nodeValue = _myOriginalSettings.childNode(i).childNode(j).firstChild;
            }
        }
        notifyListeners();
        displayMessage("Original Settings restored.");
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

    function mergeHostSpecificSettings(theSettingsFile) {
        Logger.info("Using host-specific settings from '" + theSettingsFile + "'");

        var mySettingsDom = new Node();
        mySettingsDom.parseFile(theSettingsFile);
        var myHostSettings = mySettingsDom.firstChild;

        for (var i = 0; i < myHostSettings.childNodes.length; ++i) {
            var myHostSection = myHostSettings.childNode(i);
            var myCommonSection = _mySettings.childNode(myHostSection.nodeName);

            if (myCommonSection) {
                // Merge section if it exits
                mergeHostSpecificSection(myCommonSection, myHostSection);
            } else {
                // Add section to config if it does not exits there
                _mySettings.appendChild(myHostSection.cloneNode(true));
            }
        }
    }

    function mergeHostSpecificSection(theCommonSection, theHostSection) {
        for (var i = 0; i < theHostSection.childNodes.length; ++i) {
            var myHostNode = theHostSection.childNode(i);
            var myCommonNode = theCommonSection.childNode(myHostNode.nodeName);
            if (myCommonNode) {
                // Replace common node with host specific node
                theCommonSection.replaceChild(myHostNode.cloneNode(true), myCommonNode);
            } else {
                // Add node to config if it does not exits there
                theCommonSection.appendChild(myHostNode.cloneNode(true));
            }
        }
    }

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
