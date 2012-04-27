//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
// TESTED:
// - Node attributes:
//   - src
//   - color_scale
//   - color_bias
//   - rasterpixelformat
//   - name
//   - matrix
//   - resize
//   - filter
//   - filter_params
// - Facade attributes
//   - width
//   - height
//   - bytesperpixel
//   - internal_format
//   - loadcount
//
//=============================================================================
//
// TODO:
// - type
// - mipmap
// - depth
// - tile
// - textureid (facade attribute plug)
//
//=============================================================================
//
// OPEN ISSUES:
// - Color scale/bias changes do not trigger mip-map recalculation
// - Color scale/bias does not work correct for the red component
// - exec does not return correct exit codes
//
//=============================================================================

use("Overlay.js");
use("SceneTester.js");

var window   = null;
var ourImage = null;
var ourSize  = null;
var ourLoadCount = 0;
var ourOverlay = null;
var ourTexture = null;

var ourShow = new SceneTester(arguments);
ourShow.SceneTester = [];

function ENSURE(theExpression) {
    var myResult = false;
    try {
        myResult = eval(theExpression);
    } catch(e) {
        print('###### EXCEPTION:'+e);
    }
    
    return myResult;
}

ourShow.SceneTester.setup = ourShow.setup;
ourShow.setup = function () {
    ourShow.SceneTester.setup(256, 256);
    window.fixedFrameTime = 1;
    ourShow.offscreenFlag = false;
    // Image setup
    ourImage = new Node("<image src='tex/colors.rgb'/>").firstChild;
    ourImage = window.scene.images.appendChild(ourImage);

    ourOverlay = new ImageOverlay(window.scene, ourImage);
    ourTexture = ourOverlay.node.getElementById(ourOverlay.textureunit.texture);

    window.canvas.backgroundcolor = [0.5,0.5,0.5,1];

    window.scene.updateAllModified();
    // Initial load
    ENSURE("ourImage.width == 256");
    ENSURE("ourImage.height == 256");
    ENSURE("ourImage.bytesperpixel == 3");
    ENSURE("ourImage.rasterpixelformat == 'BGR'");
    ENSURE("ourImage.name == 'JohnDoe'");
    ENSURE("almostEqual(ourImage.matrix, new Matrix4f())");

    window.scene.updateAllModified();
    // Test internal pixel format handling
    ENSURE("ourTexture.internal_format == 'RGB8'");
    ourTexture.color_scale[3] = 0;
    ENSURE("ourTexture.internal_format == 'RGBA8'");
    ourTexture.color_scale[3] = 1;
    ENSURE("ourTexture.internal_format == 'RGB8'");
}
function updateAndRender() {
    window.scene.updateAllModified();
    ourShow.saveTestImage();
}

ourShow.onFrameDone = function(theFrameCount) {
    try {
        print("########## Frame " + theFrameCount);
        switch(theFrameCount) {
            case 1:
                // Make sure we do not load the image unnessessarily
                //ENSURE("ourImage.loadcount == " + (++ourLoadCount));
                updateAndRender();
                // Test color scale and bias
                ourTexture.color_scale[1] = 0;
                preLoad(ourTexture);                        
                //ENSURE("ourImage.loadcount == " + ourLoadCount);
                break;
            case 2:
                updateAndRender();
                ourTexture.color_bias[1] = 1;
                preLoad(ourTexture);                        
                //ENSURE("ourImage.loadcount == " + ourLoadCount);
                break;
            case 3:    
                updateAndRender();
                ourTexture.color_scale[1] = 1;
                ourTexture.color_bias[1] = 0;
                
                // Test loading new alpha image
                ourImage.src = "tex/FullAlpha.rgb";
                ourLoadCount++;
                //ENSURE("ourImage.loadcount == " + ourLoadCount);
                ENSURE("ourImage.width == 128");
                ENSURE("ourImage.height == 128");
                ENSURE("ourImage.bytesperpixel == 4");
                ENSURE("ourImage.rasterpixelformat == 'BGRA'");
                ENSURE("ourTexture.internal_format == 'RGBA8'");
                //ENSURE("ourImage.loadcount == " + (ourLoadCount));
                break;
            case 4:    
                updateAndRender();
                // Test loading non power of two images
                ourImage.resize = "pad"; // default resize is 'none'
                ourImage.src = "tex/non_power_of_two.rgb";
                ourOverlay.width = 174;
                ourOverlay.height = 97;
                ENSURE("ourImage.width == 256");
                ENSURE("ourImage.height == 128");
                ourSize = getImageSize(ourImage);
                ENSURE("ourSize.x == 174");
                ENSURE("ourSize.y == 97");
                ENSURE("ourImage.bytesperpixel == 3");
                ENSURE("ourImage.rasterpixelformat == 'BGR'");
                ENSURE("ourTexture.internal_format == 'RGB8'");
                ENSURE("ourImage.resize == 'pad'");
                //ENSURE("ourImage.loadcount == " + (++ourLoadCount));
                break;
            case 5:    
                updateAndRender();
                ourImage.resize = "scale";
                ENSURE("ourImage.width == 256");
                ENSURE("ourImage.height == 128");
                //ENSURE("ourImage.loadcount == " + (++ourLoadCount));
                ENSURE("almostEqual(ourImage.matrix, new Matrix4f())");
                break;
            case 6:    
                updateAndRender();
                // Test image filtering
                ourImage.filter = "resizebilinear";
                ourImage.filter_params = [20, 10];
                //explicit image load triggern
                //ENSURE("ourImage.loadcount == " + ourLoadCount);
                ENSURE("ourImage.width == 32");
                ENSURE("ourImage.height == 16");
                //ENSURE("ourImage.loadcount == " + (++ourLoadCount));
                break;
            case 7:    
                updateAndRender();
                ourImage.filter_params = [200, 1];
                ENSURE("ourImage.width == 256");
                ENSURE("ourImage.height == 1");
                //ENSURE("ourImage.loadcount == " + (++ourLoadCount));
                break;
            case 8:    
                updateAndRender();
                // Test raster manipulation
                ourImage.filter = "";
                ourImage.raster.clear();
                //ENSURE("ourImage.loadcount == " + (++ourLoadCount));
                for (var i = 0; i < ourImage.raster.width; ++i) {
                    ourImage.raster.setPixel(i,63, [1,1,0,1]);
                }
                ENSURE("almostEqual(ourImage.raster.getPixel(0,0), [0,0,0,0])");
                ENSURE("almostEqual(ourImage.raster.getPixel(0,63), [1,1,0,0])");
                //ENSURE("ourImage.loadcount == " + ourLoadCount);
                break;
            case 9:
                updateAndRender();
                exit(0);
                break;
        }
    } catch (ex) {
        print("Exception caught: "+ex);
        exit(5);
    }

}

try {
    ourShow.setup();
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}

