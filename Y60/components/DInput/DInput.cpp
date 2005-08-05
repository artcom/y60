//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: VideoProcessingExtension.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.15 $
//
// Description:
//
//=============================================================================

#include <asl/PlugInBase.h>
#include <y60/JSScriptablePlugin.h>

#include <y60/EventDispatcher.h>

#ifdef WIN32
#include <y60/DIEventSource.h>
#endif

class DInputExtension :
    public asl::PlugInBase,
    public jslib::IScriptablePlugin
{
    public:

        DInputExtension(asl::DLHandle theDLHandle);
        const char * ClassName() {
            static const char * myClassName = "Y60DInput";
            return myClassName;
        }

    private:
#ifdef WIN32
    y60::DIEventSource   _myDIEventSource;
#endif

};

DInputExtension :: DInputExtension(asl::DLHandle theDLHandle) :
    asl::PlugInBase(theDLHandle)
{
#if WIN32
    y60::EventDispatcher::get().addSource(&_myDIEventSource);
#endif
    }


extern "C"
EXPORT asl::PlugInBase* Y60DInput_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new DInputExtension(myDLHandle);
}
