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
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "JSTask.h"
#include "JSTaskWindow.h"

#include <shlobj.h>
#include <string>

#include <asl/base/string_functions.h>
#include <asl/xpath/Path.h>
#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/Documentation.h>


using namespace std;

namespace y60 {

    class TaskManagerPlugin : public asl::PlugInBase, public jslib::IScriptablePlugin {
    public:
        TaskManagerPlugin(asl::DLHandle theDLHandle) : asl::PlugInBase(theDLHandle) {}
        
        virtual void initClasses(JSContext * theContext, JSObject *theGlobalObject) {
            JS_DefineFunctions(theContext, theGlobalObject, Functions());
            jslib::createFunctionDocumentation("TaskManager", Functions());

            jslib::JSTask::initClass(theContext, theGlobalObject);
            jslib::JSTaskWindow::initClass(theContext, theGlobalObject);
        }
        
        const char * ClassName() {
            static const char * myClassName = "TaskManager";
            return myClassName;
        }

    private:
        static int getFolderIDFromName(const string& theFolderName) {
            struct KeyValue {
                const char* key;
                int value;
            };
            static const KeyValue s_folderID[] = {
                {"BITBUCKET",     CSIDL_BITBUCKET},
                {"DESKTOP",       CSIDL_DESKTOP},
                {"PERSONAL",      CSIDL_PERSONAL},
                {"PROGRAM_FILES", CSIDL_PROGRAM_FILES},
                {"STARTUP",       CSIDL_STARTUP},
                {"SYSTEM",        CSIDL_SYSTEM},
                {"WINDOWS",       CSIDL_WINDOWS}
            };
            static unsigned int s_folderIDSize = sizeof(s_folderID) / sizeof(s_folderID[0]);

            string myUpperCaseFolderName = asl::toUpperCase(theFolderName); 
            for (unsigned int i = 0; i < s_folderIDSize; ++i) {
                if (s_folderID[i].key == myUpperCaseFolderName) {
                    return s_folderID[i].value;
                }
            }
            return -1;
        }

        static asl::Path getFolderPath(const string& theFolderName) {

            int nFolder = getFolderIDFromName(theFolderName);

            asl::Path myFolderPath;
            if (nFolder >= 0) {

                TCHAR szPath[MAX_PATH];
                //HRESULT hr = SHGetFolderPath(NULL, nFolder, NULL, 0, szPath);
                HRESULT hr = SHGetSpecialFolderPath(NULL, szPath, nFolder, FALSE);
                if (SUCCEEDED(hr)) {
                    myFolderPath = asl::Path(szPath, asl::Locale);
                    AC_DEBUG << "TaskManager::getFolderPath folder=" << theFolderName << " id=" << nFolder << " path=" << myFolderPath.toLocale();
                } else {
                    AC_WARNING << "Unable to GetFolderPath folder=" << theFolderName << " id=" << nFolder << " result=" << hr;
                }
            } else {
                AC_ERROR << "Unknown folder '" << theFolderName << "'";
            }

            return myFolderPath;
        }


        /*
         * JS binding
         */
        static JSBool getFolderPath(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
            DOC_BEGIN("Get path to well-known folder");
            DOC_PARAM("theFolderName", "Well-known folder name, e.g. DESKTOP, STARTUP, WINDOWS", jslib::DOC_TYPE_STRING);
            DOC_RVAL("Path", jslib::DOC_TYPE_STRING);
            DOC_END;
            if (argc == 1) {
                string myFolderName;
                if (jslib::convertFrom(cx, argv[0], myFolderName)) {
                    *rval = jslib::as_jsval(cx, getFolderPath(myFolderName).toUTF8());
                } else {
                    JS_ReportError(cx, "TaskManager::getFolderPath: could not convert string argument");
                }
            } else {
                JS_ReportError(cx, "TaskManager::getFolderPath: bad number of arguments");
            }
            return JS_TRUE;
        }

        JSFunctionSpec * Functions() {
            static JSFunctionSpec myFunctions[] = {
                {"getFolderPath", getFolderPath, 1},
                {0},
            };
            return myFunctions;
        }
    };
}

extern "C"
EXPORT asl::PlugInBase * TaskManager_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::TaskManagerPlugin(myDLHandle);
}
