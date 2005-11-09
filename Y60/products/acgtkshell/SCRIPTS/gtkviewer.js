//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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
    var myScene = new Scene(ourRightViewer.getModelName());
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


