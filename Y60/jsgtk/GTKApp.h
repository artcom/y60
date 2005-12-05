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
//   $RCSfile: GTKApp.h,v $
//   $Author: david $
//   $Revision: 1.4 $
//   $Date: 2005/01/25 11:58:15 $
//
//
//=============================================================================

#ifndef __Y60_GTKAPP_INCLUDED__
#define __Y60_GTKAPP_INCLUDED__

#include "JSGladeSignalAdapter.h"

#include <y60/JSApp.h>
#include <y60/JScppUtils.h>

class GTKApp : public jslib::JSApp {
    public:
        GTKApp();
        static void addSignalHandler(const gchar *handler_name,
                   GObject *object,
                   const gchar *signal_name,
                   const gchar *signal_data,
                   GObject *connect_object,
                   gboolean after,
                   gpointer theTargetJSObject);
        static gulong deleteSignalHandler(GObject* theSource, gpointer theUserData);

                

    protected:
        virtual bool initClasses(JSContext * theContext, JSObject * theGlobalObject);
    private:
        typedef std::map<GObject*, JSGladeSignalAdapterVector> SignalAdapterMap;
        static SignalAdapterMap _mySignalAdapterMap_;
};
   
#endif
