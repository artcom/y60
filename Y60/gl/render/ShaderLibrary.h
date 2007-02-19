#ifndef AC_Y60_SHADER_LIBRARY_INCLUDED
#define AC_Y60_SHADER_LIBRARY_INCLUDED

#include "ShaderDescription.h"
#include "ShaderFeatureSet.h"
#include "GLShader.h"
//#include "Texture.h"

#include <y60/DataTypes.h>
#include <y60/IShader.h>
#include <y60/MaterialParameter.h>
#include <dom/Nodes.h>
#include <asl/Ptr.h>

#ifndef _AC_NO_CG_
// CG support
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#endif

#include <string>
namespace y60 {

    struct ShaderScore {
        ShaderScore() : featurehits(0), points(0) {};
        unsigned featurehits;
        float    points;
    };

    class ShaderLibrary : public y60::IShaderLibrary {
        public:
            DEFINE_EXCEPTION(ShaderLibraryException, asl::Exception);
            ShaderLibrary();
            virtual ~ShaderLibrary();

            void load(const std::string & theLibraryFileName, std::string theVertexProfileName, std::string theFragmentProfileName); 
            void load(const std::string & theLibraryFileName) {
                load(theLibraryFileName,"","");
            }

            void load(const dom::NodePtr theNode, std::string theVertexProfileName, std::string theFragmentProfileName);
            void load(const dom::NodePtr theNode) {
                load(theNode,"","");
            }
            void reload();
            virtual y60::IShaderPtr findShader(MaterialBasePtr theMaterial);
            const GLShaderVector & getShaders() const { return _myShaders; };
#ifndef _AC_NO_CG_
            CGcontext getCgContext();
#endif
            static void setGLisReadyFlag(bool theFlag=true) {
                _myGLisReadyFlag = theFlag;
            }
            static bool GLisReady() {
                return _myGLisReadyFlag;
            }
            virtual const std::string & getVertexProfileName() {
                return _myVertexProfileName;
            }
            virtual const std::string & getFragmentProfileName() {
                return _myFragmentProfileName;
            }
        private:
            void loadAllShaders();
            GLShaderVector  _myShaders;
            static bool _myGLisReadyFlag;
#ifndef _AC_NO_CG_
            CGcontext       _myCgContext;
            std::string _myVertexProfileName;
            std::string _myFragmentProfileName;
#endif
    };

    typedef asl::Ptr<ShaderLibrary> ShaderLibraryPtr;


} // namespace y60

#endif // AC_Y60SHADER_LIBRARY_INCLUDED
