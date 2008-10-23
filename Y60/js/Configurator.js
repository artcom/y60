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

use("Y60JSSL.js");

function Configurator(theSceneViewer, theSettingsFile, theSettingsFileList) {
    this.Constructor(this, theSceneViewer, theSettingsFile, theSettingsFileList);
}       

Configurator.prototype.Constructor = function( obj, theSceneViewer, theSettingsFile, 
                                               theSettingsFileList) 
{
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

    obj.setSettingsFile = function(theSettingsFile) {
        setup(obj, theSceneViewer, theSettingsFile);
    }

    obj.setSettingsFileList = function( theFileList ) {
        _mySettingsFileList = theFileList;
        setup( obj, theSceneViewer, _myCommonSettingsFile );
    }

    obj.getSettingsFileList = function() {
        return _mySettingsFileList;
    }

    obj.hasSection = function(theSection) {
        return _myMergedSettings.childNode(theSection) != null;
    }

    obj.setSetting = function(theSection, theSetting, theValue) {
        var mySection = _myMergedSettings.childNode(theSection);
        if (!mySection) {
            mySection = _myMergedSettings.appendChild(Node.createElement(theSection));
        }

        var mySetting = mySection.childNode(theSetting);
        if (!mySetting) {
            mySetting = mySection.appendChild(Node.createElement(theSetting));
        }

        mySetting.firstChild.nodeValue = theValue;
    }

    obj.restoreSettings = function() {
        for (var i = 0; i < _myMergedSettings.childNodes.length; ++i) {
            var myChild = _myMergedSettings.childNode(i);
            for (var j = 0; j < myChild.childNodes.length; ++j) {
                myChild.childNode(j).firstChild.nodeValue = 
                    _myOriginalMergedSettings.childNode(i).childNode(j).firstChild;
            }
        }
        notifyListeners();
    }

    obj.reloadSettings = function() {
        setup( obj, theSceneViewer, _myCommonSettingsFile );
        notifyListeners();
    }
    
    function saveSection( theSourceSection, theTargetSection ) {
        // save common setting values
        for (var i = 0; i < theTargetSection.childNodesLength(); i++) {
            var myTargetNode = theTargetSection.childNode( i );
            var mySourceNode = theSourceSection.childNode( myTargetNode.nodeName );
            if (mySourceNode) {
                theTargetSection.replaceChild( mySourceNode.cloneNode(true), myTargetNode );
                // remove them from merged section
                theSourceSection.removeChild( mySourceNode ); 
            }
        }
    }

    function saveSetting( theSetting, theSettingsFile, theCurrentMergedSettings ) {

        for (var j = 0; j < theSetting.childNodesLength(); j++) {
            var myCurrentSection = theSetting.childNode( j );    
            var myMergedSection = 
                theCurrentMergedSettings.childNode( myCurrentSection.nodeName );
            if ( myMergedSection ) {
                saveSection( myMergedSection, myCurrentSection );
                if (!myMergedSection.hasChildNodes()) {
                    theCurrentMergedSettings.removeChild( myMergedSection );
                }
            }
        } 
        theSetting.saveFile( theSettingsFile );
    }

    obj.saveSettings = function() {
        var myCommonSettingsFileSavedFlag = false;
        var myCurrentMergedSettings = _myMergedSettings.cloneNode( true ); 
        for (var i = _mySettingsList.length - 1; i >= 0; i--) { 
            var myCurrentSettings = _mySettingsList[i];
            saveSetting( myCurrentSettings, _mySettingsFileList[i], 
                         myCurrentMergedSettings );
            if(_mySettingsFileList[i] == _myCommonSettingsFile) {
                myCommonSettingsFileSavedFlag = true;
            }
        }
        if(!myCommonSettingsFileSavedFlag) {
            saveSetting( _myOriginalCommonSettings, _myCommonSettingsFile, 
                         myCurrentMergedSettings );
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
                    obj.restoreSettings();
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
            if(_myNode.firstChild) {
                if(_myNode.firstChild.nodeType != Node.TEXT_NODE) {
                    _myValue = null;
                    Logger.warning("Node " + _myNode.nodeName + " has no value");
                    return;
                }
                _myValue     = _myNode.firstChild.nodeValue;
                _myArrayFlag = (_myValue[0] == "[");
                _myArray     = stringToArray(_myValue);
            }
            
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
        if ((theNode.nodeType == Node.ELEMENT_NODE)/* && theNode.firstChild && (theNode.firstChild.nodeType == Node.TEXT_NODE)*/) {
            return true;
        } else {
            return false;
        }
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

    function setup(obj, theSceneViewer, theSettingsFile, theSettingsFileList) {

        if (theSettingsFileList != undefined) {
            _mySettingsFileList = theSettingsFileList;
        }

        _myCommonSettingsFile = theSettingsFile;
        if (fileExists(_myCommonSettingsFile)) {
            Logger.info("Parsing settings from '" + _myCommonSettingsFile + "'");
            var myCommonSettingsDom = new Node();
            myCommonSettingsDom.parseFile(_myCommonSettingsFile);
            _myOriginalCommonSettings = myCommonSettingsDom.firstChild;

            _myMergedSettings = myCommonSettingsDom.firstChild.cloneNode(true);

            // merge all settings in the settings file list
            for (var i = 0; i < _mySettingsFileList.length; i++) {
                var mySettingsFile = _mySettingsFileList[i];
                if (fileExists(mySettingsFile)) {
                    var mySettingsDom = new Node();
                    mySettingsDom.parseFile( mySettingsFile );
                    var mySettings = mySettingsDom.firstChild;
                    _mySettingsList.push( mySettings );
                    Logger.info("Using settings from '" + mySettingsFile + "'");
                    mergeSettings( mySettings );
                } else {
                    Logger.warning( 'Settings file "' + mySettingsFile 
                                    + '" does not exist!');
                }
            }

            _myOriginalMergedSettings = _myMergedSettings.cloneNode( true );
            // set default current settings
            _myCurrentSection = _myMergedSettings.firstChild;
            _myCurrentSetting = new Setting(_myCurrentSection.firstChild);
            
        } else {
            // XXX todo: no settings.xml -> no configurator?
            //throw new Exception( "Settings file " + _myCommonSettingsFile + 
            //                     "does not exist!", fileline() );
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

    function mergeSettings( theSettings ) {

        for (var i = 0; i < theSettings.childNodesLength(); i++) {
            var mySection = theSettings.childNode( i );
            var myCommonSection = _myMergedSettings.childNode( mySection.nodeName );

            if (!myCommonSection) {
                myCommonSection = Node.createElement( mySection.nodeName );
                _myMergedSettings.appendChild( myCommonSection );
            }
            mergeSection( myCommonSection, mySection );
        }
        
    }

    function mergeSection(theCommonSection, theSection) {
        var myChildNodesLength = theSection.childNodesLength();
        for (var i = myChildNodesLength - 1; i >= 0; --i) {
            var myNode = theSection.childNode(i);

            var myCommonNode = theCommonSection.childNode(myNode.nodeName);
            if (myCommonNode) {
                // Use the attributes from the common node
                var myAttributes = myCommonNode.attributes;
                for (var j = 0; j < myAttributes.length; ++j) {
                    myNode[myAttributes[j].nodeName] = 
                        myCommonNode[myAttributes[j].nodeName];
                }
                // Replace the common node with new node
                theCommonSection.replaceChild(myNode.cloneNode(true), myCommonNode);
            } else {
                // Add node to config if it does not exist
                theCommonSection.appendChild(myNode.cloneNode(true));
            }
        }
    }

    var _mySceneViewer            = theSceneViewer;
    var _myMergedSettings         = null;   
    var _myOriginalCommonSettings = null;
    var _myOriginalMergedSettings = null;
    var _myCommonSettingsFile     = null;
    var _mySettingsFileList       = [];
    var _mySettingsList           = [];
    var _myCurrentSection         = null;
    var _myCurrentSetting         = null;
    var _myListeners              = [];
    var _myKeyDown                = null;

    setup( obj, theSceneViewer, theSettingsFile, theSettingsFileList );
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
