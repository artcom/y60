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
//   $RCSfile: JSApp.h,v $
//   $Author: martin $
//   $Revision: 1.6 $
//   $Date: 2005/02/16 12:22:21 $
//
//
//=============================================================================

#ifndef __Y60_JSAPP_INCLUDED__
#define __Y60_JSAPP_INCLUDED__

#include <y60/jsbase/jssettings.h>
#include <js/spidermonkey/jsapi.h>

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

    class JSApp {
        public:
            JSApp();
            static asl::PackageManagerPtr getPackageManager();
            static void setReportWarnings(bool theEnableFlag);
            static void setQuitFlag(bool theQuitFlag);
            static bool getQuitFlag();
            void setStrictFlag(bool theStrictFlag);
            void setJSVersion(int theVersion);
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
            int    ourJSVersion;
            static std::string _myProgramName;
    };
}
#endif
