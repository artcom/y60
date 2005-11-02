//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSdirectory_functions.cpp,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2005/02/10 12:26:15 $
//
//=============================================================================

#include "JSdirectory_functions.h"
#include "Documentation.h"

#include <asl/directory_functions.h>
#include <asl/os_functions.h>


using namespace std;

namespace jslib {

static JSBool
getDirList(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("Fetch list of directories");
    DOC_PARAM("thePath", "", DOC_TYPE_STRING);
    DOC_RVAL("List of paths", DOC_TYPE_ARRAY);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        std::string myPath;
        convertFrom(cx, argv[0], myPath);
        try {
            *rval = as_jsval(cx, asl::getDirList(asl::expandEnvironment(myPath)));
        } catch (asl::DirectoryException &) {
            *rval = JSVAL_NULL;
            return JS_TRUE;
        } catch (asl::FileNotFoundException &) {
            *rval = JSVAL_NULL;
            return JS_TRUE;
        }

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
getAppDirectory(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("Returns directory where the running application is located");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        std::string myDirectory = asl::getAppDirectory();
        * rval = as_jsval(cx, myDirectory);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
getAppDataDirectory(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("Returns directory for application data (e.g. $HOME/.application on Unix)");
    DOC_PARAM("theAppName", "Application name", DOC_TYPE_STRING);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        std::string myAppName;
        convertFrom(cx, argv[0], myAppName);
        std::string myDirectory = asl::getAppDataDirectory(myAppName);
        * rval = as_jsval(cx, myDirectory);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
getTempDirectory(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("Returns directory for temp. files (e.g. /tmp on Unix)");
    DOC_END;
    try {
        ensureParamCount(argc, 0);
        std::string myDirectory = asl::getTempDirectory();
        * rval = as_jsval(cx, myDirectory);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
IsDirectory(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Tests if the path points to a directory.");
    DOC_PARAM("thePath", "", DOC_TYPE_STRING);
    DOC_RVAL("true if path is a directory", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        JSString   * str;
        string myPath;

        if (argc != 1) {
            JS_ReportError(cx, "'isDirectory' takes a filepath as argument");
            return JS_FALSE;
        }

        str = JS_ValueToString(cx, argv[0]);
        if (!str) {
            JS_ReportError(cx, "isDirectory() could not convert argument value to string.");
            return JS_FALSE;
        }

        myPath = asl::expandEnvironment(JS_GetStringBytes(str));

        *rval = as_jsval(cx, asl::isDirectory(myPath));

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
MakeDir(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates directory");
    DOC_PARAM("thePath", "", DOC_TYPE_STRING);
    DOC_END;
    try {
        JSString   * str;
        string myPath;

        if (argc != 1) {
            JS_ReportError(cx, "'makeDir takes a filepath as argument");
            return JS_FALSE;
        }

        str = JS_ValueToString(cx, argv[0]);
        if (!str) {
            JS_ReportError(cx, "makeDir() could not convert argument value to string.");
            return JS_FALSE;
        }

        myPath = asl::expandEnvironment(JS_GetStringBytes(str));

        try {
            if (!asl::fileExists(myPath) ) {
                asl::createDirectory(myPath);
                *rval = as_jsval(cx, true);
            }
        } catch (asl::FileNotFoundException) {
            *rval = JSVAL_NULL;
            return JS_TRUE;
        }

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSDirectoryFunctions::Functions() {
    static JSFunctionSpec myFunctions[] = {
        {"getDirList",          getDirList,          1},
        {"isDirectory",         IsDirectory,         1},
        {"getAppDirectory",     getAppDirectory,     0},
        {"getAppDataDirectory", getAppDataDirectory, 1},
        {"getTempDirectory",    getTempDirectory,    0},
        {"makeDir",             MakeDir,             1},
        {0},
    };
    return myFunctions;
}

} // end of namespace jslib

