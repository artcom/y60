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

 try {
    use("Exception.js");
    use("SceneViewer.js");

    var myArgs = parseArguments(arguments, { "multisamples" : "%s",
                                             "m"            : "%s" });
    var myMultisampling = 8;
    if ("multisamples" in myArgs.options) {
        myMultisampling = Number(myArgs.options.multisamples);
    }

    if ("m" in myArgs.options) {
        myMultisampling = Number(myArgs.options.m);
    }

    if ("help" in myArgs.options) {
        print("ART+COM SceneViewer Usage: sv filename.[x60/b60/d60] [options]");
        print("available options:");
        print(" --help                         - print this help message");
        print(" --m or --multisamples [number] - specify the Number of multisamples.");
        print("                                  Must a be power of two.");
    }

    window = new RenderWindow();
    if (operatingSystem() == "LINUX") {
        window.multisamples = Math.min(4, myMultisampling);
    } else {
        window.multisamples = myMultisampling;
    }

    var ourShow = new SceneViewer(arguments);
    var myModel = ourShow.getModelName();

    if (myModel && myModel.search(/\.st./) != -1) {
        plug("StlImporter");
    }
    if (myModel && myModel.search(/\.x3d/) != -1) {
        plug("X3dImporter");
    }

    ourShow.setup(1024,768);
    ourShow.go();
} catch (ex) {
    reportException(ex);
}
