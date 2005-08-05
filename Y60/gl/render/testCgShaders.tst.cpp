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
compileShader(const ShaderDescription & myShader, 
              const CGcontext theCgContext,
              const std::string & theShaderDir)
    {
        std::string myPathName = theShaderDir + myShader._myFilename; 
        CGprofile myCgProfile = CgProgramInfo::asCgProfile(myShader);
        
        CGprogram myCgProgramID = cgCreateProgramFromFile(theCgContext, CG_SOURCE,
                                                            myPathName.c_str(),
                                                            myCgProfile,
                                                            myShader._myEntryFunction.c_str(), 0);

        CGerror myCgError = cgGetError();
        if (myCgError != CG_NO_ERROR) {
            std::string myErrorMessage = "Cg error loading '" + myPathName + "': "
                                        + cgGetErrorString(myCgError);
            if (myCgError == CG_COMPILER_ERROR) {
                myErrorMessage += "\n" + std::string(cgGetLastListing(theCgContext));
            }
            throw asl::Exception(myErrorMessage);
        }
        cgDestroyProgram(myCgProgramID);
    } 

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
