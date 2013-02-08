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
use("SceneViewer.js");

plug("Pango");

var ourShow = new SceneViewer(arguments);
var _myImage = null;

ourShow.SceneViewer = [];

ourShow.SceneViewer.setup = ourShow.setup;
ourShow.setup = function() {
    ourShow.SceneViewer.setup();
    window.resize(1024, 768);
    window.canvas.backgroundcolor = new Vector4f(0.2, 0.8, 1.0, 1.0);

    var myImage = Modelling.createImage(window.scene, 1024, 1024, "BGRA");
    var myTexture  = Modelling.createTexture(window.scene, myImage);
    myTexture.wrapmode = "clamp_to_edge";
    var myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, myTexture);
    var myShape = Modelling.createQuad(window.scene, myMaterial.id, new Vector3f(-0.04, -0.06, -0.1), new Vector3f(0.04, 0.02, -0.1));
    var myNode = Modelling.createBody(window.scene.world, myShape.id);

    var myLayout = new Pango.Layout(myImage);
    myLayout.setColor([0.1,0.8,0.4,1.0]);
    var myFontDesc = new Pango.FontDescription("Alex Brush 100");
    myLayout.font_description = myFontDesc;
    myLayout.text = "\u1E78berl\u00E4nge";
    Logger.warning(myLayout.context);
    Logger.warning(myLayout.font_description);
};


try {
    ourShow.setup();
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(5);
}
