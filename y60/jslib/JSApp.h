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
//   $RCSfile: JSApp.h,v $
//   $Author: martin $
//   $Revision: 1.6 $
//   $Date: 2005/02/16 12:22:21 $
//
//
//=============================================================================

#ifndef __Y60_JSAPP_INCLUDED__
#define __Y60_JSAPP_INCLUDED__

#include "y60_jslib_settings.h"

#include <y60/jsbase/jssettings.h>

#ifdef USE_LEGACY_SPIDERMONKEY
#include <js/spidermonkey/jsapi.h>
#else
#include <js/jsapi.h>
#endif

#include <asl/base/PlugInBase.h>
#include <asl/base/MappedBlock.h>
#include <asl/zip/PackageManager.h>
#include <vector>
#include <string>
#include <map>
#include <list>


struct JSStackFrame;

namespace jslib {

    extern JSContext * ourJSContext;

    class Y60_JSLIB_DECL JSApp {
        public:
            JSApp();
            virtual ~JSApp() {}
            static asl::PackageManagerPtr getPackageManager();
            static void setReportWarnings(bool theEnableFlag);
            static void setQuitFlag(bool theQuitFlag);
            static bool getQuitFlag();
            void setStrictFlag(bool theStrictFlag);
            void setJSVersion(int theVersion);
            void enableJIT(bool theFlag);
            void enableXML(bool theFlag);
            static void setProgramName(const std::string & theProgramName) {
                _myProgramName = theProgramName;
            }
            static const std::string & getProgramName() {
                return _myProgramName;
            }
            JSRuntime * initialize();
            void shutdown();
            static void ShellErrorReporter(JSContext *cx, const char *message,
                    JSErrorReport *report);

            int run(const std::string & theScriptFilename,
                    const std::string & theIncludePath,
                    const std::vector<std::string> & theScriptArgs);

            static JSObject * copyArguments(JSContext * theContext,
                 const std::vector<std::string> & theScriptArgs );
        protected:
            virtual bool initClasses(JSContext * theContext, JSObject * theGlobalObject);
        private:
            void Process(JSContext *cx, JSObject *obj, const char *filename);

            void setupPath(const std::string & theIncludePath);
            int processArguments(JSContext * theContext, JSObject * theObject,
                const std::string & theScriptFilename,
                const std::string & theIncludePath,
                const std::vector<std::string> & theScriptArgs);

            bool   ourStrictFlag;
            bool   ourXMLFlag;
            bool   ourJITFlag;
            int    ourJSVersion;
            static std::string _myProgramName;
    };
}
#endif
