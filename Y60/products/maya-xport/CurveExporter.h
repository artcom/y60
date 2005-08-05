//=============================================================================
// Copyright (C) 2000-2002, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: CurveExporter.h,v $
//   $Author: christian $
//   $Revision: 1.3 $
//   $Date: 2005/03/24 23:36:02 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_maya_CurveExporter_h_
#define _ac_maya_CurveExporter_h_

namespace y60 {
    class SceneBuilder;
    class WorldBuilderBase;
}

class MFnDagNode;

void clearCurveMaterials();
void exportCurve(MFnDagNode & theDagNode, y60::WorldBuilderBase & theParentTransform, y60::SceneBuilder & theSceneBuilder);

#endif
