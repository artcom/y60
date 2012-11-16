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
//   $RCSfile: gtkviewer.js,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2005/04/11 10:52:23 $
//
//
//=============================================================================
if (__main__ == undefined) var __main__ = "gtkviewer";

use("Y60JSSL.js");
use("SceneViewer.js");

var VERSION_STRING = "0.5";

var ourGlade = null;
//var ourRightViewer = null;
//var ourLeftViewer = null;
var ourMainWindow = null
var ourMainGUIBox = null
var ourHandler = {};
var ourAboutDialog = null;
var ourAreas = [];
var ourScenes = [];

var ourStatusBar = null;

var GLADE_FILE = "../glade/multiviewer.glade";

//=================================================
//
//  script command line parameters
//
//=================================================

var ourAllowedOptions = {
//    'unit-size': "%d"
};
//=================================================
//
//  Gtk Signal Handlers
//
//=================================================

ourHandler.on_mainWindow_realize = function() {
}

//=================================================
// File Menu Item Handlers
//=================================================

ourHandler.on_new_activate = function() {
}

ourHandler.on_quit_activate = function() {
    GtkMain.quit();
}


ourHandler.on_save1_activate = function(theMenuItem) {
    ourRightViewer.getRenderWindow().saveScene("save.x60",false);
}
ourHandler.on_quit1_activate = function() {
    GtkMain.quit();
}
//=================================================
// Tools Menu Item Handlers
//=================================================
//=================================================
// Help Menu Item Handlers
//=================================================

ourHandler.on_about1_activate = function(theMenuItem) {
    ourAboutDialog.find_child("lblVersion").label =
            "<span size=\"25000\">gtkviewer</span>\n" +
            "Version: " + VERSION_STRING;
    ourAboutDialog.show();
}

ourHandler.on_about_closed = function() {
    ourAboutDialog.hide();
}

//=================================================
// Toolbar Handlers
//=================================================

//=================================================
// Other GUI Handlers
//=================================================

function CanvasViewer(theArguments) {
    this.Constructor(this, theArguments);
}
CanvasViewer.prototype.Constructor = function(self, theArguments) {
    BaseViewer.prototype.Constructor(self, theArguments);
    self.BaseViewer = [];

    self.BaseViewer.onMouseMotion = self.onMouseMotion;
    self.onMouseMotion = function(theX, theY) {
        //self.BaseViewer.onMouseMotion(theX, theY)
    }

    self.BaseViewer.onMouseButton = self.onMouseButton;
    self.onMouseButton = function(theButton, theState, theX, theY) {
        self.BaseViewer.onMouseButton(theButton, theState, theX, theY);
    }
}

//===========================================
//
//  Main Application
//
//===========================================

function toggleToolbar(theModelType,theVisibleFlag) {
    switch (theModelType) {
        case SliceViewer.MODEL_TYPE_POLYGONAL:
            ourMainWindow.find_child("rbRotateMode").visible = theVisibleFlag;
            break;

        case SliceViewer.MODEL_TYPE_6FACES:
            ourMainWindow.find_child("rbRotateMode").visible = theVisibleFlag;

            break;

        case SliceViewer.MODEL_TYPE_CTSCAN:
            ourMainWindow.find_child("rbRotateMode").hide();
            ourMainWindow.find_child("pbResetCamera").hide();
            ourMainWindow.find_child("hboxCTScanWindowController").visible = theVisibleFlag;
            break;
    }
}

    function get3DFiles(theArguments) {
        var myFiles = [];
        for (var i = 0; i < theArguments.length; ++i) {
            var myArgument = theArguments[i];
            if (myArgument.search(/\.[xb]60/) != -1 ||
                       myArgument.search(/\.st./) != -1 ||
                       myArgument.search(/\.x3d/) != -1)
            {
                myFiles.push(myArgument);
            }
        }
        return myFiles;
    }

function main(argv) {
    ourGlade = new Glade(GLADE_FILE);
    ourMainWindow = ourGlade.get_widget("mainWindow");
    ourMainGUIBox = ourGlade.get_widget("mainGUIBox");

    ourAboutDialog = ourGlade.get_widget("dlgAbout");

    ourGlade.autoconnect(ourHandler, ourMainWindow);
    ourHandler.arguments = parseArguments(argv, ourAllowedOptions);
    ourHandler.files = get3DFiles(argv);
    ourHandler.isLoaded = false;

    var myList = ourGlade.get_widget("list");

    for (var i = 0; i < ourHandler.files.length; ++i) {
        var myScene = new Scene(ourHandler.files[i]);
        myScene.setup();
        ourScenes[i] = myScene;
        var myArea = null;
        if (ourAreas.length == 0) {
            myArea = new RenderArea();
        } else {
            myArea = new RenderArea(ourAreas[0]);
        }
        ourAreas.push(myArea);
        myArea.renderingCaps = Renderer.MULTITEXTURE_SUPPORT;
        myList.add(myArea);
        myList.homegeneous = true;
        myArea.show();
        var myViewer = new CanvasViewer(ourHandler.arguments);
        myViewer.setupWindow(myArea);
        myViewer.setScene(myScene);
        myViewer.setCanvasByIndex(0);

        //var myDom = myArea.scene.dom;
        //var myCanvas = myDom.getElementById("cv0");
        myViewer.registerMover(TrackballMover);
        //myViewer.setMover(TrackballMover);
        Logger.info("Added Canvas for: " + ourHandler.files[i]);
    }

    myList.show();
/*
    // load scene in first viewer
    ourRightViewer.setModelName(ourHandler.files[0]);
    ourRightViewer.loadScene();
    myRightArea.canvas.backgroundcolor = [0.5,0.5,0.5];
    ourRightViewer.registerMover(TrackballMover);
    ourRightViewer.setMover(TrackballMover, myRightArea.canvas.childNode('viewport'));

    // and add it to the second viewer
    ourLeftViewer.setModelName(ourHandler.files[1]);
    ourLeftViewer.loadScene();
    */
    ourMainWindow.show();

    // patch in a second canvas
    /*if (!myLeftCanvas) {
        myLeftCanvas = mySceneDom.getElementById("cv0").cloneNode(true);
        myLeftCanvas.id = "cv1";
        var myUpperViewport = myLeftCanvas.childNode(0);
        myUpperViewport.id = "vp1";
        myUpperViewport.camera = myRightArea.camera.id;
        myUpperViewport.position = "[0,0]";
        myUpperViewport.size = "[1,0.7]";
        var myLowerViewport = myUpperViewport.cloneNode(true);
        myLowerViewport.id = "vp2";
        myLowerViewport.camera = "c0";
        myLowerViewport.position = "[0,0.7]";
        myLowerViewport.size = "[1,0.3]";
        myLeftCanvas.appendChild(myLowerViewport);
        getDescendantByTagName(mySceneDom, "canvases", false).appendChild(myLeftCanvas);
        myLeftCanvas.backgroundcolor = [1,1,0.5];
    }*/

    //ourLeftViewer.setMover(TrackballMover, myUpperViewport);
    //ourLeftViewer.setMover(TrackballMover, myLowerViewport);
    // ourLeftViewer.activateHeadlight(true);

    // now we're ready to run
    return GtkMain.run(ourMainWindow);
}

function getFilenameDialog(theTitle, theAction)
{

    var myFileChooserDialog = new FileChooserDialog(theTitle, theAction);

    myFileChooserDialog.add_button(StockID.CANCEL,Dialog.RESPONSE_CANCEL);
    myFileChooserDialog.add_button(StockID.OK,Dialog.RESPONSE_OK);


    var myRetVal = myFileChooserDialog.run();
     // :-( otherwise dialog won't close before mainwindow gets focus
    myFileChooserDialog.hide();

    var myFilename = null;
    if (myRetVal == Dialog.RESPONSE_OK) {
        myFilename = myFileChooserDialog.get_filename();
    }

    return myFilename;
 }


if (main(arguments) != 0) {
    exit(1);
};


