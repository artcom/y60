//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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

#include "c4d_include.h"
#include "MaterialExporter.h"

#include <asl/linearAlgebra.h>

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
