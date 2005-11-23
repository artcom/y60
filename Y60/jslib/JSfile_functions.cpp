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
#include "JSBlock.h"

#include <asl/Block.h>
#include <asl/file_functions.h>

using namespace std;
using namespace asl;

namespace jslib {

JS_STATIC_DLL_CALLBACK(JSBool)
GetWholeFileAsString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
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
PutWholeStringToFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
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
PutWholeBlockToFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Writes a given binary block into a file");
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
    DOC_PARAM("theBlock", "", DOC_TYPE_BLOCK);
    DOC_RVAL("Returns true if successful.", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        std::string myPath;
        convertFrom(cx, argv[0], myPath);
        asl::Block * myContent = 0;
        convertFrom(cx, argv[1], myContent);
        *rval = as_jsval(cx, asl::putWholeFile(myPath, *myContent));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
GetFilenamePart(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the filename without its directory");
    DOC_PARAM("thePath", "", DOC_TYPE_STRING);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        std::string myPath;
        convertFrom(cx, argv[0], myPath);
        *rval = as_jsval(cx, asl::getFilenamePart((myPath)));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
GetDirectoryPart(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("extracts the directory from a path");
    DOC_PARAM("thePath", "", DOC_TYPE_STRING);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        std::string myPath;
        convertFrom(cx, argv[0], myPath);
        *rval = as_jsval(cx, asl::getDirectoryPart((myPath)));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
FileExists(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Tests whether a given file exists.");
    DOC_PARAM("theFileName", "Path and filename to test", DOC_TYPE_STRING);
    DOC_RVAL("True if file exists", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        JSString   * str;
        const char * myFileName;

        if (argc != 1) {
            JS_ReportError(cx, "'fileExists' takes a filename as argument");
            return JS_FALSE;
        }

        str = JS_ValueToString(cx, argv[0]);
        if (!str) {
            JS_ReportError(cx, "fileExists() could not convert argument value to string.");
            return JS_FALSE;
        }

        myFileName = JS_GetStringBytes(str);

        *rval = BOOLEAN_TO_JSVAL(asl::fileExists(myFileName));

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}



JS_STATIC_DLL_CALLBACK(JSBool)
IncludePath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Adds the given path as search-path to the packet manager.");
    DOC_PARAM("theIncludePath", "A valid path", DOC_TYPE_STRING);
    DOC_END;
    try {
        if (!argc) {
            return JS_TRUE;
        }

        string myIncludePath;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myIncludePath)) {
            JS_ReportError(cx, "includePath(): argument #1 must be a string. (the path)");
            return JS_FALSE;
        }
        JSApp::getPackageManager()->add(myIncludePath);
        PlugInManager::get().setSearchPath(JSApp::getPackageManager()->getSearchPath());
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
RemovePath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Removes the given path from the include path of the package manager.");
    DOC_PARAM("thePath", "The path to remove", DOC_TYPE_STRING);
    DOC_END;
    try {
        if (!argc) {
            return JS_TRUE;
        }

        string myIncludePath;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myIncludePath)) {
            JS_ReportError(cx, "removePath(): argument #1 must be a string. (the path)");
            return JS_FALSE;
        }
        JSApp::getPackageManager()->remove(myIncludePath);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
GetPath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the search-path of the package manager, which is used for includes, plugins, and shaderlibrary.");
    DOC_RVAL("thePath", DOC_TYPE_STRING);
    DOC_END;
    try {
        *rval = as_jsval(cx, JSApp::getPackageManager()->getSearchPath());
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}



JS_STATIC_DLL_CALLBACK(JSBool)
findFiles(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Searches theRelativePath in either thePackageName or all packages \
and returns all files in theRelativePath. If theRelativePath is a file it returns theRelativePath.");
    DOC_PARAM("theRelativePath", "The path to search in. It is relative and must be defined inside a package", DOC_TYPE_STRING);
    DOC_RVAL("All files in theRelativePath as array of strings", DOC_TYPE_ARRAY);
    DOC_RESET;
    DOC_PARAM("theRelativePath", "The path to search in. It is relative and must be defined inside a package", DOC_TYPE_STRING);
    DOC_PARAM("thePackageName", "The package to search in.", DOC_TYPE_STRING);
    DOC_PARAM("theRecurseFlag", "Search recursivly, if true", DOC_TYPE_STRING);
    DOC_RVAL("All files in theRelativePath as array of strings", DOC_TYPE_ARRAY);
    DOC_END;
    try {
        if (argc > 3 ) {
            JS_ReportError(cx, "findFiles(): expects at most three string arguments, (path, package, theRecurseFlag)");
            return JS_FALSE;
        }
        string myRelativePath = "";
        string myPackageName = "";
        if (argc > 0) {
            convertFrom(cx, argv[0], myRelativePath);
        }
        if (argc > 1) {
            convertFrom(cx, argv[1], myPackageName);
        }
        vector<string> myFiles;
        if (argc == 3) {
            bool myRecurseFlag;
            myRecurseFlag = convertFrom(cx, argv[2], myRecurseFlag);
            myFiles = JSApp::getPackageManager()->findFiles(myRelativePath, myPackageName, myRecurseFlag);
        } else {
            myFiles = JSApp::getPackageManager()->findFiles(myRelativePath, myPackageName);
        }

        *rval = as_jsval(cx, myFiles);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
JS_STATIC_DLL_CALLBACK(JSBool)
GetWholeFileAsBlock(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Searches theRelativePath in either thePackageName or all packages and open the file located at theRelativePath if found.");
    DOC_PARAM("theRelativePath", "The path to the file requested. It is relative and must be defined inside a package.", DOC_TYPE_STRING);
    DOC_PARAM("thePackageName", "The name of a package, Package names are stored just as you enter them by calling includePath.", DOC_TYPE_STRING);
    DOC_RVAL("The opened file as a asl::ReadableBlock or null if not found.", DOC_TYPE_STRING);
    DOC_END;
    try {
        if (argc == 0 || argc > 2 ) {
            JS_ReportError(cx, "openFile(): expects one or two string arguments, (theRelativePath, [optional] package)");
            return JS_FALSE;
        }
        string myRelativePath = "";
        convertFrom(cx, argv[0], myRelativePath);

        string myPackageName;
        if (argc > 1) {
            convertFrom(cx, argv[1], myPackageName);
        }
        // since we don't have a wrapped ReadableBlock,
        // we have to make a copy
        asl::Ptr<asl::Block> myBlock(new asl::Block);
        if (argc == 1) {
            *myBlock = *(JSApp::getPackageManager()->openFile(myRelativePath));
        } else {
            *myBlock = *(JSApp::getPackageManager()->openFile(myRelativePath, myPackageName));
        }

        if (myBlock) {
            *rval = as_jsval(cx, myBlock, &(*myBlock) );
        } else {
            *rval = JSVAL_NULL;
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSFileFunctions::Functions() {
    static JSFunctionSpec myFunctions[] = {
        {"getWholeFileAsString",   GetWholeFileAsString,    1},
        {"getWholeFileAsBlock",    GetWholeFileAsBlock,  1},
        {"putWholeStringToFile",   PutWholeStringToFile, 2},
        {"putWholeBlockToFile",    PutWholeStringToFile, 2},
        {"getLastModified",        GetLastModified,   1},
        {"getFilenamePart",        GetFilenamePart,    1},
        {"getDirectoryPart",       GetDirectoryPart,     1},
        {"deleteFile",             DeleteFile,        1},
        {"moveFile",               MoveFile,          2},
        {"fileExists",             FileExists,     1},
        {"includePath",            IncludePath,    1},
        {"removePath",             RemovePath,     1},
        {"getPath",                GetPath,        0},
        {"findFiles",              findFiles,      2},
        {0},
    };
    return myFunctions;
}

} // end of namespace jslib

