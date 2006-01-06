//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: PreferenceDialog.js,v $
//   $Author: christian $
//   $Revision: 1.2 $
//   $Date: 2005/04/22 16:30:06 $
//
//
//=============================================================================

const PREFERENCE_FILE_NAME = "preference.xml";

const PREFERENCE_NAMES = ["background_color", "headlight", "sunlight", "daytime", "window_width", "window_height", "skymap"];

function PreferenceDialog(theGladeHandle, theViewer) {
    this.Constructor(this, theGladeHandle, theViewer);
}

PreferenceDialog.prototype.Constructor = function(self, theGladeHandle, theViewer) {

    var _myGladeHandle         = theGladeHandle;
    var _myViewer              = theViewer;
    var _myDialog              = _myGladeHandle.get_widget("dlgPreferences");
    var _myPreferenceDocument  = null;
    var _myWidgets             = [];

    function setup() {
        for (var i in PREFERENCE_NAMES) {
            var myName = PREFERENCE_NAMES[i];
            _myWidgets[myName] = _myGladeHandle.get_widget(myName);
            if (!_myWidgets[myName]) {
                print("### ERROR: Could not find widget: " + myName);
            }
        }

        _myDialog.signal_response.connect(self, "onResponse");
        loadPreferences();
    }

    setup();

    self.show = function() {
        _myDialog.show();
    }

    self.apply = function() {
        window.backgroundColor = _myWidgets.background_color.color;
        _myGladeHandle.get_widget("headlight1").active = _myWidgets.headlight.active;
        _myGladeHandle.get_widget("sunlight1").active = _myWidgets.sunlight.active;

        ourViewer.getLightManager().setSunPosition(_myWidgets.daytime.value);
        ourMainWindow.resize(_myWidgets.window_width.value, _myWidgets.window_height.value);

        var mySkyMap = _myWidgets.skymap.text;
        if (mySkyMap == "") {
            ourViewer.removeSkyBox();
        } else {
            if (fileExists(mySkyMap)) {
                ourViewer.addSkyBoxFromFile(_myWidgets.skymap.text);
            } else {
                var myError = "### WARNING: Could not find skymap file: " + mySkyMap;
                print(myError);
                ourStatusBar.set(myError);
            }
        }
        window.queue_draw();
    }

    function loadPreferences() {
        _myPreferenceDocument = Node.createDocument();
        if (fileExists(PREFERENCE_FILE_NAME)) {
            _myPreferenceDocument.parseFile(PREFERENCE_FILE_NAME);
        } else {
            //print("### WARNING: preference file '" + PREFERENCE_FILE_NAME + "' not found. Creating default preferences.");
            _myPreferenceDocument.appendChild(new Node.createElement("preferences"));
        }

        // Append new born preferences
        var myPreferences = _myPreferenceDocument.childNode("preferences");
        for (var i in PREFERENCE_NAMES) {
            var myName = PREFERENCE_NAMES[i];
            if (myPreferences.childNode(myName) == null) {
                myPreferences.appendChild(new Node.createElement(myName));
                writeToXML(myName);
            }
        }

        setupDialogFromDocument();
    }

    function writeToDialog(thePreferenceName) {
        var myValue  = _myPreferenceDocument.childNode("preferences").childNode(thePreferenceName).value
        var myWidget = _myWidgets[thePreferenceName];
        if ("color" in myWidget) {
            myWidget.color = stringToArray(myValue);
        } else if ("active" in myWidget) {
            myWidget.active = Number(myValue);
        } else if ("value" in myWidget) {
            myWidget.value = Number(myValue);
        } else if ("text" in myWidget) {
            myWidget.text = myValue;
        } else {
            print("### WARNING: Unknown widget type: " + myWidget);
        }
    }

    function writeToXML(thePreferenceName) {
        var myNode   = _myPreferenceDocument.childNode("preferences").childNode(thePreferenceName)
        var myWidget = _myWidgets[thePreferenceName];
        if ("color" in myWidget) {
            myNode.value = myWidget.color;
        } else if ("active" in myWidget) {
            myNode.value = Number(myWidget.active);
        } else if ("value" in myWidget) {
            myNode.value = Number(myWidget.value);
        } else if ("text" in myWidget) {
            myNode.value = myWidget.text;
        } else {
            print("### WARNING: Unknown widget type: " + myWidget);
        }
    }

    function setupDialogFromDocument() {
        for (var i in PREFERENCE_NAMES) {
            writeToDialog(PREFERENCE_NAMES[i]);
        }
    }

    function savePreferences() {
        for (var i in PREFERENCE_NAMES) {
            writeToXML(PREFERENCE_NAMES[i]);
        }

        _myPreferenceDocument.saveFile(PREFERENCE_FILE_NAME);
    }

    self.onResponse = function(theResponse) {
        switch (theResponse) {
            case Dialog.RESPONSE_OK:
                self.apply();
                savePreferences();
                _myDialog.hide();
                break;
            case Dialog.RESPONSE_APPLY:
                self.apply();
                break;
            case Dialog.RESPONSE_CANCEL:
                loadPreferences();
                self.apply();
                _myDialog.hide();
                break;
        }
    }
}