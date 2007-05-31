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
//   $RCSfile: AnimationExporter.h,v $
//   $Author: pavel $
//   $Revision: 1.20 $
//   $Date: 2005/04/24 00:41:20 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_c4d_AnimationExporter_h_
#define _ac_c4d_AnimationExporter_h_


#include <y60/iostream_functions.h>
#include <y60/AnimationBuilder.h>
#include <y60/SceneBuilder.h>
#include <y60/ClipBuilder.h>

#include "c4d_include.h"

#include <string>
#include <vector>
#include <set>
#include <map>

#undef DB
#define DB(x) // x

namespace y60 {
    class SceneBuilder;
}
class SceneExporter;

typedef std::map<std::string, std::string> AttributeMap;
typedef std::map<std::string, std::vector<float> > RotationMap;
class AnimationExporter {
    public:
        AnimationExporter(y60::SceneBuilder & theSceneBuilder, BaseDocument * theDocument);
        virtual ~AnimationExporter();

        void exportCharacter(BaseObject * theNode, const std::string & theNodeId);
        void exportGlobal(BaseObject * theNode, const std::string & theNodeId,
                          y60::ClipBuilderPtr theClipBuilder = y60::ClipBuilderPtr(0));

    private:
        bool WriteTrack(CTrack* theTrack, const std::string & theNodeId, 
                        y60::ClipBuilderPtr theClipBuilder);
        bool WriteCurve(CCurve* theCurve, std::vector<float> & theValues, bool theInverseFlag);
        bool exportRotationAnimation(const std::string & theNodeId,
                                     y60::ClipBuilderPtr theClipBuilder);
        y60::SceneBuilder & _mySceneBuilder;
        BaseDocument     *  _myDocument;
        AttributeMap        _myAttributeMap;
        RotationMap         _myRotationMap;
};


#endif
