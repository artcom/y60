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
*/

#ifndef _ac_scene_SceneOptimizer_h_
#define _ac_scene_SceneOptimizer_h_

#include "y60_scene_settings.h"

#include "Scene.h"
#include "Body.h"
#include "Shape.h"
#include <asl/math/Matrix4.h>

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
                                 const RenderStyles & theRenderStyles, unsigned theBeginIndex, unsigned theEndIndex);
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
