#include "CgShader.h"
#include "CgProgramInfo.h"
#include "ShaderLibrary.h"
#include "ShaderTypes.h"
#include "ShaderLibrary.h"

#include <y60/glrender/y60glrender_paths.h>

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

#define SHADER_DIR CMAKE_CURRENT_SOURCE_DIR"/../shader/"

//    const std::string myLibraryDir = "../../../shader/";
//const std::string myLibraryDir = "../../shader/";

    try {
        testShaderLibrary(SHADER_DIR "shaderlibrary_nocg.xml","","");

        std::vector<std::pair<std::string, std::string> > myProfiles;
        myProfiles.push_back(std::make_pair("arbvp1", "arbfp1"));
        myProfiles.push_back(std::make_pair("vp20", "fp20"));
        myProfiles.push_back(std::make_pair("vp30", "fp30"));
        myProfiles.push_back(std::make_pair("vp40", "fp40"));
        myProfiles.push_back(std::make_pair("gp4vp", "gp4fp"));

        for (std::vector<std::pair<std::string, std::string> >::size_type i = 0; i < myProfiles.size(); ++i) {
            //testShaderLibrary(myLibraryDir+"toonshaders.xml", myProfiles[i].first, myProfiles[i].second);
            testShaderLibrary(SHADER_DIR "shaderlibrary.xml", myProfiles[i].first, myProfiles[i].second);
        }
        return 0;
    } catch (const asl::Exception & ex) {
        cerr << "### Exception caught: " << ex;
        return -1;
    }
}
