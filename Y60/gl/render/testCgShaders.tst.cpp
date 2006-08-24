#include "CgProgramInfo.h"
#include "ShaderLibrary.h"
#include "ShaderTypes.h"
#include "ShaderLibrary.h"
#include "CGShader.h"

#include <asl/Exception.h>
#include <Cg/cg.h>
#include <iostream>

using namespace std;
using namespace y60;

void
testShaderLibrary(const std::string & theLibraryFileName) {
    cerr << "Testing shader library file'"<<theLibraryFileName<<endl;
    ShaderLibrary myShaderLibrary;
    myShaderLibrary.load(theLibraryFileName );
    GLShaderVector myShaders = myShaderLibrary.getShaders();
    if (myShaders.empty()) {
        throw asl::Exception("Error in testCgShaders: no shaders found in library " + theLibraryFileName);
    }

    for (unsigned i = 0; i < myShaders.size(); ++i) {
        CGShaderPtr myShader = dynamic_cast_Ptr<CGShader>(myShaders[i]);
        if (myShader) {
            cerr << "testing shader " << myShader->getName() << endl;
            myShader->compile(myShaderLibrary);
        }
    }
}

int main(int argc, char * argv[]) {
    const std::string myLibraryDir = "../../../../shader/";

    try {
        testShaderLibrary(myLibraryDir+"shaderlibrary.xml");
        testShaderLibrary(myLibraryDir+"toonshaders.xml");
        testShaderLibrary(myLibraryDir+"shaderlibrary_nocg.xml");
        return 0;
    } catch (const asl::Exception & ex) {
        cerr << "### Exception caught: " << ex;
        return -1;
    }
}
