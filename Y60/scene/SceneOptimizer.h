//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_scene_SceneOptimizer_h_
#define _ac_scene_SceneOptimizer_h_

#include "Scene.h"
#include "Body.h"
#include "Shape.h"
#include <asl/Matrix4.h>

namespace y60 {

    class SceneOptimizer {
        public:
            SceneOptimizer(Scene & theScene);
            void run();
            void run(const dom::NodePtr & theRootNode);

        private:
            SceneOptimizer();

            typedef std::map<std::string, unsigned> VertexDataMap;
            typedef std::map<std::string, std::vector<std::string> > RoleMap;

            class PrimitiveCache {
                public:
                    PrimitiveCache(dom::NodePtr & thePrimitive) : _myPrimitives(thePrimitive)
                    {}
                    dom::NodePtr getIndex(const std::string & theName, const std::string & theRole);

                private:
                    dom::NodePtr _myPrimitives;
                    typedef std::map<std::string, dom::NodePtr> NodeMap;
                    NodeMap _myIndices;
            };
            typedef asl::Ptr<PrimitiveCache> PrimitiveCachePtr;

            class SuperShape {
                public:
                    SuperShape(Scene & theScene);
                    dom::NodePtr getVertexData(const std::string & theType, const std::string & theName);
                    PrimitiveCachePtr getPrimitive(const std::string & theType, const std::string & theMaterialRef, 
                                                   const RenderStyles & theRenderStyles, unsigned theAdditionalKeyValue);
                    std::string getShapeId() const;
                    dom::NodePtr getShapeNode();

                private:
                    std::map<std::string, PrimitiveCachePtr> _myPrimitiveMap;
                    std::map<std::string, dom::NodePtr> _myVertexDataMap;
                    dom::NodePtr _myShapeNode;
            };
            typedef asl::Ptr<SuperShape> SuperShapePtr;

            void runNode(dom::NodePtr & theRootNode);
            void cleanupScene(dom::NodePtr & theNode);
            void mergeVertexData(const dom::NodePtr & theVertexData, bool theFlipFlag, const asl::Matrix4f & theMatrix,
                                 const RoleMap & theRoles, VertexDataMap & theVertexDataOffsets);
            void mergePrimitives(const dom::NodePtr & theElements, bool theFlipFlag, VertexDataMap & theVertexDataOffsets,
                                 const RenderStyles & theRenderStyles);
            bool mergeBodies(dom::NodePtr & theNode, const asl::Matrix4f & theMatrix);
            void removeInvisibleNodes(dom::NodePtr & theNode);
            void pinAnimatedNodes(dom::NodePtr & theRootNode);
            void pinTransparentBodies(dom::NodePtr & theRootNode);
            void pinBodiesWithSameShapes(dom::NodePtr & theRootNode);
            void collectIds(const dom::NodePtr & theNode, std::set<std::string> & theIds);
            void collectShapeIds(const dom::NodePtr & theNode, std::set<std::string> & theIds);
            void removeUnusedShapes();
            void transformToParent(dom::NodePtr & theNode);
            void convertToTransformNode(dom::NodePtr & theNode);

            template <class T>
            unsigned copyVertexData(dom::NodePtr & theSrcVertexData, dom::NodePtr & theDstVertexData);
            unsigned transformVertexData(dom::NodePtr & theSrcVertexData, dom::NodePtr & theDstVertexData,
                                         bool theFlipFlag, const asl::Matrix4f & theMatrix, bool theNormalFlag = false);

            Scene & _myScene;
            dom::NodePtr _myRootNode;

            typedef std::map<std::string, SuperShapePtr> SuperShapeMap;
            SuperShapeMap _myShapes;
            SuperShapePtr _mySuperShape;

            typedef std::vector<dom::NodePtr> NodeVector;
            NodeVector _myStickyNodes;

            std::string _myOptimizedBodyName;
    };

    template <class T>
    unsigned
    SceneOptimizer::copyVertexData(dom::NodePtr & theSrcVertexData, dom::NodePtr & theDstVertexData) {
        const std::vector<T> & mySrc = theSrcVertexData->firstChild()->dom::Node::nodeValueRef<std::vector<T> >();
        std::vector<T> & myDst = theDstVertexData->firstChild()->dom::Node::nodeValueRefOpen<std::vector<T> >();
        unsigned myOffset = myDst.size();

        myDst.resize(mySrc.size() + myOffset);

        for (unsigned j = 0; j < mySrc.size(); ++j) {
           myDst[myOffset + j] = mySrc[j];
        }

        theDstVertexData->firstChild()->dom::Node::nodeValueRefClose<std::vector<T> >();
        return myOffset;
    }
}

#endif
