//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: ElementBuilder.h,v 1.2 2005/04/13 09:41:41 christian Exp $
//   $RCSfile: ElementBuilder.h,v $
//   $Author: christian $
//   $Revision: 1.2 $
//   $Date: 2005/04/13 09:41:41 $
//
//
//  Description: XML-File-Export Plugin
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef _ac_y60_PrimitivesBuiler_h_
#define _ac_y60_PrimitivesBuiler_h_

#include "BuilderBase.h"
#include <y60/base/VertexDataRoles.h>
#include <y60/base/typedefs.h>
#include <y60/base/DataTypes.h>
#include <asl/base/Ptr.h>
#include <asl/dom/Nodes.h>
#include <asl/dom/Value.h>
#include <vector>
#include <string>
#include <map>

namespace y60 {

    typedef dom::ValueWrapper<VectorOfUnsignedInt>::Type IndicesValueType;
    typedef asl::Ptr<IndicesValueType,dom::ThreadingModel> IndicesValueTypePtr;

    typedef dom::ValueWrapper<VectorOfSignedInt>::Type HalfEdgesValueType;
    typedef asl::Ptr<HalfEdgesValueType, dom::ThreadingModel> HalfEdgesValueTypePtr;

    struct VertexIndexBin {
        VertexIndexBin() : indices(0) {}

        std::string           vertexdata;
        VertexDataRole   role;
        //VectorOfUnsignedInt * indices;
        dom::NodePtr          indices;
    };

    typedef std::vector<VertexIndexBin> VertexIndexBinMap;

    class ElementBuilder : public BuilderBase {
        public:
            ElementBuilder(const std::string & thePrimitiveType,
                           const std::string & theMaterial);

            virtual ~ElementBuilder() {
            }

            void copyIndexBin(const VertexDataRole & theSourceRole,
                              const VertexDataRole & theTargetRole,
                              const std::string & theTargetVertexDataName);

            dom::NodePtr createIndex(const std::string & theVertexDataName,
                             VertexDataRole theRole, unsigned theSize = 0);

            void appendIndex(VertexDataRole theRole, unsigned theIndex);
            void appendHalfEdge(int theTwin);
            dom::NodePtr createHalfEdges();
            int getHalfEdgeSize() const;
            const std::string & getMaterialId() const {
                return _myMaterialId;
            }
            const std::string & getType() const {
                return _myPrimitiveType;
            }

            const VertexIndexBinMap & getIndexBins() const {
                return _myVertexIndexBins;
            }

            void generateHalfEdges();

        private:
            const std::string     _myPrimitiveType;
            const std::string     _myMaterialId;
            VertexIndexBinMap     _myVertexIndexBins;
            std::vector<int>      _myHalfEdges;

    };

    typedef asl::Ptr<ElementBuilder>  ElementBuilderPtr;
}

#endif

