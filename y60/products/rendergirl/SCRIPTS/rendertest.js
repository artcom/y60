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

use("Y60JSSL.js");

var RenderTest           = {};
var ourFrameCounter      = 0;
var ourSavedFrameCounter = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All our tests
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

var ourTests = [];
ourTests.push({title: "Initial Image", screenshot: true});

// File Menu Item Handlers
ourTests.push({title: "Save", handler: "on_save_activate"});
ourTests.push({title: "Save as", handler: "on_save_as_activate", arguments: "TESTMODELS/test_save_as.x60"});
ourTests.push({title: "New File", handler: "on_new_activate", screenshot: true});
ourTests.push({title: "Open", handler: "on_open_activate", arguments: "TESTMODELS/test_save_as.x60", screenshot: true});
ourTests.push({title: "Saved screenshot"}); // Just to change the title
ourTests.push({title: "Save screenshot", handler: "on_save_screenshot_activate", arguments: "TEST_IMAGES/saved_image.png"});

// Edit Menu Item Handlers
ourTests.push({title: "Open Preferences", handler: "on_preferences_activate"});
ourTests.push({title: "Close Preferences", callback: "closePreferences()"});

// View Menu Item Handlers
ourTests.push({title: "Wireframe", handler: "on_wireframe_activate", active: true, screenshot: true});
ourTests.push({title: "Texturing off", handler: "on_texturing_activate", active: false, screenshot: true});
ourTests.push({title: "Flatshading on", handler: "on_flatshading_activate", active: true, screenshot: true});
ourTests.push({title: "Backface culling on", handler: "on_backface_culling_activate", active: true, callback: "moveCameraToCenter()", screenshot: true});
ourTests.push({title: "Backface culling off", handler: "on_backface_culling_activate", active: false,  screenshot: true});
ourTests.push({title: "Reset camera", handler: "on_reset_camera_activate", screenshot: true});
//ourTests.push({title: "Coordinate system on", handler: "on_coordinate_system_activate", active: true, screenshot: true});
// on_fullscreen_activate does not test easily

// Camera Menu Item Handlers
ourTests.push({title: "Previous camera", handler: "on_previous_camera_activate", screenshot: true});
ourTests.push({title: "Next camera", handler: "on_next_camera_activate", screenshot: true});
ourTests.push({title: "Picking trackball mover", handler: "on_picking_trackball_activate", active: true});
ourTests.push({title: "Centered trackball mover", handler: "on_centered_trackball_activate", active: true});
ourTests.push({title: "Fly mover", handler: "on_fly_mover_activate", active: true});
ourTests.push({title: "Walk mover", handler: "on_walk_mover_activate", active: true});
ourTests.push({title: "Zoom in", handler: "on_zoom_in_activate", screenshot: true});
ourTests.push({title: "Zoom out", handler: "on_zoom_out_activate", screenshot: true});
ourTests.push({title: "Reset camera", handler: "on_reset_camera_activate"});

// Light Menu Item Handlers
ourTests.push({title: "Lighting off", handler: "on_lighting_activate", active: false, screenshot: true});
ourTests.push({title: "Headlight off", handler: "on_headlight_activate", active: false, screenshot: true});
ourTests.push({title: "Sunlight off", handler: "on_sunlight_activate", active: false, screenshot: true});

// Tools Menu Item Handlers
ourTests.push({title: "Body bounding boxes", handler: "on_bb_body_activate", active: false, screenshot: true});
ourTests.push({title: "Shape bounding boxes", handler: "on_bb_shape_activate", active: false, screenshot: true});
ourTests.push({title: "Hierarchy bounding boxes", handler: "on_hierarchy_activate", active: false, screenshot: true});
ourTests.push({title: "Bounding boxes off", handler: "on_bb_off_activate", active: false, screenshot: true});
ourTests.push({title: "Culling on", handler: "on_culling_activate", active: true});
ourTests.push({title: "Debug culling on", handler: "on_debug_culling_activate", active: true});
ourTests.push({title: "Pause on", handler: "on_pause_activate", active: true, callback: "ourHandler.on_pause_activate({active:false})"});
ourTests.push({title: "Statistics on", handler: "on_statistics_activate", active: true});

// Help Menu Item Handlers
ourTests.push({title: "About box", handler: "on_about1_activate", active: true});
ourTests.push({title: "Close About box", handler: "on_about_closed", active: true});

// Animation Menu Item Handlers
ourTests.push({title: "Run animations", handler: "on_run_animations_activate", active: true, screenshot: true});
ourTests.push({title: "Stop animations", handler: "on_run_animations_activate", active: false, screenshot: true});
ourTests.push({title: "Animations step forward", handler: "on_animation_step_forward_activate", screenshot: true});
ourTests.push({title: "Animations step backard", handler: "on_step_backward_activate", screenshot: true});
ourTests.push({title: "Reset animations", handler: "on_reset_animations_activate", screenshot: true});

// And finally the quit handler :-)
ourTests.push({title: "Quit", handler: "on_quit_activate"});

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Some helper callbacks
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

function closePreferences() {
    ourPreferenceDialog.onResponse(Dialog.RESPONSE_OK);
}

function moveCameraToCenter() {
    window.camera.position = new Vector3f(0, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The test framework
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderTest.onFrame = function(theTime) {
    try {
        window.fixedFrameTime = 0.1;
        var myTest = ourTests[ourFrameCounter];
        print(ourFrameCounter + ". " + myTest.title);

        // Activate test
        if ("handler" in myTest) {
            var myArguments = myTest;
            if ("arguments" in myTest) {
                myArguments = myTest.arguments;
            }
            ourHandler[myTest.handler](myArguments);
        }

        if ("callback" in myTest) {
            eval(myTest.callback);
        }

        window.renderText([30, 500], ourFrameCounter + ". " + myTest.title);
    } catch (ex) {
        reportException(ex);
        GtkMain.exitCode = 1;
        GtkMain.quit();
    }
}

RenderTest.onPostRender = function(theTime) {
    try {
        var myTest = ourTests[ourFrameCounter];
        if ("screenshot" in myTest && myTest.screenshot) {
            //print(">>> Save: '" + myTest.title + "' as 'cube_" + ourFrameCounter + ".png'");
            var myNumber = ourFrameCounter < 10 ? "0" + ourFrameCounter : ourFrameCounter;
            //window.saveBuffer("TEST_IMAGES/cube_" + myNumber + ".png");
            triggerOffscreenRendering("TEST_IMAGES/cube_" + myNumber + ".png");
        }
        ourFrameCounter++;

        // Deactivate the test
        if ("handler" in myTest && "active" in myTest) {
            myTest.active = !myTest.active;
            ourHandler[myTest.handler](myTest);
        }

        if (ourFrameCounter >= ourTests.length) {
            GtkMain.quit();
        }
    } catch (ex) {
        reportException(ex);
        GtkMain.exitCode = 1;
        GtkMain.quit();
    }
}

var ourOffscreenRenderArea = null;

function setupOffscreenRendering() {
    // Create target image for offscreen rendering
    var myTargetImage = getDescendantByName(window.scene.images, "offscreen", false);
    if (!myTargetImage) {
        myTargetImage = Modelling.createImage(window.scene, 800, 520, "RGB");
        myTargetImage.name = "offscreen"
        myTargetImage.mipmap = false;
    }

    // Copy main canvas to offscreen canvas
    var myOffscreenCanvas = window.scene.canvases.firstChild.cloneNode(true);
    adjustNodeIds(myOffscreenCanvas);
    myOffscreenCanvas.targets = [myTargetImage.id];

    if (window.scene.canvases.childNodes.length == 1) {
        window.scene.canvases.appendChild(myOffscreenCanvas);
    } else {
        window.scene.canvases.replaceChild(myOffscreenCanvas, window.scene.canvases.lastChild);
    }

    // Setup offscreen render area
    ourOffscreenRenderArea = new OffscreenRenderArea();
    ourOffscreenRenderArea.scene = window.scene;
    ourOffscreenRenderArea.canvas = myOffscreenCanvas;
    ourOffscreenRenderArea.eventListener = ourOffscreenRenderArea;
    ourOffscreenRenderArea.fixedFrameTime = window.fixedFrameTime;
    ourOffscreenRenderArea.getRenderer().boundingVolumeMode = window.getRenderer().boundingVolumeMode;

    ourOffscreenRenderArea.onFrame = function() {
        ourOffscreenRenderArea.renderText([30, 500], ourFrameCounter + ". " + ourTests[ourFrameCounter].title);
    }

    ourOffscreenRenderArea.onPreViewport = function() {
        // Use main canvas viewport, otherwise the lightmanager gets confused
        ourViewer.onPreViewport(window.scene.canvases.firstChild.firstChild);
    }
    ourOffscreenRenderArea.onPostViewport = function() {
        // Use main canvas viewport, otherwise the lightmanager gets confused
        ourViewer.onPostViewport(window.scene.canvases.firstChild.firstChild);
    }

    return myTargetImage;
}

function triggerOffscreenRendering(theFilename) {
    var myTargetImage = setupOffscreenRendering();

    // Trigger offscreen rendering
    ourOffscreenRenderArea.renderToCanvas(true);

    // Flip vertically since framebuffer content is upside-down
    saveImageFiltered(myTargetImage, theFilename, ["flip"], [[]]);
}

// Start the rendergirl application
use("rendergirl.js");

