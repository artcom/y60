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

#ifndef AC_Y60_SHADER_LIBRARY_INCLUDED
#define AC_Y60_SHADER_LIBRARY_INCLUDED

#include "y60_glrender_settings.h"

#include "ShaderDescription.h"
#include "ShaderFeatureSet.h"
#include "GLShader.h"
//#include "Texture.h"

#include <y60/base/DataTypes.h>
#include <y60/scene/IShader.h>
#include <y60/scene/MaterialParameter.h>
#include <asl/dom/Nodes.h>
#include <asl/base/Ptr.h>

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

#ifndef _AC_NO_CG_
    struct CgContextHolder {
        CgContextHolder() : _myCgContext(0) {
            _myCgContext = cgCreateContext();
        }
        ~CgContextHolder();
        CGcontext get() {
            return _myCgContext;
        }
        CGcontext _myCgContext;
    };
#endif

    class Y60_GLRENDER_DECL ShaderLibrary : public IShaderLibrary {
        public:
            DEFINE_EXCEPTION(ShaderLibraryException, asl::Exception);
            ShaderLibrary();
            virtual ~ShaderLibrary();

            void prepare(const std::string & theShaderLibraryName,
                    std::string theVertexProfileName,
                    std::string theFragmentProfileName)
            {
                _myShaderLibraryNames.push_back(theShaderLibraryName);
                _myVertexProfileNames.push_back(theVertexProfileName);
                _myFragmentProfileNames.push_back(theFragmentProfileName);
            }

            void load(const std::string & theLibraryFileName, std::string theVertexProfileName, std::string theFragmentProfileName);
            void load(const std::string & theLibraryFileName) {
                load(theLibraryFileName,"","");
            }
            void load();

            void load(const dom::NodePtr theNode, std::string theVertexProfileName, std::string theFragmentProfileName);
            void load(const dom::NodePtr theNode) {
                load(theNode,"","");
            }

            void reload();

            virtual IShaderPtr findShader(MaterialBasePtr theMaterial);
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
            CgContextHolder _myContextHolder; // This must be deleted last
            GLShaderVector  _myShaders;
            static bool _myGLisReadyFlag;
#ifndef _AC_NO_CG_
            CgContextHolder           _myCgContext;
            std::vector<std::string> _myShaderLibraryNames;
            std::vector<std::string> _myVertexProfileNames;
            std::vector<std::string> _myFragmentProfileNames;
            std::string _myVertexProfileName;
            std::string _myFragmentProfileName;
#endif
    };

    typedef asl::Ptr<ShaderLibrary> ShaderLibraryPtr;

} // namespace y60

#endif // AC_Y60SHADER_LIBRARY_INCLUDED
