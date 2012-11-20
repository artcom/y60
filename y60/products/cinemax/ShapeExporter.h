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
//   $RCSfile: ShapeExporter.h,v $
//   $Author: valentin $
//   $Revision: 1.14 $
//   $Date: 2005/04/29 15:07:34 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_c4d_ShapeExporter_h_
#define _ac_c4d_ShapeExporter_h_

#include "MaterialExporter.h"
#include <asl/math/linearAlgebra.h>

#include <string>
#include <vector>

namespace y60 {
    class SceneBuilder;
    class ShapeBuilder;
    class ElementBuilder;
}

namespace asl {
    template <class Number>
    class VertexNormalBuilder;
}

class SelectionTag;
struct Vector;

class ShapeExporter {
    public:
        ShapeExporter(y60::SceneBuilder & theSceneBuilder, MaterialExporter & theMaterialExporter);
        ~ShapeExporter() {};

        std::vector<std::string> writeShape(BaseObject * theNode, BaseObject * thePolygonizedNode,
                                            bool theForceFrontBackFacing = false);

    private:
        y60::SceneBuilder & _mySceneBuilder;
        MaterialExporter & _myMaterialExporter;

        std::string writeSelection(BaseObject * theNode, BaseObject * thePolygonNode,
                                   SelectionTag * theSelection,
                                   SelectionTag * theBaseSelection,
                                   const TextureList & theTextureList,
                                   const UVTagList & theUVTagList,
                                   bool theForceFrontBackFacing);

        void setupNormalBuilder(asl::VertexNormalBuilder<float> & theVertexNormalBuilder,
                                BaseObject * theNode);

        void findExtrema(asl::Vector3f & theVector, const Vector & theVertex, bool theMinFlag);

        unsigned appendVertexNormal(asl::VertexNormalBuilder<float> & theVertexNormalBuilder,
                                    y60::ShapeBuilder & theShapeBuilder,
                                    const asl::Vector3f & theFaceNormal,
                                    long theVertexIndex, float theSmoothingAngle);

        void appendVertexIndices(y60::ElementBuilder & theElementBuilder,
                                 int thePositionIndex, int theColorIndex, int theNormalIndex,
                                 std::vector<int> theUVCoordIndex, int theTextureCount);
};

#endif
