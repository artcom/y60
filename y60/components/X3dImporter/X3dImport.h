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

#include "y60_x3dimporter_settings.h"

#include <asl/base/Exception.h>
#include <asl/base/Stream.h>
#include <asl/dom/Nodes.h>
#include <y60/scene/SceneBuilder.h>
#include <y60/scene/WorldBuilder.h>
#include <y60/scene/MaterialBuilder.h>
#include <y60/scene/ShapeBuilder.h>
#include <y60/scene/ElementBuilder.h>

#include <asl/base/PlugInBase.h>
#include <y60/scene/ISceneDecoder.h>

namespace y60 {

    const std::string MIME_TYPE_X3D = "model/x3d";

    class X3dImport : public ISceneDecoder, public asl::PlugInBase
    {
    public:
        DEFINE_NESTED_EXCEPTION(y60::X3dImport, ImportException, asl::Exception);

        X3dImport(asl::DLHandle theDLHandle)
            : PlugInBase(theDLHandle) { }

        ~X3dImport() { }

      bool decodeScene(asl::Ptr<asl::ReadableStreamHandle> theSource, dom::DocumentPtr theScene);
        //bool encodeScene(const dom::DocumentPtr theScene, asl::WriteableStream * theStream);
        virtual bool setProgressNotifier(IProgressNotifierPtr theNotifier);
        std::string canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theSource);
        const char * ClassName() {
            static const char * myClassName = "StlPlugIn";
            return myClassName;
        }
        bool setLazy(bool lazy) {
            if (lazy) {
                AC_WARNING << "X3dImport::decodeScene: lazy loading not supported";
            }
            return false;
        }
        bool addSource(asl::Ptr<asl::ReadableStreamHandle> theSource) {
            AC_WARNING << "X3dImport::addSource: additional sources not supported";
            return false;
        }
    private:
        void processNode(const dom::NodePtr theNode);

        void processGroupNode(const dom::NodePtr theNode);
        void processTransformNode(const dom::NodePtr theNode);
        void processSwitchNode(const dom::NodePtr theSwitchNode);
        
        void processShapeNode(const dom::NodePtr theShapeNode);

        void processAppearanceNode(const dom::NodePtr theAppearanceNode, std::string & theMaterialId);
        void processMaterialNode(const dom::NodePtr theMaterialNode,
                                 y60::MaterialBuilder & theMaterialBuilder);
        void processImageTextureNode(const dom::NodePtr theImageTextureNode,
                                     y60::MaterialBuilder & theMaterialBuilder);

        void processGeometryNode(const dom::NodePtr theNode,
                                 const std::string & theMaterialId);
        void processIndexedFaceSetNode(const dom::NodePtr theIndexedFaceSetNode,
                                       const std::string & theMaterialId);

        template <class T>
        unsigned createVertices(y60::ShapeBuilder & theShapeBuilder,
                                const std::string & theVertices,
                                const y60::VertexDataRole & theRole,
                                const unsigned theVertexCount = 0);

        unsigned createIndices(y60::ElementBuilder & theElementBuilder,
                               const char * thePrimitiveType,
                               const std::string & theIndices,
                               const y60::VertexDataRole & theRole,
                               bool ccw = true);

        void countNodesProcessed(unsigned theCount);
        unsigned getDeepNodeCount(const dom::Node & theNode) const;

        y60::WorldBuilderBase * _myCurrentTransform;

        y60::SceneBuilderPtr _mySceneBuilder;
        y60::WorldBuilderPtr _myWorldBuilder;
        IProgressNotifierPtr _myProgressNotifier;
        unsigned _myNodesToProcess;
        unsigned _myNodesProcessed;
    };

}



