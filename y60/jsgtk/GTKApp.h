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
//   $RCSfile: GTKApp.h,v $
//   $Author: david $
//   $Revision: 1.4 $
//   $Date: 2005/01/25 11:58:15 $
//
//
//=============================================================================

#ifndef __Y60_GTKAPP_INCLUDED__
#define __Y60_GTKAPP_INCLUDED__

#include "y60_jsgtk_settings.h"

#include "JSGladeSignalAdapter.h"

#include <y60/jslib/JSApp.h>
#include <y60/jsbase/JScppUtils.h>

namespace jslib {

class Y60_JSGTK_DECL GTKApp : public jslib::JSApp {
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

}

#endif
