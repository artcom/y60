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
// Text Decoder Widget.
//
// for a definition of what widgets are, see WidgetFactory.js and widgetBase.js.
//
// This widget represents an animated label. 
//
// TextDecoderWidget inherits from LabelWidget and exhibits all its properties and functions.
// Inherited Properties and Functions are:
//
// - text:         a string with the initial contents of this Label.
// - position:     a vector2f describing the relative position of this button to its parent.
// - size:         a vector2f describing the size of this label.
// - captions:     the "captions" property is a mapping from language id to caption texts.
// - style:        an optional style object, defining the properties of rendered text.
//                 default is DEFAULT_LABELWIDGET_STYLE.
//                 NB: the "style"-property may also be a DOM Node.
// 
// New Properties of TextDecoder Widget:
// 
// - animating: a boolean value indicating whether an animation is currently running.
// 
// Functions:
// 
// 
// 
// Example:
//=============================================================================

var ourBGTextures = [];
ourBGTextures[19] = [ourCommonPath + "TEX/BGS/proj_headline_small_bg_left.png", 
                     ourCommonPath + "TEX/BGS/proj_headline_small_bg_center.png", 
                     ourCommonPath + "TEX/BGS/proj_headline_small_bg_right.png"];

ourBGTextures[32] = [ourCommonPath + "TEX/BGS/touch_headline_small_bg_left.png", 
                     ourCommonPath + "TEX/BGS/touch_headline_small_bg_center.png", 
                     ourCommonPath + "TEX/BGS/touch_headline_small_bg_right.png"];
                     

ourBGTextures[42] = [ourCommonPath + "TEX/BGS/proj_headline_large_bg_left.png", 
                     ourCommonPath + "TEX/BGS/proj_headline_large_bg_center.png", 
                     ourCommonPath + "TEX/BGS/proj_headline_large_bg_right.png"];
                     
ourBGTextures[70] = [ourCommonPath + "TEX/BGS/touch_headline_large_bg_left.png", 
                     ourCommonPath + "TEX/BGS/touch_headline_large_bg_center.png", 
                     ourCommonPath + "TEX/BGS/touch_headline_large_bg_right.png"];

ourBGTextures["48_player"] = [ourCommonPath + "TEX/BGS/player_textbg48_left.png", 
                             ourCommonPath + "TEX/BGS/player_textbg48_center.png", 
                             ourCommonPath + "TEX/BGS/player_textbg48_right.png"];

ourBGTextures["36_player"] = [ourCommonPath + "TEX/BGS/player_textbg36_left.png", 
                             ourCommonPath + "TEX/BGS/player_textbg36_center.png", 
                             ourCommonPath + "TEX/BGS/player_textbg36_right.png"];

function TextDecoderWidget() {
	this.Constructor(this);
}

TextDecoderWidget.prototype.Constructor = function(Public) {

     LabelWidget.prototype.Constructor(Public);

     var myDecoders = [];

     var _myLanguage = null;

     _myCharacterSoup = new CharacterSoup(Public.style.name,
                                          Public.style.font,
                                          [Public.style.size, Public.style.fontsize],[0,0],0); 

     Public.setVisible = function(theBool) {
	 for (i in myDecoders) {
	     myDecoders[i].setVisible(theBool);
	 }
     }

     Public.startAnimation(theTime) {
	 _myDecoders[_myLanguage][theLanguage].startAnimation(2.5);
     }

     Public.animate = function(theTime) {
	 for (i in _myDecoders) {
	     var myDecoder = _myDecoders[i];
	     myDecoder[_myLanguage].onFrame(theT);
	     if (Math.floor(theTime)%10 == 0) {
		 myDecoder[_myLanguage].startAnimation(2.5);
	     }
	 }
     }

     Public.onLanguageChange = function(theLanguage) {
	 if (theLanguage in myDecoders) {
	     _myLanguage
	 }
     }

     Public.setup = function() {

	 if (!(myStyle.size in ourBGTextures)) {
	     Logger.fatal(Public.WIDGET_META.name + ": no image data for size + myStyle.size"); 
	 }

	 var myTransform = Modelling.createTransform(window.scene.world, myPageNode.id);
	 for (lang in captions) {
	     myText = captions[lang];
	     var myTextDecoder = 
	     new TextDecoder(myText,
			     myStyle.size, 
			     _myCharacterSoup, 
			     myWidgetPos3D,
			     new Vector3f([1,0,0]), 
			     true,
			     ourBGTextures[myStyle.size],
		     {front:10, back:10, top:10, bottom:10});
	     _myDecoders[lang] = myTextDecoder;
	     for (b in myTextDecoder.bodies) {
		 _myTransform.appendChild(myTextDecoder.bodies[b]);
	     }
	 }
     }
}
