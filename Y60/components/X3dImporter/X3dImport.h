
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: X3dImport.h,v 1.3 2005/04/21 17:15:24 danielk Exp $
//   $Author: danielk $
//   $Revision: 1.3 $
//   $Date: 2005/04/21 17:15:24 $
//
// X3D importer.
//
//=============================================================================


#include <asl/Exception.h>
#include <asl/Stream.h>
#include <dom/Nodes.h>
#include <y60/SceneBuilder.h>
#include <y60/WorldBuilder.h>
#include <y60/MaterialBuilder.h>
#include <y60/ShapeBuilder.h>
#include <y60/ElementBuilder.h>

#include <asl/PlugInBase.h>
#include <y60/ISceneDecoder.h>

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



