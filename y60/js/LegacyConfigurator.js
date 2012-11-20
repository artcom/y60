/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//   $RCSfile: Configurator.js,v $
//   $Author: martin $
//   $Revision: 1.20 $
//   $Date: 2005/03/30 17:45:57 $
//
//
//=============================================================================

/*jslint nomen:false, plusplus:false, bitwise:false*/
/*globals use, fileline, Exception, Node, Logger, print, trim, stringToArray,
          fileExists, hostname, BaseConfigurator*/

use("BaseConfigurator.js");
use("Y60JSSL.js");

function LegacyConfigurator(theSceneViewer, theSettingsFile, theSettingsFileList) {
    this.Constructor(this, {}, theSceneViewer, theSettingsFile, theSettingsFileList);
}

LegacyConfigurator.SETTINGS_FILE_NAMES = ["settings.xml", "CONFIG/settings.xml"];

LegacyConfigurator.create = function (theSceneViewer, theOptionsString) {
    // look for settings file
    var mySettingsFile = "";
    var i;
    for (i = 0; i < LegacyConfigurator.SETTINGS_FILE_NAMES.length; ++i) {
        if (fileExists(LegacyConfigurator.SETTINGS_FILE_NAMES[i])) {
            mySettingsFile = LegacyConfigurator.SETTINGS_FILE_NAMES[i];
            break;
        }
    }

    // look for host dependent settings file
    var myHostSettingsFile = "";
    var myHostFiles = ["settings-" + hostname() + ".xml",
                       "CONFIG/settings-" + hostname() + ".xml"];
    for (i = 0; i < myHostFiles.length; ++i) {
        if (fileExists(myHostFiles[i])) {
            myHostSettingsFile = myHostFiles[i];
            break;
        }
    }

    // look for command line argument of settings file(s)
    // e.g. "settings mysettings.xml:moresettings.xml"
    var mySettingsList;
    if (theOptionsString) {
        mySettingsList = theOptionsString.split(":");
    }

    // add host name dependent settings file to the beginning of the file list
    if (myHostSettingsFile !== "") {
        if (mySettingsList === undefined) {
            mySettingsList = [myHostSettingsFile];
        } else {
            mySettingsList.unshift(myHostSettingsFile);
        }
    }

    if (mySettingsFile !== "" || mySettingsList !== undefined) {
        return new LegacyConfigurator(theSceneViewer, mySettingsFile, mySettingsList);
    }
    return null;
};

LegacyConfigurator.prototype.Constructor = function (obj, Protected, theSceneViewer, theSettingsFile,
                                               theSettingsFileList) {
    BaseConfigurator.prototype.Constructor(obj, Protected, theSceneViewer);
    
    /////////////////////
    // Private Members //
    /////////////////////

    var _myOriginalCommonSettings = null;
    var _myOriginalMergedSettings = null;
    var _myCommonSettingsFile     = null;
    var _mySettingsFileList       = [];
    var _mySettingsList           = [];
    
    /////////////////////
    // Private Methods //
    /////////////////////
    
    function saveSection(theSourceSection, theTargetSection) {
        var i, myTargetNode, mySourceNode;
        // save common setting values
        for (i = 0; i < theTargetSection.childNodesLength(); i++) {
            myTargetNode = theTargetSection.childNode(i);
            mySourceNode = theSourceSection.childNode(myTargetNode.nodeName);
            if (mySourceNode) {
                theTargetSection.replaceChild(mySourceNode.cloneNode(true), myTargetNode);
                // remove them from merged section
                theSourceSection.removeChild(mySourceNode);
            }
        }
    }

    function saveSetting(theSetting, theSettingsFile, theCurrentMergedSettings) {
        var j, myCurrentSection, myMergedSection;
        for (j = 0; j < theSetting.childNodesLength(); j++) {
            myCurrentSection = theSetting.childNode(j);
            myMergedSection = theCurrentMergedSettings.
                                  childNode(myCurrentSection.nodeName);
            if (myMergedSection) {
                saveSection(myMergedSection, myCurrentSection);
                if (!myMergedSection.hasChildNodes()) {
                    theCurrentMergedSettings.removeChild(myMergedSection);
                }
            }
        }
        theSetting.saveFile(theSettingsFile);
    }

    function mergeSection(theCommonSection, theSection) {
        var myChildNodesLength = theSection.childNodesLength();
        var i, j, myNode, myCommonNode;
        for (i = myChildNodesLength - 1; i >= 0; --i) {
            myNode = theSection.childNode(i);

            myCommonNode = theCommonSection.childNode(myNode.nodeName);
            if (myCommonNode) {
                // Use the attributes from the common node
                var myAttributes = myCommonNode.attributes;
                for (j = 0; j < myAttributes.length; ++j) {
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
    
    function mergeSettings(theSettings) {
        var i, mySection, myCommonSection;
        for (i = 0; i < theSettings.childNodesLength(); i++) {
            mySection = theSettings.childNode(i);
            myCommonSection = Protected.settings.childNode(mySection.nodeName);

            if (!myCommonSection) {
                myCommonSection = Node.createElement(mySection.nodeName);
                Protected.settings.appendChild(myCommonSection);
            }
            mergeSection(myCommonSection, mySection);
        }
    }
    
    function setup(obj, theSceneViewer, theSettingsFile, theSettingsFileList) {
        var i, myCommonSettingsDom, mySettingsFile, mySettings, mySettingsDom;
        _mySettingsFileList = theSettingsFileList || [];
        _myCommonSettingsFile = theSettingsFile;
        if (fileExists(_myCommonSettingsFile)) {
            Logger.info("<Configurator> Parsing settings from '" + _myCommonSettingsFile + "'");
            myCommonSettingsDom = new Node();
            myCommonSettingsDom.parseFile(_myCommonSettingsFile);
            _myOriginalCommonSettings = myCommonSettingsDom.firstChild;

            Protected.settings = myCommonSettingsDom.firstChild.cloneNode(true);

            // merge all settings in the settings file list
            for (i = 0; i < _mySettingsFileList.length; i++) {
                mySettingsFile = trim(_mySettingsFileList[i]);
                if (fileExists(mySettingsFile)) {
                    mySettingsDom = new Node();
                    mySettingsDom.parseFile(mySettingsFile);
                    mySettings = mySettingsDom.firstChild;
                    _mySettingsList.push(mySettings);
                    Logger.info("<Configurator> Using settings from '" + mySettingsFile + "'");
                    mergeSettings(mySettings);
                } else {
                    Logger.warning('<Configurator> Settings file "' + mySettingsFile +
                                   '" does not exist!');
                }
            }

            _myOriginalMergedSettings = Protected.settings.cloneNode(true);
            // set default current settings
            Protected.setFirstSetting();
        } else {
            if (_mySettingsFileList.length > 0) {
                Logger.warning("Not loading settings '[" + _mySettingsFileList.join(",") + "]'");
            }
        }
    }
    
    ////////////////////
    // Public Members //
    ////////////////////
    
    ///////////////////////
    // Protected Methods //
    ///////////////////////
    
    Protected.saveSettings = function () {
        var myCommonSettingsFileSavedFlag = false;
        var myCurrentMergedSettings = Protected.settings.cloneNode(true);
        var i, myCurrentSettings;
        for (i = _mySettingsList.length - 1; i >= 0; i--) {
            myCurrentSettings = _mySettingsList[i];
            saveSetting(myCurrentSettings, _mySettingsFileList[i],
                        myCurrentMergedSettings);
            if (_mySettingsFileList[i] === _myCommonSettingsFile) {
                myCommonSettingsFileSavedFlag = true;
            }
        }
        if (!myCommonSettingsFileSavedFlag) {
            saveSetting(_myOriginalCommonSettings, _myCommonSettingsFile,
                        myCurrentMergedSettings);
        }
    };

    Protected.restoreSettings = function () {
        var i, j, myChild;
        for (i = 0; i < Protected.settings.childNodes.length; ++i) {
            myChild = Protected.settings.childNode(i);
            for (j = 0; j < myChild.childNodes.length; ++j) {
                myChild.childNode(j).firstChild.nodeValue =
                    _myOriginalMergedSettings.childNode(i).childNode(j).firstChild;
            }
        }
        Protected.notifyListeners();
    };
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    obj.saveSettings = Protected.saveSettings; // needed for tests 

    obj.setSetting = function (theSection, theSetting, theValue) {
        var mySection = Protected.settings.childNode(theSection);
        if (!mySection) {
            mySection = Protected.settings.appendChild(Node.createElement(theSection));
        }

        var mySetting = mySection.childNode(theSetting);
        if (!mySetting) {
            mySetting = mySection.appendChild(Node.createElement(theSetting));
            mySetting.appendChild(Node.createTextNode(theValue));
        } else {
            mySetting.firstChild.nodeValue = theValue;
        }
        Protected.notifyListeners();
    };

    obj.reloadSettings = function () {
        setup(obj, theSceneViewer, _myCommonSettingsFile);
        Protected.notifyListeners();
    };

    obj.getSettingsFile = function () {
        if (_myCommonSettingsFile) {
            return _myCommonSettingsFile;
        } else {
            throw new Exception("No settingsfile found", fileline());
        }
    };

    obj.setSettingsFile = function (theSettingsFile) {
        setup(obj, theSceneViewer, theSettingsFile);
    };

    obj.setSettingsFileList = function (theFileList) {
        _mySettingsFileList = theFileList;
        setup(obj, theSceneViewer, _myCommonSettingsFile);
    };

    obj.getSettingsFileList = function () {
        return _mySettingsFileList;
    };

    obj.hasSection = function (theSection) {
        return (Protected.settings.childNode(theSection) !== null);
    };
    
    obj.findSections  = function (theXpath) {
        return Protected.settings.findAll(theXpath);
    };

    setup(obj, theSceneViewer, theSettingsFile, theSettingsFileList);
};

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
    this.onUpdateSettings = function (theNode) {
        var i, myChild;
        for (i = 0; i < theNode.childNodes.length; ++i) {
            myChild = theNode.childNodes[i];
            this[myChild.nodeName] = myChild.firstChild.nodeValue;
        }
    };
}

var ourSettings = ourSettings || new Settings();
