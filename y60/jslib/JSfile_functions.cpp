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

// own header
#include "JSfile_functions.h"

#include <y60/jsbase/Documentation.h>
#include "JSApp.h"
#include <y60/jsbase/JSBlock.h>

#include <asl/base/Block.h>
#include <asl/base/file_functions.h>
#include <asl/base/os_functions.h>

using namespace std;
using namespace asl;

namespace jslib {
static JSBool
getDirectoryEntries(JSContext * cx, JSObject * obj, uintN argc, jsval * argv, jsval * rval) {
    DOC_BEGIN("Collects a list of entries in a given directory");
    DOC_PARAM("thePath", "The directory path to look inside", DOC_TYPE_STRING);
    DOC_RVAL("Array of path-strings", DOC_TYPE_ARRAY);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        std::string myPath;
        convertFrom(cx, argv[0], myPath);
        try {
            *rval = as_jsval(cx, asl::getDirectoryEntries(asl::expandEnvironment(myPath)));
        } catch (asl::OpenDirectoryFailed &) {
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
    DOC_RVAL("The application directory string", DOC_TYPE_STRING);
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
    DOC_PARAM("theAppName", "The application name ot get the data directory for", DOC_TYPE_STRING);
    DOC_RVAL("The application data directory string", DOC_TYPE_STRING);
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
    DOC_RVAL("The temp directory string", DOC_TYPE_STRING);
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
    DOC_PARAM("thePath", "The path string to test", DOC_TYPE_STRING);
    DOC_RVAL("True if path is a directory", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        string myPath;

        if (argc != 1) {
            JS_ReportError(cx, "'isDirectory' takes a filepath as argument");
            return JS_FALSE;
        }

        string myString;
        if (!convertFrom(cx, argv[0], myString)) {
            JS_ReportError(cx, "isDirectory() could not convert argument value to string.");
            return JS_FALSE;
        }

        myPath = asl::expandEnvironment(myString);

        *rval = as_jsval(cx, asl::isDirectory(myPath));

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
createDirectory(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates directory");
    DOC_PARAM("thePath", "The directory to create", DOC_TYPE_STRING);
    DOC_RVAL("Returns true if successful or directory already exists, false if it fails to create it and it does not exist", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        string myPath;

        if (argc != 1) {
            JS_ReportError(cx, "'createDirectory takes a filepath as argument");
            return JS_FALSE;
        }

        string myString;
        if (!convertFrom(cx, argv[0], myString)) {
            JS_ReportError(cx, "createDirectory() could not convert argument value to string.");
            return JS_FALSE;
        }
        myPath = asl::expandEnvironment(myString);

        try {
            if (!asl::fileExists(myPath) ) {
                asl::createDirectory(myPath);
                *rval = as_jsval(cx, true);
            }
        } catch (asl::CreateDirectoryFailed & ex) {
            AC_DEBUG << ex;
            *rval = JSVAL_NULL;
            return JS_TRUE;
        }

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
JS_STATIC_DLL_CALLBACK(JSBool)
removeDirectory(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Removes directory");
    DOC_PARAM("thePath", "The directory to create", DOC_TYPE_STRING);
    DOC_RVAL("Returns true if successful or directory does not exist, false if it fails to remove it", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        string myPath;

        if (argc < 1 || argc > 2) {
            JS_ReportError(cx, "'removeDirectory takes a filepath as argument");
            return JS_FALSE;
        }

        string myString;
        if (!convertFrom(cx, argv[0], myString)) {
            JS_ReportError(cx, "removeDirectory() could not convert argument value to string.");
            return JS_FALSE;
        }
        myPath = asl::expandEnvironment(myString);

        bool myRecursive = false;
        if(argc > 1) {
            if(!convertFrom(cx, argv[1], myRecursive)) {
                JS_ReportError(cx, "removeDirectory() could not convert argument value to boolean.");
                return JS_FALSE;
            }
        }

        try {
            if (asl::fileExists(myPath) ) { // XXX: this line is preposterous. totally misplaced and bug-provoking.
                asl::removeDirectory(myPath, myRecursive);
                *rval = as_jsval(cx, true);
            }
        } catch (asl::RemoveDirectoryFailed & ex) {
            AC_DEBUG << ex;
            *rval = JSVAL_NULL;
            return JS_TRUE;
        }

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
createPath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a directory and all parent directories if necessary");
    DOC_PARAM("thePath", "The path to create", DOC_TYPE_STRING);
    DOC_RVAL("Returns true if successful, false if it fails", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        string myPath;

        if (argc != 1) {
            JS_ReportError(cx, "'createPath needs a filepath as argument");
            return JS_FALSE;
        }

        string myString;
        if (!convertFrom(cx, argv[0], myString)) {
            JS_ReportError(cx, "createDirectory() could not convert argument value to string.");
            return JS_FALSE;
        }

        myPath = asl::expandEnvironment(myString);

        try {
            if (!asl::fileExists(myPath) ) {
                asl::createPath(myPath);
                *rval = as_jsval(cx, true);
            }
        } catch (asl::CreateDirectoryFailed & ex) {
            AC_DEBUG << ex;
            *rval = JSVAL_NULL;
            return JS_TRUE;
        }

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
ReadFileAsString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Loads a text file into a string using the package manager");
    DOC_PARAM("theRelativePath", "The path to search in. It is relative and must be defined inside a package", DOC_TYPE_STRING);
    DOC_RVAL("Returns the file content as string", DOC_TYPE_STRING);
    DOC_RESET;
    DOC_PARAM("theRelativePath", "The path to search in. It is relative and must be defined inside a package", DOC_TYPE_STRING);
    DOC_PARAM("thePackageName", "The package to search in.", DOC_TYPE_STRING);
    DOC_RVAL("Returns the file content as string", DOC_TYPE_STRING);
    DOC_END;
    try {
        ensureParamCount(argc, 1, 2);
        string myFilename;
        string myPackage;
        if ( ! convertFrom(cx, argv[0], myFilename)) {
            JS_ReportError(cx, "readFile(): argument #1 must be a string (relative filename)");
            return JS_FALSE;
        }
        asl::Ptr<ReadableBlockHandle> myBlock;
        if (argc == 1) {
            myBlock = JSApp::getPackageManager()->readFile(myFilename);
        } else {
            if ( ! convertFrom(cx, argv[1], myPackage)) {
                JS_ReportError(cx, "readFile(): argument #2 must be a string (package path)");
                return JS_FALSE;
            }
            myBlock = JSApp::getPackageManager()->readFile(myFilename, myPackage);
        }
        if (myBlock) {
            string myContent(myBlock->getBlock().strbegin(), myBlock->getBlock().strend());
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
    DOC_PARAM("theFilename", "The file path to read modification time for", DOC_TYPE_STRING);
    DOC_RVAL("The timestamp of the last modification", DOC_TYPE_INTEGER);
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
CopyFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Copies the source file to the destination");
    DOC_PARAM("theSource", "The source file path", DOC_TYPE_STRING);
    DOC_PARAM("theTarget", "The destination file path", DOC_TYPE_STRING);
    DOC_RVAL("Returns true if successful.", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        std::string mySource;
        std::string myTarget;
        convertFrom(cx, argv[0], mySource);
        convertFrom(cx, argv[1], myTarget);
        *rval = as_jsval(cx, asl::copyFile(mySource, myTarget));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
MoveFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Moves the source file to the destination");
    DOC_PARAM("theSource", "The source file path", DOC_TYPE_STRING);
    DOC_PARAM("theTarget", "The destination file path", DOC_TYPE_STRING);
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
    DOC_PARAM("theFilename", "The file path to delete", DOC_TYPE_STRING);
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
WriteStringToFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Writes a given string into a file");
    DOC_PARAM("theFilename", "The target file path", DOC_TYPE_STRING);
    DOC_PARAM("theString", "The string to write to the file", DOC_TYPE_STRING);
    DOC_RVAL("Returns true if successful.", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        std::string myPath;
        convertFrom(cx, argv[0], myPath);
        std::string myContent;
        convertFrom(cx, argv[1], myContent);
        *rval = as_jsval(cx, asl::writeFile(myPath, myContent));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
WriteBlockToFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Writes a given binary block into a file");
    DOC_PARAM("theFilename", "The target file path", DOC_TYPE_STRING);
    DOC_PARAM("theBlock", "The block to write", DOC_TYPE_BLOCK);
    DOC_RVAL("Returns true if successful.", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        ensureParamCount(argc, 2);
        std::string myPath;
        convertFrom(cx, argv[0], myPath);
        asl::Block * myContent = 0;
        convertFrom(cx, argv[1], myContent);
        *rval = as_jsval(cx, asl::writeFile(myPath, *myContent));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
GetFilenamePart(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the filename without its directory");
    DOC_PARAM("thePath", "The file path to get the filename from", DOC_TYPE_STRING);
    DOC_RVAL("The filename without directory", DOC_TYPE_STRING);
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
    DOC_BEGIN("Extracts the directory from a path");
    DOC_PARAM("thePath", "The file path to extract the directory from", DOC_TYPE_STRING);
    DOC_RVAL("The directory without filename", DOC_TYPE_STRING);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        std::string myPath;
        convertFrom(cx, argv[0], myPath);
        *rval = as_jsval(cx, asl::getDirectoryPart((myPath)));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
GetExtensionPart(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Return extension of a filename");
    DOC_PARAM("thePath", "The file path get the extension from", DOC_TYPE_STRING);
    DOC_RVAL("The extension of the filename", DOC_TYPE_STRING);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        std::string myPath;
        convertFrom(cx, argv[0], myPath);
        *rval = as_jsval(cx, asl::getExtension((myPath)));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
FileExists(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Tests whether a given file exists.");
    DOC_PARAM("theFileName", "The filename to test", DOC_TYPE_STRING);
    DOC_RVAL("True if file exists", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        if (argc != 1) {
            JS_ReportError(cx, "'fileExists' takes a filename as argument");
            return JS_FALSE;
        }

        std::string myPath;
        if (!convertFrom(cx, argv[0], myPath)) {
            JS_ReportError(cx, "fileExists() could not convert argument value to string.");
            return JS_FALSE;
        }

        *rval = BOOLEAN_TO_JSVAL(asl::fileExists(myPath));

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}



JS_STATIC_DLL_CALLBACK(JSBool)
IncludePath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Adds the given path as search-path to the packet manager.");
    DOC_PARAM("theIncludePath", "The path to add to the packet manager", DOC_TYPE_STRING);
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
    DOC_RVAL("The searchpath of the package manager", DOC_TYPE_STRING);
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
ReadFileAsBlock(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Searches theRelativePath in either thePackageName or all packages and open the file located at theRelativePath if found.");
    DOC_PARAM("theRelativePath", "The path to the file requested. It is relative and must be defined inside a package.", DOC_TYPE_STRING);
    DOC_PARAM("thePackageName", "The name of a package, Package names are stored just as you enter them by calling includePath.", DOC_TYPE_STRING);
    DOC_RVAL("The opened file as a asl::ReadableBlock or null if not found.", DOC_TYPE_STRING);
    DOC_END;
    try {
        if (argc == 0 || argc > 2 ) {
            JS_ReportError(cx, "ReadFileAsBlock(): expects one or two string arguments, (theRelativePath, [optional] package)");
            return JS_FALSE;
        }
        string myRelativePath = "";
        convertFrom(cx, argv[0], myRelativePath);

        string myPackageName;
        if (argc > 1) {
            convertFrom(cx, argv[1], myPackageName);
        }
        asl::Ptr<asl::ReadableBlockHandle> myReadableBlock;
        if (argc == 1) {
            myReadableBlock = JSApp::getPackageManager()->readFile(myRelativePath);
        } else {
            myReadableBlock = JSApp::getPackageManager()->readFile(myRelativePath, myPackageName);
        }

        if (myReadableBlock) {
            // since we don't have a wrapped ReadableBlock,
            // we have to make a copy
            asl::Ptr<asl::Block> myBlock(new asl::Block);
            *myBlock = myReadableBlock->getBlock();
            *rval = as_jsval(cx, myBlock, myBlock.get() );
        } else {
            *rval = JSVAL_NULL;
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
searchFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Searches thePath for theFile; if no path is given, the current path returned by getPath() is used.");
    DOC_PARAM("theFile", "The file to search for.", DOC_TYPE_STRING);
    DOC_PARAM_OPT("thePath", "The path to search in.", DOC_TYPE_STRING,"getPath()");
    DOC_RVAL("the absolute path as string, empty string if not found", DOC_TYPE_STRING);
    DOC_END;
    try {
        if (argc > 2 ) {
            JS_ReportError(cx, "searchFile(): expects at most two string arguments, (theFile, thePath)");
            return JS_FALSE;
        }
        if (argc < 1 ) {
            JS_ReportError(cx, "searchFile(): expects at least one string (theFile)");
            return JS_FALSE;
        }
        string myPath = JSApp::getPackageManager()->getSearchPath();
        string myFile = "";
        if (argc > 0) {
            convertFrom(cx, argv[0], myFile);
        }
        if (argc > 1) {
            convertFrom(cx, argv[1], myPath);
        }
        std::string myFilePath = asl::searchFile(myFile, myPath);
        if (myFilePath.size()) {
            *rval = as_jsval(cx, myFilePath);
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSFileFunctions::Functions() {
    static JSFunctionSpec myFunctions[] = {
        {"readFileAsString",  ReadFileAsString,  2},
        {"readFileAsBlock",   ReadFileAsBlock,   2},
        {"writeStringToFile", WriteStringToFile, 2},
        {"writeBlockToFile",  WriteBlockToFile, 2},
        {"getLastModified",        GetLastModified,  1},
        {"getFilenamePart",        GetFilenamePart,  1},
        {"getDirectoryPart",       GetDirectoryPart, 1},
        {"getExtensionPart",       GetExtensionPart, 1},
        {"deleteFile",             DeleteFile,  1},
        {"copyFile",               CopyFile,    2},
        {"moveFile",               MoveFile,    2},
        {"fileExists",             FileExists,  1},
        {"includePath",            IncludePath, 1},
        {"removePath",             RemovePath,  1},
        {"getPath",                GetPath,     0},
        {"findFiles",              findFiles,   3},
        {"getDirectoryEntries", getDirectoryEntries, 1},
        {"isDirectory",         IsDirectory,         1},
        {"getAppDirectory",     getAppDirectory,     0},
        {"getAppDataDirectory", getAppDataDirectory, 1},
        {"getTempDirectory",    getTempDirectory,    0},
        {"createDirectory",     createDirectory,     1},
        {"removeDirectory",     removeDirectory,     1},
        {"createPath",          createPath,          1},
        {"searchFile",          searchFile,          2},
        {0},
    };
    return myFunctions;
}

} // end of namespace jslib
