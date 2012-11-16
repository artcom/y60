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

// external dependencies
plug("Sound");

use("Exception.js");
use("ASSManager.js");
use("SceneViewer.js");
use("MaterialQuadBody.js");

// namespace for GUI library
var GUI = {};

// utilities
use("Characters.js");
use("Internationalization.js");
use("SceneConfig.js");
use("Utilities.js");

// layer crap
use("LayerManager.js");
use("Layers/LayerBase.js");
use("Layers/ImageLayer.js");

// widgets
use("GuiBase.js");

use("Image.js");
use("ImageButton.js");

use("Text.js");
use("TextWidget.js");
use("TextBox.js");
use("TextButton.js");
use("TextLabel.js");

// animation framework
use("Animation/AnimationManager.js");
use("Animation/Animation.js");
use("Animation/SimpleAnimation.js");
use("Animation/ClosureAnimation.js");
use("Animation/PropertyAnimation.js");
use("Animation/RotationAnimation.js");
use("Animation/CompositeAnimation.js");
use("Animation/ParallelAnimation.js");
use("Animation/SequenceAnimation.js");
use("Animation/DelayAnimation.js");

