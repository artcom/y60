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

#include <y60/scene/SceneBuilder.h>
#include <y60/base/DataTypes.h>

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
