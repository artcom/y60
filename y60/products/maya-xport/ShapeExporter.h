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
//   $Author: christian $
//   $Revision: 1.14 $
//   $Date: 2005/04/14 16:56:16 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_maya_ShapeExporter_h_
#define _ac_maya_ShapeExporter_h_

#include "MaterialExporter.h"

#include <y60/scene/SceneBuilder.h>
#include <y60/base/DataTypes.h>
#include <asl/base/Ptr.h>

#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatArray.h>

#include <string>

#define ATTR_DUMP(x) // x

class ShapeExporter {

    public:
        ShapeExporter(MDagPath dagPath);
        virtual ~ShapeExporter();

        void appendToScene(y60::SceneBuilder & theSceneBuilder,
                           MaterialExporter & theMaterialExporter,
                           std::map<std::string, std::string>& myPathToIdMap);

        bool isInstanced() const {
            return _myInstanceNumber != 0;
        }

        unsigned getVertexCount() const {
            return _myMesh->numVertices();
        }

    private:
        void extractGeometry();
        void outputSets(y60::ShapeBuilder & theShape, y60::SceneBuilder & theSceneBuilder);
        void outputPositions(y60::ShapeBuilder & theShape);
        bool outputPrimitives(y60::ShapeBuilder & theShape, y60::SceneBuilder & theSceneBuilder,
                              MaterialExporter & theMaterialExporter, bool theHasSkinAndBonesFlag);
        void outputNormals(y60::ShapeBuilder & theShape);
        void outputColors(y60::ShapeBuilder & theShape);
        void outputUVs(y60::ShapeBuilder & theShape);
        void getUVSetNamesFromTextures(const y60::VectorOfString & theTextures,
                                       y60::VectorOfString & theUVSetNameList);

        bool outputJointWeights(y60::ShapeBuilder & theShape);

        void checkVertexCountPerFace(unsigned theVertexCount, MStatus & theStatus);

        // members
        MPointArray         _myVertexArray;
        MColorArray         _myColorArray;
        MFloatVectorArray   _myNormalArray;

        MFnMesh *           _myMesh;
        MDagPath *          _myDagPath;
        int                 _myInstanceNumber;
        MObjectArray        _myPolygonSets;
        MObjectArray        _myPolygonComponents;

        std::vector<int>    _myColorIndices;

        std::map<std::string, std::string>  _myTextureUVSetMap;

        bool                _myDoubleSidedFlag;
        bool                _myOppositeFlag;
};

typedef asl::Ptr<ShapeExporter> ShapeExporterPtr;

inline
bool
isValidColor(const MColor & theColor) {
    return (theColor.r >= 0 && theColor.g >= 0 && theColor.b >= 0 && theColor.a >= 0);
}
#endif // _ac_maya_ShapeExporter_h_
