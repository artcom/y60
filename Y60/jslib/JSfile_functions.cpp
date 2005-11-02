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

#include "JSfile_functions.h"
#include "Documentation.h"
#include "JSApp.h"

#include <asl/Block.h>
#include <asl/file_functions.h>

using namespace std;
using namespace asl;

namespace jslib {

JS_STATIC_DLL_CALLBACK(JSBool)
GetWholeFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Loads a text file into a string using the package manager");
    DOC_PARAM("theRelativeFilename", "", DOC_TYPE_STRING);
    DOC_PARAM("thePackagePath", "", DOC_TYPE_STRING);
    DOC_END;
    try {
        ensureParamCount(argc, 1, 2);
        string myFilename;
        string myPackage;
        if ( ! convertFrom(cx, argv[0], myFilename)) {
            JS_ReportError(cx, "getWholeFile(): argument #1 must be a string (relative filename)");
            return JS_FALSE;
        }
        Ptr<ReadableBlock> myBlock;
        if (argc == 1) {
            myBlock = JSApp::getPackageManager()->openFile(myFilename);
        } else {
            if ( ! convertFrom(cx, argv[1], myPackage)) {
                JS_ReportError(cx, "getWholeFile(): argument #2 must be a string (package path)");
                return JS_FALSE;
            }
            myBlock = JSApp::getPackageManager()->openFile(myFilename, myPackage);
        }
        if (myBlock) {
            string myContent(myBlock->strbegin(), myBlock->strend());
            *rval = as_jsval(cx, myContent);
        } else {
            *rval = JSVAL_NULL;
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}


static JSBool
GetLastModified(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the modification time for the given file");
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        std::string myPath;
        convertFrom(cx, argv[0], myPath);
        *rval = as_jsval(cx, asl::getLastModified((myPath)));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
MoveFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Moves the given file");
    DOC_PARAM("theSource", "", DOC_TYPE_STRING);
    DOC_PARAM("theTarget", "", DOC_TYPE_STRING);
    DOC_RVAL("Returns true if successful.", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        std::string mySource;
        std::string myTarget;
        convertFrom(cx, argv[0], mySource);
        convertFrom(cx, argv[1], myTarget);
        *rval = as_jsval(cx, asl::moveFile(mySource, myTarget));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
DeleteFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Deletes the given file");
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
    DOC_RVAL("Returns true if successful.", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        std::string myPath;
        convertFrom(cx, argv[0], myPath);
        *rval = as_jsval(cx, asl::deleteFile(myPath));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
PutWholeFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Writes a given string into a file");
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
    DOC_PARAM("theString", "", DOC_TYPE_STRING);
    DOC_RVAL("Returns true if successful.", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        std::string myPath;
        convertFrom(cx, argv[0], myPath);
        std::string myContent;
        convertFrom(cx, argv[1], myContent);
        *rval = as_jsval(cx, asl::putWholeFile(myPath, myContent));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
Basename(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the filename without its directory");
    DOC_PARAM("thePath", "", DOC_TYPE_STRING);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        std::string myPath;
        convertFrom(cx, argv[0], myPath);
        *rval = as_jsval(cx, asl::getBaseName((myPath)));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
Dirname(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("extracts the directory from a path");
    DOC_PARAM("thePath", "", DOC_TYPE_STRING);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        std::string myPath;
        convertFrom(cx, argv[0], myPath);
        *rval = as_jsval(cx, asl::getDirName((myPath)));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSFileFunctions::Functions() {
    static JSFunctionSpec myFunctions[] = {
        {"getWholeFile",       GetWholeFile,      1},
        {"getLastModified",    GetLastModified,   1},
        {"basename",           Basename,          1},
        {"dirname",            Dirname,           1},
        {"deleteFile",         DeleteFile,        1},
        {"moveFile",           MoveFile,          2},
        {"putWholeFile",       PutWholeFile,      2},
        {0},
    };
    return myFunctions;
}

} // end of namespace jslib

