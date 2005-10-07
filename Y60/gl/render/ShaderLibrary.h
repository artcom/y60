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

// CG support
#include <Cg/cg.h>
#include <Cg/cgGL.h>

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

            void load(const std::string & theLibraryFileName);
            void load(const dom::NodePtr theNode);
            void reload();
            virtual y60::IShaderPtr findShader(MaterialBasePtr theMaterial);

            const std::string & getShaderDir() const { return _myShaderDirectory; };
            void setShaderDir(const std::string & theShaderDir) { _myShaderDirectory = theShaderDir; };
            const GLShaderVector & getShaders() const { return _myShaders; };
            CGcontext getCgContext();
         private:
            void loadAllShaders();
            GLShaderVector  _myShaders;
            std::string     _myShaderDirectory;
		    CGcontext       _myCgContext;
		    dom::NodePtr    _myShaderList;

     };

    typedef asl::Ptr<ShaderLibrary> ShaderLibraryPtr;


} // namespace y60

#endif // AC_Y60SHADER_LIBRARY_INCLUDED
