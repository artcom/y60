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

#include "y60_scene_settings.h"

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
        VertexIndexBin() : indices() {}

        std::string           vertexdata;
        VertexDataRole   role;
        //VectorOfUnsignedInt * indices;
        dom::NodePtr          indices;
    };

    typedef std::vector<VertexIndexBin> VertexIndexBinMap;

    class Y60_SCENE_DECL ElementBuilder : public BuilderBase {
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

