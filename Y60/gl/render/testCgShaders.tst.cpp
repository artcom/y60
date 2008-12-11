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
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "CgShader.h"
#include "CgProgramInfo.h"
#include "ShaderLibrary.h"
#include "ShaderTypes.h"
#include "ShaderLibrary.h"

#include <asl/base/Exception.h>
#include <Cg/cg.h>
#include <iostream>

using namespace std;
using namespace y60;

void
testShaderLibrary(const std::string & theLibraryFileName, const std::string theVertexProfileName, const std::string theFragmentProfileName) {
    cerr << "Testing shader library file'"<< theLibraryFileName
         << " with vertex profile '" << theVertexProfileName 
         << "' and fragment profile '" << theFragmentProfileName  << "'" << endl;
    ShaderLibrary myShaderLibrary;
    myShaderLibrary.load(theLibraryFileName, theVertexProfileName, theFragmentProfileName);
    GLShaderVector myShaders = myShaderLibrary.getShaders();
    if (myShaders.empty()) {
        throw asl::Exception("Error in testCgShaders: no shaders found in library " + theLibraryFileName);
    }

    for (unsigned i = 0; i < myShaders.size(); ++i) {
        CgShaderPtr myShader = dynamic_cast_Ptr<CgShader>(myShaders[i]);
        if (myShader) {
            const VectorOfString & myFragmentShaders = myShader->getShader(FRAGMENT_SHADER)._myPossibleProfileNames;
            const VectorOfString & myVertexShaders = myShader->getShader(VERTEX_SHADER)._myPossibleProfileNames;
            bool matchesFragmentShader = 
                (std::find(myFragmentShaders.begin(), myFragmentShaders.end(), theFragmentProfileName) != myFragmentShaders.end());
            bool matchesVertexShader = 
                (std::find(myVertexShaders.begin(), myVertexShaders.end(), theVertexProfileName) != myVertexShaders.end());
            if (matchesFragmentShader != matchesVertexShader) {
                cerr << "##### WARNING: vertex and fragment profiles of different generation";
            }
            if (matchesFragmentShader || matchesVertexShader) {
                cerr << "testing shader " << myShader->getName() << " (profiles = " << theVertexProfileName << "/" << theFragmentProfileName << ")" << endl;
                myShader->compile(myShaderLibrary);
            }
        }
    }
}

int main(int argc, char * argv[]) {
    const std::string myLibraryDir = "../../../../shader/";
//const std::string myLibraryDir = "../../shader/";

    try {
        testShaderLibrary(myLibraryDir+"shaderlibrary_nocg.xml","","");
        
        std::vector<std::pair<std::string, std::string> > myProfiles;
        myProfiles.push_back(std::make_pair("arbvp1", "arbfp1"));
        myProfiles.push_back(std::make_pair("vp20", "fp20"));
        myProfiles.push_back(std::make_pair("vp30", "fp30"));
        myProfiles.push_back(std::make_pair("vp40", "fp40"));
        myProfiles.push_back(std::make_pair("gp4vp", "gp4fp"));
        
        for (std::vector<std::pair<std::string, std::string> >::size_type i = 0; i < myProfiles.size(); ++i) {
            //testShaderLibrary(myLibraryDir+"toonshaders.xml", myProfiles[i].first, myProfiles[i].second);
            testShaderLibrary(myLibraryDir+"shaderlibrary.xml", myProfiles[i].first, myProfiles[i].second);
        }
        return 0;
    } catch (const asl::Exception & ex) {
        cerr << "### Exception caught: " << ex;
        return -1;
    }
}
