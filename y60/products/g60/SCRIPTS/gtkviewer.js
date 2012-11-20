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
var ourRightViewer = null;
var ourLeftViewer = null;
var ourMainWindow = null
var ourMainGUIBox = null
var ourHandler = {};
var ourAboutDialog = null;

var ourStatusBar = null;

var GLADE_FILE = "../glade/gtkviewer.glade";

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

//=================================================
// View Menu Item Handlers
//=================================================
ourHandler.on_wireframe_activate = function(theMenuItem) {
    ourHandler.lastViewport.wireframe = theMenuItem.active;
    ourHandler.lastViewer.getRenderWindow().queue_draw();
}

ourHandler.on_next_camera_activate = function(theMenuItem) {
    ourHandler.lastViewer.nextCamera(ourHandler.lastViewport);
    ourHandler.lastViewer.getRenderWindow().queue_draw();
}
ourHandler.on_prev_camera_activate = function(theMenuItem) {
    ourHandler.lastViewer.prevCamera(ourHandler.lastViewport);
    ourHandler.lastViewer.getRenderWindow().queue_draw();
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
    self.onMouseButton = function(theButton, theState, theX, theY) {
    }

    self.BaseViewer.onMouseButton = self.onMouseButton;
    self.onMouseButton = function(theButton, theState, theX, theY) {
        if (theButton == RIGHT_BUTTON) {
            if (theState) {
                var myPicking = new Picking(self.getRenderWindow());
                ourHandler.lastViewport = myPicking.getViewportAt(theX, theY);
                ourHandler.lastViewer = self;
                ourGlade.get_widget("wireframe").active = ourHandler.lastViewport.wireframe;
                ourViewportPopup.popup(theButton);
            }
        } else {
            self.BaseViewer.onMouseButton(theButton, theState, theX, theY);
        }
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


function main(argv) {
    ourGlade = new Glade(GLADE_FILE);
    ourMainWindow = ourGlade.get_widget("mainWindow");
    ourMainGUIBox = ourGlade.get_widget("mainGUIBox");
    ourViewportPopup = ourGlade.get_widget("viewportMenu");

    ourAboutDialog = ourGlade.get_widget("dlgAbout");

    ourGlade.autoconnect(ourHandler, ourMainWindow);
    ourHandler.arguments = parseArguments(argv, ourAllowedOptions);
    ourHandler.isLoaded = false;

    var myPanel = ourGlade.get_widget("rightBox");
    print("-1");
    var myRightArea = new RenderArea();
    myRightArea.renderingCaps = Renderer.MULTITEXTURE_SUPPORT;
    myPanel.add( myRightArea );
    myRightArea.show();
    ourMainWindow.show();
    ourRightViewer = new CanvasViewer(ourHandler.arguments);
    ourRightViewer.setupWindow(myRightArea);

    // start a second renderer
    var myLeftArea = new RenderArea(myRightArea); // share textures with this context
    myLeftArea.show();
    ourGlade.get_widget("leftBox").add(myLeftArea);
    ourLeftViewer = new CanvasViewer(ourHandler.arguments);
    ourLeftViewer.setupWindow(myLeftArea);
    // load scene in first viewer
    var myModelname = ourRightViewer.getModelName();
    if (myModelname) {
        myScene = new Scene(ourRightViewer.getModelName());
    } else {
        myScene = new Scene();
    }

    var myScene = new Scene(ourRightViewer.getModelName());
    myScene.setup();
    ourRightViewer.setScene(myScene);
    ourLeftViewer.setScene(myScene);
    ourRightViewer.setCanvasByIndex(0);
    ourLeftViewer.setCanvasByIndex(1);

/*
    var myCanvas = getDescendantByTagName(myScene.dom, 'canvas', true);
    ourRightViewer.setCanvas(myCanvas);
    myRightArea.canvas.backgroundcolor = [0.5,0.5,0.5];
    ourRightViewer.registerMover(TrackballMover);
    ourRightViewer.setMover(TrackballMover, myRightArea.canvas.childNode('viewport'));

    // and add it to the second viewer

    // patch in a second canvas
    var mySceneDom = myRightArea.scene.dom;
    var myCanvasRoot = getDescendantByTagName(mySceneDom, "canvases", true);
    var myRightCanvas = myCanvasRoot.childNodes[0];
    var myLeftCanvas = null;
    if (myCanvasRoot.childNodes.length > 1) {
        myLeftCanvas = myCanvasRoot.childNodes[1];
    }
    if (!myLeftCanvas) {
        myLeftCanvas = myRightCanvas.cloneNode(true);
        myLeftCanvas.id = "cv1";
        var myUpperViewport = myLeftCanvas.childNode(0);
        myUpperViewport.id = "vp1";
        myUpperViewport.camera = myRightArea.camera.id;
        myUpperViewport.position = "[0,0]";
        myUpperViewport.size = "[1,0.7]";
        var myLowerViewport = myUpperViewport.cloneNode(true);
        myLowerViewport.id = "vp2";
        myLowerViewport.position = "[0,0.7]";
        myLowerViewport.size = "[1,0.3]";
        myLeftCanvas.appendChild(myLowerViewport);
        getDescendantByTagName(mySceneDom, "canvases", false).appendChild(myLeftCanvas);
        myLeftCanvas.backgroundcolor = [1,1,0.5];
    }
    // attach the second viewer to the second canvas
    ourLeftViewer.setCanvas(myLeftCanvas);
    */

    // Add a second Viewport
    var myCanvasRoot = getDescendantByTagName(myScene.dom, "canvases", true);
    var myLeftCanvas = myCanvasRoot.childNodes[1];
    var myUpperViewport = myLeftCanvas.childNode(0);
    var myLowerViewport = myUpperViewport.cloneNode(true);
    myLowerViewport.id = "xxxxx";
    myLowerViewport.position = "[0,0.7]";
    myLowerViewport.size = "[1,0.3]";
    myLeftCanvas.appendChild(myLowerViewport);

    ourLeftViewer.registerMover(TrackballMover);
    ourLeftViewer.setMover(TrackballMover, myUpperViewport);
    ourLeftViewer.setMover(TrackballMover, myLowerViewport);
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


