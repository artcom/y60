//=============================================================================
// Copyright (C) 2004-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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
