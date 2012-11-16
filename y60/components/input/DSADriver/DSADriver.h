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
//    $RCSfile: DSADriver.h,v $
//     $Author: ulrich $
//   $Revision: 1.12 $
//       $Date: 2005/02/03 16:59:51 $
//
// Dieters Sensor Array...
//
//=============================================================================

#ifndef _DSA_DRIVER_INCLUDED
#define _DSA_DRIVER_INCLUDED

#include "y60_dsadriver_settings.h"

#include "SensorServer.h"
#include "SensorArray.h"

#include <y60/jsbase/JSScriptablePlugin.h>
#include <y60/jsbase/JSWrapper.h>
#include <y60/input/IEventSource.h>
#include <asl/base/PlugInBase.h>

#include <vector>
#include <map>

class DSADriver :
    public asl::PlugInBase,
    public y60::IEventSource,
    public jslib::IScriptablePlugin
{
public:
    DSADriver(asl::DLHandle theDLHandle);

    void onUpdateSettings(dom::NodePtr theConfiguration);

    void init() {};
    std::vector<y60::EventPtr> poll();

    const char * ClassName() {
        static const char * myClassName = "DSADriver";
        return myClassName;
    }

private:
    bool             _myInterpolateFlag;

    typedef std::map<int, SensorServerPtr> SensorServerList;
    SensorServerList _mySensorServers;

    typedef std::map<int, SensorArrayPtr> SensorArrayList;
    SensorArrayList  _mySensorArray;
};

#endif
