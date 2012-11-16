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
//   $RCSfile: GtkAnimationManager.js,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/04/22 16:30:06 $
//
//
//=============================================================================

use("AnimationManager.js");
use("Slider.js");

const GLOBAL_ANIMATIONS = "Global Animations";

function GtkAnimationManager(theViewer) {
    this.Constructor(this, theViewer);
}

GtkAnimationManager.prototype.Constructor = function(self, theViewer) {
    AnimationManager.prototype.Constructor(self, theViewer);
    var Base = {};

    const ANIMATION_FRAMES_PER_SECOND = 25;
    var _myViewer            = theViewer;
    var _myCurrentCharacter  = null;
    var _myCurrentClip       = null;
    var _mySlider            = null;
    var _myCharacterComboBox = null;
    var _myActive            = true;

    self.onActivate = function(theFlag) {
        self.enable(theFlag);

        var myAnimationButton = ourGlade.get_widget("animation_toggle_button");
        if (myAnimationButton.active != theFlag) {
            myAnimationButton.active = theFlag;
        }

        if (theFlag) {
            if (_mySlider.isDone()) {
                self.reset();
            }
            startCurrentClip();
        }
    }

    self.onStep = function(theDirection) {
        ourGlade.get_widget("run_animations_menuitem").active = false;
        startCurrentClip();
        return ourAnimationManager.step(theDirection);
    }

    self.onSlider = function() {
        if (_mySlider.wasMoved()) {
            ourGlade.get_widget("run_animations_menuitem").active = false;
            startCurrentClip();
            self.setTime(_mySlider.getValue());
        }
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        if (_myActive) {
            Base.onFrame(theTime);
            var myAnimationTime = self.getTime() % (_mySlider.length + 1 / ANIMATION_FRAMES_PER_SECOND);

            _mySlider.setValue(myAnimationTime);
            ourGlade.get_widget("animation_slider_label").text = Math.floor(myAnimationTime * ANIMATION_FRAMES_PER_SECOND) + " / " +
                Math.round(_mySlider.length * ANIMATION_FRAMES_PER_SECOND);

            if (_mySlider.isDone()) {
                ourGlade.get_widget("run_animations_menuitem").active = false;
            }
        }
    }

    function startCurrentClip() {
        if (_myCurrentCharacter) {
            self.startClip(_myCurrentCharacter, _myCurrentClip);
        }
    }

    self.setup = function() {
        // Turn off all animation widgets, if we do not need them
        if (window.scene.animations.childNodes.length == 0 && window.scene.characters.childNodes.length == 0) {
            ourGlade.get_widget("animation_toolbar").hide();
            ourGlade.get_widget("animation_menu").hide();
            _myActive = false;
            return;
        } else {
            ourGlade.get_widget("animation_toolbar").show();
            ourGlade.get_widget("animation_menu").show();
            _myActive = true;
        }

        self.enable(false);
        _mySlider = new AnimationSlider(ourGlade.get_widget("animation_slider"));

        // Only display combo boxes if we have characters
        if (window.scene.characters.childNodes.length == 0) {
            _mySlider.setLimits(window.scene.animations);
            return;
        }

        // Create Animation combo box
        _myCharacterComboBox = new ComboBoxText();
        _myCharacterComboBox.show();

        if (window.scene.animations.childNodes.length) {
            _myCharacterComboBox.append_text(GLOBAL_ANIMATIONS);
        }

        var myCharacters = window.scene.characters;
        for (var i = 0; i < myCharacters.childNodes.length; ++i) {
            var myCharacter = myCharacters.childNode(i);
            for (var j = 0; j < myCharacter.childNodes.length; ++j) {
                _myCharacterComboBox.append_text(myCharacter.name + " | " + myCharacter.childNode(j).name);
            }
        }

        ourGlade.get_widget("animation_box").pack_end(_myCharacterComboBox, false, false);
        _myCharacterComboBox.signal_changed.connect(_myCharacterComboBox, "on_changed");
        _myCharacterComboBox.on_changed = function() {
            if (_myCurrentCharacter) {
                self.stop(_myCurrentCharacter);
            }
            self.enableGlobalAnimations(this.active_text == GLOBAL_ANIMATIONS);
            self.reset();

            if (this.active_text == GLOBAL_ANIMATIONS) {
                _mySlider.setLimits(window.scene.animations);
                _myCurrentCharacter = null;
            } else {
                var mySplitText = this.active_text.split(" | ");
                _myCurrentCharacter = mySplitText[0];
                _myCurrentClip      = mySplitText[1];
                //print("character clip: "+ _myCurrentClip);
                var myClip = getDescendantByName(window.scene.characters, _myCurrentClip, true);
                if (myClip && myClip.childNodes.length > 0) {
                    _mySlider.setLimits(myClip.childNode(0));
                }
                //_mySlider.setLimits(getDescendantByName(window.scene.characters, _myCurrentClip, true));
            }
        }

        _myCharacterComboBox.active_index = 0;
    }
}

//=================================================
// Animation Menu Item Handlers
//=================================================

ourHandler.on_run_animations_activate = function(theMenuItem) {
    ourAnimationManager.onActivate(theMenuItem.active);
    ourStatusBar.set("Animations " + (theMenuItem.active ? "on" : "off"));
}
ourHandler.on_reset_animations_activate = function() {
    ourAnimationManager.reset();
    ourStatusBar.set("Reset Animations");
}
ourHandler.on_animation_step_forward_activate = function(theMenuItem) {
    var myTime = ourAnimationManager.onStep(true);
    ourStatusBar.set("Step forward to " + myTime.toFixed(2) + " sec.");
}

ourHandler.on_step_backward_activate = function(theMenuItem) {
    var myTime = ourAnimationManager.onStep(false);
    ourStatusBar.set("Step backward to " + myTime.toFixed(2) + " sec.");
}

//=================================================
// Animation Toolbar Handlers
//=================================================

ourHandler.on_animation_toggle_button_toggled = function(theButton) {
    ourGlade.get_widget("run_animations_menuitem").active = theButton.active;
}

ourHandler.on_animation_stop_clicked = function(theButton) {
    ourGlade.get_widget("run_animations_menuitem").active = false;
    ourAnimationManager.reset();
}

ourHandler.on_animation_slider_value_changed = function(theSlider) {
    ourAnimationManager.onSlider();
}
