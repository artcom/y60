//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
// 
// Resizeable Button.
//
// This widget represents a resizeable button. 
//
// for a definition of what widgets are, see WidgetFactory.js and widgetBase.js.
//
// New Properties of Resizeable Button:
// 
// - image:        a path prefix for a set of images that follow the naming scheme
//                 PREFIX_
// - pressedImage: while the button is pressed down, display this image instead.
// 
// 
// onClick():      issued when a release event inside the region of the button widget
//                 after a press event inside the region of the button widget is received.
//                 if a nonzero value is returned, event propagation will be stopped.
// 
// Example:
/*
  <style id="style###" name="aButtonStyle"
  color='555555' selectedColor='FFFFFF' backgroundColor="999999" textColor='FFFFFF'
  VTextAlign="0" HTextAlign="0"
  font='FONTS/arial.ttf' fontsize='20' />

  <widget:ButtonWidget id="###" position="[100,100]" size="[100, 30]"
  text="'Sample Text"
  style="#style###"
  image = "'TEX/button.png"
  pressedImage="'TEX/button_pressed.png"
  captions="{'de':'Klick mich!',
  'en':'Click me!'}"
  onClick="print('do something');return true;"/>
*/
//=============================================================================

use("Styles.js");
use("Fonts.js");

use("BuildUtils.js");

function myRenderTextAsImage(theText, theSize, theStyle, thePosition) 
{
    if (!thePosition) {
        thePosition = new Vector2i(0,0);
    }

    var myImageNode = null;
    var myTextSize = null;
    var myText = new String(theText);

    if (!myImageNode) {
        window.setTextPadding(theStyle.topPad, theStyle.bottomPad, theStyle.leftPad, theStyle.rightPad);
        window.setHTextAlignment(theStyle.HTextAlign);
        window.setVTextAlignment(theStyle.VTextAlign);

	var myOldColor = null;
	if(myOldColor){
	    myOldColor = window.getTextColor();
	}
        
        window.setTextColor(asColor(theStyle.textColor));
        window.setTracking(theStyle.tracking);
        window.setLineHeight(theStyle.lineHeight);

        myImageNode = Modelling.createImage(window.scene, 0, 0, "BGRA");

        myImageNode.resize = "pad";
        myImageNode.wrapmode = "clamp_to_edge"
        myImageNode.mipmap = false;

        Logger.trace("now adding the rendered text image to the scene...");
        window.scene.images.appendChild(myImageNode);
        Logger.trace("image added successfully...");

        var myFontName = getFontNameFor(theStyle.font, theStyle.fontsize);

        Logger.trace("now rendering the text...");
	myTextSize = window.renderTextAsImage( myImageNode, theText, 
					       myFontName, 
					       theSize[0], theSize[1],
					       thePosition);
        Logger.trace("text rendering successful.");

        var myRelativeTextSize = [myTextSize[0] / myImageNode.width,myTextSize[1] / myImageNode.height];

	if(myOldColor){
	    window.setTextColor(myOldColor);
	}

	var myMatrix = new Matrix4f();
        myMatrix.makeScaling(new Vector3f(myTextSize.x / myImageNode.width,
                                          myTextSize.y / myImageNode.height, 1));
        myImageNode.matrix = myMatrix;

        Logger.trace("now updating scene...");
	window.scene.update(Scene.ALL);
        Logger.trace("scene updated successfully...");
    }

    window.setTextPadding(0,0,0,0);

    return myImageNode;
}

// utility function to create a texture from three single images (left, center, right)
// and resize it using the size of the rendered text
// theId: when multiple textures (e.g. with multiple colors) for one
//        button exist, specify which texture you want to use.

function composeBackgroundFrom(theLeftImage, theCenterImage, theRightImage, theCenterWidth) {


    var myLeftSize  = getImageSize(theLeftImage);
    var myCenterSize  = getImageSize(theCenterImage);
    var myRightSize  = getImageSize(theRightImage);

    // XXX TODO: copy theCenterImage before modifying it.

    applyImageFilter(theCenterImage, "crop", [0,0,myCenterSize.x,myCenterSize.y]);
    applyImageFilter(theCenterImage, "resizebilinear", [theCenterWidth,myCenterSize[1]] );

    var myTotalSize = [];
    myTotalSize[0] = myLeftSize[0] + theCenterWidth + myRightSize[0];
    myTotalSize[1] = Math.max(Math.max(myLeftSize[1], myCenterSize[1]), myRightSize[1]);

    Logger.trace("creating image of size "+myTotalSize+" for the resizeable button background ...");
    var myImageNode = Modelling.createImage(window.scene, myTotalSize[0], myTotalSize[1], "BGRA");
    myImageNode.resize = "none";

    window.scene.images.appendChild(myImageNode);
    Logger.trace("image created.");

    var x1 = 0;
    var x2 = x1 + myLeftSize[0];
    var x3 = x2 + theCenterWidth;
    blitImage(theLeftImage, myImageNode,    [x1,0], [0,0], myLeftSize);
    blitImage(theCenterImage, myImageNode,[x2-1,0]);
    blitImage(theRightImage, myImageNode, [x3-1,0], [0,0], myRightSize);

    return myImageNode;
}

function myCreateMaterialFromImage( theImage ) {
        var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, theImage);
        addMaterialRequirement(myMaterial, "vertexparams", "[10[color]]");    
        myMaterial.transparent = true;
        myMaterial.properties.targetbuffers.depth = false;    
	    return myMaterial; 
}

function ResizeableButtonWidget() {
    this.Constructor(this);
}

ResizeableButtonWidget.prototype.Constructor = function(Public) {

    EffectWidget.prototype.Constructor(Public);
    LayoutWidget.prototype.Constructor(Public);

    Public._myTextImageNode = 0;
    Public._myBackgroundImageNode = 0;

    Public._myLeftImageNode = 0;
    Public._myCenterImageNode = 0;
    Public._myRightImageNode = 0;

    // required by "fade" transition from EffectWidget
    Public.alpha setter = function(theAlpha) {
	Public._myTextMQB.material.properties.surfacecolor[3] = theAlpha;
	Public._myBackgroundMQB.material.properties.surfacecolor[3] = theAlpha;
    }

    Public.setText = function(theText) {

	if (Public._myTextImageNode) {
	    Public._myTextImageNode.parentNode.removeChild(Public._myTextImageNode);
	}

	// We rely on a style getter here, defined in LayoutWidget.
	// 
	var myTextImageNode = myRenderTextAsImage(theText, [0,0], Public.style, Public.position);


	var myMaterial = myCreateMaterialFromImage(myTextImageNode);
	var myQuad = Modelling.createQuad(window.scene, myMaterial.id, [0,0,-0.1],[1,1,-0.1]);
	var myBody = Modelling.createBody(window.scene.world, myQuad.id);
	Public._myTextMQB = {"material": myMaterial, "quad": myQuad, "shape": myQuad, "body":myBody };


	var myTextSize = getImageSize(myTextImageNode);

	var myBackgroundImageNode = composeBackgroundFrom(Public._myLeftImageNode, Public._myCenterImageNode, Public._myRightImageNode, myTextSize[0]);

	myMaterial = myCreateMaterialFromImage(myBackgroundImageNode);
	myQuad = Modelling.createQuad(window.scene, myMaterial.id, [0,0,0],[1,1,0]);
	myBody = Modelling.createBody(window.scene.world, myQuad.id);
	Public._myBackgroundMQB = {"material": myMaterial, "quad": myQuad, "shape": myQuad, "body":myBody };

	Public.intrinsicSize = getImageSize(myBackgroundImageNode);
    }

    var _myBaseSetup = Public.setup;
    Public.setup = function() {

        Logger.trace("now loading image1");
	Public._myLeftImageNode = Modelling.createImage(window.scene, Public.leftimage);
        Logger.trace("now loading image2");
	Public._myCenterImageNode = Modelling.createImage(window.scene, Public.centerimage);
        Logger.trace("now loading image3");
	Public._myRightImageNode = Modelling.createImage(window.scene, Public.rightimage);
        Logger.trace("images loaded...");
	window.scene.images.appendChild(Public._myLeftImageNode);
	window.scene.images.appendChild(Public._myCenterImageNode);
	window.scene.images.appendChild(Public._myRightImageNode);
        Logger.trace(" .. and added to scene graph.");

	window.scene.update(Scene.MATERIALS);

	_myBaseSetup();
	if ("text" in Public) {
	    Public.setText(Public.text);
	}
    }

    var _myBaseFinish = Public.finish;
    Public.finish = function() {
	_myBaseFinish();
	Public.position = Public.getAbsolutePosition();	
    }
}
