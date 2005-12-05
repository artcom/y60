//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: LightExporter.h,v $
//   $Author: david $
//   $Revision: 1.1 $
//   $Date: 2003/09/09 11:28:15 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_maya_LightExporter_h_
#define _ac_maya_LightExporter_h_

#include "MaterialExporter.h"

#include <y60/SceneBuilder.h>
#include <y60/DataTypes.h>

#include <maya/MFnLight.h>

#include <string>

class LightExporter {

    public:
        LightExporter(MDagPath dagPath);
        virtual ~LightExporter();
        
        void appendToScene(y60::SceneBuilder & theSceneBuilder,
                           std::map<std::string, std::string>& thePathToIdMap );

    private:
        MDagPath  * _myDagPath;
        MFnLight  * _myLight;
        
        unsigned    _myInstanceNumber;
};

#endif // _ac_maya_LightExporter_h_
