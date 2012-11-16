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

#include "JSASSOscClient.h"

#include <y60/jsbase/IScriptablePlugin.h>
#include <asl/base/PlugInBase.h>

namespace y60 {

class ASSOscPlugin :
    public asl::PlugInBase,
    public jslib::IScriptablePlugin
{
    public:
        ASSOscPlugin (asl::DLHandle theDLHandle);
        virtual ~ASSOscPlugin();

        void initClasses(JSContext * theContext, JSObject *theGlobalObject);
        const char * ClassName() {
            static const char * myClassName = "ASSOscPlugin";
            return myClassName;
        }

};

} // end of namespace y60

using namespace std;
using namespace asl;

namespace y60 {

ASSOscPlugin::ASSOscPlugin(DLHandle theDLHandle) :
            PlugInBase(theDLHandle)
{
}

ASSOscPlugin :: ~ASSOscPlugin() {
}

void
ASSOscPlugin::initClasses(JSContext * theContext, JSObject *theGlobalObject) {
    jslib::JSASSOscClient::initClass(theContext, theGlobalObject);
}


} // end of namespace y60

extern "C"
EXPORT PlugInBase * ASSOscClient_instantiatePlugIn(DLHandle myDLHandle) {
    return new y60::ASSOscPlugin(myDLHandle);
}
