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

const PREFERENCE_FILE_NAME = "preference.xml";

const PREFERENCE_NAMES = ["background_color", "headlight", "sunlight",
                                                    "daytime", "window_width", "window_height", "skymap",
                                                    "fog_disabled", "fog_linear", "fog_exponential",
                                                    "range_start", "range_end", "fog_density",
                                                    "fog_color", "fog_type_exp", "fog_type_exp2"];

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
        window.canvas.backgroundcolor = _myWidgets.background_color.color;
        _myGladeHandle.get_widget("headlight1").active = _myWidgets.headlight.active;
        _myGladeHandle.get_widget("sunlight1").active = _myWidgets.sunlight.active;

                if (_myWidgets.fog_disabled.active) {
                    window.scene.world.fogmode = "";
                    ourHandler.on_fog_disabled();
                } else if (_myWidgets.fog_linear.active) {
                    window.scene.world.fogmode = "linear";
                    window.canvas.backgroundcolor = _myWidgets.fog_color.color;
            window.scene.world.fogcolor = _myWidgets.fog_color.color;
            window.scene.world.fogrange = [_myWidgets.range_start.value,_myWidgets.range_end.value];
            window.scene.world.fogdensity = _myWidgets.fog_density.value;
            ourHandler.on_fog_linear();

                } else if (_myWidgets.fog_exponential.active) {
                    if(_myWidgets.fog_type_exp.active) {
                        window.scene.world.fogmode = "exp";
                    } else {
                        window.scene.world.fogmode = "exp2";
                    }

                    window.canvas.backgroundcolor = _myWidgets.fog_color.color;
            window.scene.world.fogcolor = _myWidgets.fog_color.color;
            window.scene.world.fogrange = [_myWidgets.range_start.value,_myWidgets.range_end.value];
            window.scene.world.fogdensity = _myWidgets.fog_density.value;
            ourHandler.on_fog_exponential();

                }

                ourHandler.on_range_start();
        ourHandler.on_range_end();
                ourHandler.on_fog_density();

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
