//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSApp.h"
#include "IRendererExtension.h"

#include "jscpp.h"
#include <y60/JScppUtils.h>
#include <y60/JScppUtils.impl>
#include <y60/JSNode.h>
#include <y60/JSBlock.h>
#include <y60/JSVector.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/IScriptablePlugin.h>
#include <y60/IFactoryPlugin.h>
#include <y60/QuitFlagSingleton.h>
#include <y60/Documentation.h>

#include <asl/string_functions.h>
#include <asl/file_functions.h>
#include <asl/DirectoryPackage.h>
#include <asl/os_functions.h>
#include <asl/Time.h>
#include <asl/MappedBlock.h>
#include <asl/Arguments.h>
#include <asl/PlugInManager.h>
#include <asl/Logger.h>
#include <asl/Revision.h>
#include <asl/numeric_functions.h>

#include <js/jsapi.h>
#include <js/jsprf.h>
#include <js/jsparse.h>
#include <js/jsscan.h>
#include <js/jsemit.h>
#include <js/jsscript.h>
#include <js/jsarena.h>
#include <js/jscntxt.h>
#include <js/jsdbgapi.h>

#include <y60/IPaintLibExtension.h>
#include <paintlib/planydec.h>

#include <y60/IDecoder.h>
#include <y60/DecoderManager.h>
#include <y60/IEventSource.h>
#include <y60/EventDispatcher.h>
#include <y60/Image.h>
#include <y60/CommonTags.h>
#include <y60/Request.h>

#include <errno.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <list>
#include <map>

#define DB(x) // x

using namespace std;
using namespace asl;
using namespace y60;

namespace jslib {

JSContext * ourJSContext(0);
int ourExitCode = 0;

FILE * gOutFile = NULL;
FILE * gErrFile = NULL;
JSBool reportWarnings = JS_TRUE;
std::string ourTopScriptFilename;

std::string JSApp::_myProgramName;

JSApp::JSApp() : ourStrictFlag(false), ourJSVersion(0)
{}

// ****************  runtime-options **********************
typedef std::map<std::string, asl::MappedBlock*> MappedBlockMap;
MappedBlockMap ourHeartbeatBlocks;

typedef std::list<asl::PlugInBasePtr> PlugInList;
static PlugInList ourLoadedPlugIns;

typedef std::vector<std::string> IncludeGuardVector;
IncludeGuardVector ourIncludeGuard;

asl::PackageManagerPtr
JSApp::getPackageManager() {
    return AppPackageManager::get().getPtr();
}

void
JSApp::setReportWarnings(bool theEnableFlag) {
    reportWarnings = theEnableFlag ? JS_TRUE : JS_FALSE;
}

void
JSApp::setStrictFlag(bool theStrictFlag) {
    ourStrictFlag = theStrictFlag;
}

void
JSApp::setJSVersion(int theVersion) {
    ourJSVersion = theVersion;
}

bool
JSApp::getQuitFlag() {
    return QuitFlagSingleton::get().getQuitFlag();
}

void
JSApp::setQuitFlag(bool theFlag) {
    QuitFlagSingleton::get().setQuitFlag(theFlag);
}


// ****************  end runtime-options **********************

void
throwException(JSContext * cx, const char * theMessage, std::string theFilename, unsigned theLine) {
    //cerr << "Throw Exception: " << theMessage << ", file: " << theFilename << ", line: " << theLine << endl;
    JSString* jsstr;

    // if we get errors during error reporting we report those
    if (((jsstr=JS_NewStringCopyZ(cx, theMessage))) && (JS_AddNamedRoot(cx, &jsstr, "jsstr"))) {
        jsval dummy;

        // We can't use JS_EvaluateScript since the stack would be wrong
        JSFunction * func;
        JSObject   * fobj;
        const char * fbody = "throw new Error(msg, file, line);";
        const char * argnames[] = {"msg", "file", "line"};
        if ((func = JS_CompileFunction(cx, NULL, NULL, 3, argnames, fbody, strlen(fbody), NULL, 0))) {
            // root function
            if (((fobj = JS_GetFunctionObject(func))) && (JS_AddNamedRoot(cx, &fobj, "fobj"))) {
                jsval args[] = {STRING_TO_JSVAL(jsstr), as_jsval(cx, theFilename), as_jsval(cx, theLine)};
                JS_CallFunction(cx, NULL, func, 3, args, &dummy);
                JS_RemoveRoot(cx, &fobj);
            }
        }
        JS_RemoveRoot(cx,&jsstr);
    }
}

std::string ourLastFileName   = "";
unsigned    ourLastLineNumber = 0;

void
JSApp::ShellErrorReporter(JSContext *cx, const char * message, JSErrorReport * report) {
    if (!report) {
        AC_PRINT << message << endl;
        return;
    }

    /* Conditionally ignore reported warnings. */
    asl::Severity mySeverity = SEV_ERROR;
    char * myStrictMsg = "";
    if (JSREPORT_IS_WARNING(report->flags)) {
        mySeverity = SEV_WARNING;
        if (JSREPORT_IS_STRICT(report->flags)) {
            myStrictMsg = "[strict]";
        }
    }
    const char * myFileName = "JSApp.cpp";
    if (report->filename) {
        myFileName = report->filename;
    }

/*
    if (report->linebuf) {
        std::string myMarker;
        int i, j, k, n;
        n = report->tokenptr - report->linebuf;
        for (i = j = 0; i < n; i++) {
            if (report->linebuf[i] == '\t') {
                for (k = (j + 8) & ~7; j < k; j++) {
                    myMarker += '.';
                }
                continue;
            }
            myMarker += '.';
            j++;
        }
        myMarker += '^';
        AC_LOG(mySeverity, myFileName, report->lineno) <<  myStrictMsg << message << endl;
    }
*/

    if (!JSREPORT_IS_WARNING(report->flags)) {
        ourExitCode = EXITCODE_RUNTIME_ERROR;

        // Only log uncaught exceptions, because the others are caught in javascript
        if (report->errorNumber == 147 /* "uncaugth exception" */) {
            AC_LOG(mySeverity, ourLastFileName.c_str(), ourLastLineNumber) <<  myStrictMsg << message << endl;
        } else {
            AC_LOG(mySeverity, myFileName, report->lineno) <<  myStrictMsg << message << endl;
            throwException(cx, message, myFileName, report->lineno);
            // To be able to report uncaught exceptions correctly
            // Could be solved better, if the rethrowing mechanism would file the report structure correctly
            ourLastFileName   = myFileName;
            ourLastLineNumber = report->lineno;
        }
    } else {
        AC_LOG(mySeverity, myFileName, report->lineno) <<  myStrictMsg << message << endl;
    }
}

JS_STATIC_DLL_CALLBACK(JSBool)
Line(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the linenumber in the script currently being executed.");
    DOC_RVAL("Current linenumber", DOC_TYPE_INTEGER);
    DOC_END;
    try {
        const char * myFile;
        int myLine;

        if (getFileLine(cx,obj,argc,argv,myFile,myLine)) {
            *rval = INT_TO_JSVAL(myLine);
            return JS_TRUE;
        }

        return JS_FALSE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
File(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns filename of the current script.");
    DOC_RVAL("Current filename", DOC_TYPE_STRING);
    DOC_END;
    try {
        const char * myFile;
        int myLine;
        if (getFileLine(cx,obj,argc,argv,myFile,myLine)) {
            *rval = as_jsval(cx, string(myFile));
            return JS_TRUE;
        }
        return JS_FALSE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
FileLine(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns line and filename of the current scriptfile as properties of an object");
    DOC_RVAL("", DOC_TYPE_STRING);
    DOC_END;
    try {
        const char * myFile;
        int myLine;
        if (getFileLine(cx,obj,argc,argv,myFile,myLine)) {
            JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
            if (!JS_DefineProperty(cx, myReturnObject, "file",  as_jsval(cx, asl::getFilenamePart(myFile)), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
            if (!JS_DefineProperty(cx, myReturnObject, "line", as_jsval(cx, myLine), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
            *rval = OBJECT_TO_JSVAL(myReturnObject);
            return JS_TRUE;
        }
        return JS_FALSE;
    } HANDLE_CPP_EXCEPTION;
}


JS_STATIC_DLL_CALLBACK(JSBool)
DumpStack(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Dumps the stack to stderr.");
    DOC_END;
    try {
        JSStackFrame* fp;
        JSStackFrame* iter = 0;
        int num = 0;

        fprintf(gOutFile, "Dumping Stackframes:\n");

        while(0 != (fp = JS_FrameIterator(cx, &iter))) {
            fprintf(gOutFile, "Stackframe %d:", num);
            if(!JS_IsNativeFrame(cx, fp)) {
                JSScript* script = JS_GetFrameScript(cx, fp);
                jsbytecode* pc = JS_GetFramePC(cx, fp);
                if(script && pc) {
                    const char * filename = JS_GetScriptFilename(cx, script);
                    int lineno =  JS_PCToLineNumber(cx, script, pc);

                    const char * funname = 0;
                    JSFunction * fun = JS_GetFrameFunction(cx, fp);
                    if(fun) {
                        funname = JS_GetFunctionName(fun);
                    }
                    fprintf(gOutFile, "  lineno:%5d filename: '%s' function: %s\n", lineno, filename ? filename : "-", funname ? funname : "-");
                }
            } else {
                fprintf(gOutFile, "native\n");
            }
            ++num;
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
Print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Tries string conversion on the given data and prints the result to stderr.");
    DOC_PARAM("theMessage", "Message to print to console", DOC_TYPE_STRING);
    DOC_PARAM("...",  "Any number of parameters can be provided.", DOC_TYPE_STRING);
    DOC_END;
    try {
        uintN i, n;

        for (i = n = 0; i < argc; i++) {
            std::string myString;
            if (! convertFrom(cx, argv[i], myString) ) {
                return JS_FALSE;
            }
            cout << (i ? " " : "") << myString;
        }
        n++;
        if (n) {
            cout << endl;
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
ReadStdIn(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Reads n bytes from stdin or until an EOF.");
    DOC_PARAM("theByteCount", "Number of bytes to be read", DOC_TYPE_STRING);
    DOC_RVAL("input string", DOC_TYPE_STRING);
    DOC_END;
    try {
        unsigned myByteCount = 0;
        if (argc > 0) {
            if (!convertFrom(cx, argv[0], myByteCount)) {
                JS_ReportError(cx, "JSApp::ReadStdIn(): argument #1 must be an integer (byte count)");
                return JS_FALSE;
            }
        }

        const unsigned BUFSIZE = 1024;
        char myBuffer[BUFSIZE];
        string myInputStr;
        unsigned n = 0;
        do {
            cin.read(myBuffer, asl::minimum(BUFSIZE, myByteCount - n));
            unsigned myBytesRead = cin.gcount();
            myInputStr.append(myBuffer, myBytesRead);
            n += myBytesRead;
        } while (cin.good() && n < myByteCount);

        if (cin.bad() || (!cin.eof() && n < myByteCount)) {
            JS_ReportError(cx, "JSApp::ReadStdIn(): error while reading from stdin");
            return JS_FALSE;
        }

        *rval = as_jsval(cx, myInputStr);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}



JS_STATIC_DLL_CALLBACK(JSBool)
ParseArguments(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns an arguments object (array-of-string) of valid arguments after \
being handed input arguments and comparing them with a list of allowed arguments.");
    DOC_PARAM("theArguments", "Array of argument-strings to parse", DOC_TYPE_ARRAY);
    DOC_PARAM("theAllowedOptions", "(array-of-(array-of-string))", DOC_TYPE_ARRAY);
    DOC_RVAL("(array-of-string) arguments object", DOC_TYPE_ARRAY);
    DOC_END;
    try {
        if (argc != 2 ) {
            JS_ReportError(cx, "ParseArguments(): expects two arguments, (array-of-string, array-of-(array-of-string))");
            return JS_FALSE;
        }
        asl::Arguments myArguments;

        vector<string> myArgs;
        convertFrom(cx, argv[0], myArgs);

        myArgs.insert(myArgs.begin(), ourTopScriptFilename);

        JSObject * myValidOptions = JSVAL_TO_OBJECT(argv[1]);
        JSIdArray * myPropIds = JS_Enumerate(cx, myValidOptions);

        for (int i = 0; i < myPropIds->length; ++i) {
            jsval myIdVal;
            if (JS_IdToValue(cx,myPropIds->vector[i],&myIdVal)) {
                string myPropName;
                convertFrom(cx, myIdVal, myPropName);
                jsval myValue;
                JS_GetProperty(cx, myValidOptions, myPropName.c_str(), &myValue);
                string myValueString;
                if (myValue != JSVAL_NULL) {
                    convertFrom(cx, myValue, myValueString);
                }
                string myOptionName = "--" + myPropName;
                Arguments::AllowedOption myAllowedOption[] = {{ myOptionName.c_str(), myValueString.c_str() },
                    { "", ""}};
                myArguments.addAllowedOptions(myAllowedOption);
                DB(AC_TRACE << "PropName[" << i << "] = '" << myPropName << "' -> '" << myValueString << "'" << endl);
            }
        }
        myArguments.parse(myArgs);

        // create a JS "arguments" object
        vector<string> justArguments;
        for (int i = 0; i < myArguments.getCount(); ++i) {
            justArguments.push_back(myArguments.getArgument(i));
        }
        *rval = as_jsval(cx, justArguments);
        JSObject * myArgumentsObject = JSVAL_TO_OBJECT(*rval);
        // add a "options" property to "arguments"
        JSObject * myOptionsObject = JS_NewObject(cx, NULL, NULL, NULL);
        jsval myOptionsValue = OBJECT_TO_JSVAL(myOptionsObject);
        if (! JS_SetProperty(cx, myArgumentsObject, "options", &myOptionsValue)) {
            AC_ERROR << "can't add property 'options' to argument object" << endl;
            return JS_FALSE;
        };
        // add the command-line options as properties to the "options" object
        vector<string> myOptionNames = myArguments.getOptionNames();
        for (int i = 0; i < myOptionNames.size(); ++i) {
            const string & myOptionArgument = myArguments.getOptionArgument(myOptionNames[i]);
            jsval myOptionArgumentValue;
            if (myOptionArgument.empty()) {
                myOptionArgumentValue = JSVAL_NULL;
            } else {
                myOptionArgumentValue = as_jsval(cx, myOptionArgument);
            }
            string::size_type myFirstNotHyphen = myOptionNames[i].find_first_not_of(Arguments::OPTION_START_CHAR);
            if (myFirstNotHyphen != string::npos) {
                myOptionNames[i] = myOptionNames[i].substr(myFirstNotHyphen);
            }
            JS_SetProperty(cx, myOptionsObject, myOptionNames[i].c_str(), &myOptionArgumentValue);
        }
        // set programname
        jsval myProgramNameValue = as_jsval(cx, myArguments.getProgramName());
        JS_SetProperty(cx, myArgumentsObject, "programname", &myProgramNameValue);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}


JS_STATIC_DLL_CALLBACK(JSBool)
Plug(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Plugs the component with the given name.");
    DOC_PARAM("thePluginName", "Specifies the name of the plugin. (e.g. 'y60FFMpegDecoder')", DOC_TYPE_STRING);
    DOC_RESET;
    DOC_PARAM("thePluginName", "Specifies the name of the plugin. (e.g. 'y60FFMpegDecoder')", DOC_TYPE_STRING);
    DOC_PARAM("theNode", "Optionally a node can be given that will be passed on to the plugged component", DOC_TYPE_NODE);
    DOC_END;
    try {
        if (argc < 1) {
            JS_ReportError(cx, "plug(): expects at least one argument");
            return JS_FALSE;
        }

        string myPluginName;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myPluginName)) {
            JS_ReportError(cx, "plug(): argument #1 must be a string. (thePluginName)");
            return JS_FALSE;
        }
        PlugInBasePtr myPlugIn = PlugInManager::get().getPlugIn(myPluginName);

        if (IScriptablePluginPtr myScriptablePlugin = dynamic_cast_Ptr<IScriptablePlugin>(myPlugIn)) {
            AC_INFO << myPluginName << ": as ScriptablePlugin" << endl;
            myScriptablePlugin->initClasses(cx, obj);
            if (argc > 1) {
                dom::NodePtr myNode;
                if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myNode)) {
                    JS_ReportError(cx, "plug(): argument #2 must be a Node");
                    return JS_FALSE;
                }
                myScriptablePlugin->onUpdateSettings(myNode);
            }

            //*val = OBJ_TO_JSVAL(JS_NewObject(cx, &x.getclass(), NULL, NULL));

        }

        if (IFactoryPluginPtr myFactoryPlugin = dynamic_cast_Ptr<IFactoryPlugin>(myPlugIn)) {
            AC_INFO << myPluginName << ": as FactoryPlugin" << endl;
            //*rval = OBJ_TO_JSVAL(myFactoryPlugin->initClasses(cx,obj));
            myFactoryPlugin->initClasses(cx,obj);
        }

        if (y60::IPaintLibExtensionPtr myPaintLibExtension = dynamic_cast_Ptr<y60::IPaintLibExtension>(myPlugIn)) {
            AC_INFO << myPluginName << ": as PaintlibExtension" << endl;
            PLAnyPicDecoder::RegisterDecoder(myPaintLibExtension->getDecoderFactory());
        }

        if (IEventSourcePtr myEventSource = dynamic_cast_Ptr<IEventSource>(myPlugIn)) {
            AC_INFO << myPluginName << ": as EventSource" << endl;
            y60::EventDispatcher::get().addSource(&*myEventSource);
        }
        if (IDecoderPtr myDecoder = dynamic_cast_Ptr<IDecoder>(myPlugIn)) {
            AC_INFO << myPluginName << ": as Decoder" << endl;
            DecoderManager::get().addDecoder(myDecoder);
        }
        ourLoadedPlugIns.push_back(myPlugIn);

        // if this is a renderer extension, then return the instance ...
        if (IRendererExtensionPtr myRendererExtension = dynamic_cast_Ptr<IRendererExtension>(myPlugIn)) {
            IScriptablePluginPtr myScriptablePlugin = dynamic_cast_Ptr<IScriptablePlugin>( myPlugIn );
            if (myScriptablePlugin) {
                * rval = as_jsval(cx, myScriptablePlugin);
            }
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}


JS_STATIC_DLL_CALLBACK(JSBool)
SaveImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Exports a specified image node into a file with the given filename.");
    DOC_PARAM("theImageNode", "A node in the scene that should be saved as file", DOC_TYPE_NODE);
    DOC_PARAM("theFilename", "Filename and path where to save the image. The image format is automatically determined by the file-extension.", DOC_TYPE_STRING);
    DOC_END;
    try {
        if (argc != 2) {
			JS_ReportError(cx, "saveImage(): expects at least two arguments : image node, file name.");
            return JS_FALSE;
        }

		dom::NodePtr myImageNode;
		if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myImageNode)) {
            JS_ReportError(cx, "saveImage(): argument #1 must be an image node");
            return JS_FALSE;
        }

        string myFileName;
        if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myFileName)) {
            JS_ReportError(cx, "saveImage(): argument #2 must be a string. (theFilename)");
            return JS_FALSE;
        }

		ImagePtr myImage = myImageNode->getFacade<y60::Image>();
		myImage->saveToFile(myFileName);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
JS_STATIC_DLL_CALLBACK(JSBool)
BlitImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Copy pixels from one image to another");
    DOC_PARAM("theSourceNode", "A X60 source imagenode", DOC_TYPE_NODE);
    DOC_PARAM("theTargetNode", "A X60 target imagenode", DOC_TYPE_NODE);
    DOC_PARAM("theTargetPos", "The pixelposition in target image", DOC_TYPE_VECTOR2I);
    DOC_PARAM("theSourceMin", "The min position of the source image area to copy (default: 0,0). Must be combined with theSourceSize", DOC_TYPE_VECTOR2I);
    DOC_PARAM("theSourceMax", "The max position of the source area to copy (default: width,height). Must be combined with theSourceSize", DOC_TYPE_VECTOR2I);
    DOC_END;
    try {
        if (argc != 3 && argc != 5) {
			JS_ReportError(cx, "blitImage(): expects at least three arguments : source image node,"
                               "target image node, target position [,source position, source-size]");
            return JS_FALSE;
        }
		dom::NodePtr mySourceImageNode;
		if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], mySourceImageNode)) {
            JS_ReportError(cx, "blitImage(): argument #1 must be an image node");
            return JS_FALSE;
        }
		dom::NodePtr myTargetImageNode;
		if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myTargetImageNode)) {
            JS_ReportError(cx, "blitImage(): argument #2 must be an image node");
            return JS_FALSE;
        }
        asl::Vector2i myTargetPosition;
		if (JSVAL_IS_VOID(argv[2]) || !convertFrom(cx, argv[2], myTargetPosition)) {
            JS_ReportError(cx, "blitImage(): argument #3 must be a Vector2i");
            return JS_FALSE;
        }
		ImagePtr myTargetImage = myTargetImageNode->getFacade<y60::Image>();
		ImagePtr mySourceImage = mySourceImageNode->getFacade<y60::Image>();
        if (argc == 5) {
            asl::Vector2i myMinPosition;
            if (JSVAL_IS_VOID(argv[3]) || !convertFrom(cx, argv[3], myMinPosition)) {
                JS_ReportError(cx, "blitImage(): argument #4 must be a Vector2i");
                return JS_FALSE;
            }
            asl::Vector2i myMaxPosition;
            if (JSVAL_IS_VOID(argv[4]) || !convertFrom(cx, argv[4], myMaxPosition)) {
                JS_ReportError(cx, "blitImage(): argument #5 must be a Vector2i");
                return JS_FALSE;
            }
            asl::Box2i mySourceArea(myMinPosition, myMaxPosition);
		    myTargetImage->blitImage(mySourceImage, myTargetPosition, & mySourceArea);
        } else {
    		myTargetImage->blitImage(mySourceImage, myTargetPosition);
        }

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
ApplyImageFilter(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Apply a filter to an image");
    DOC_PARAM("theImageNode", "A image node in the X60-DOM", DOC_TYPE_NODE);
    DOC_PARAM("theFilterName", "Name of the filter to apply to the image", DOC_TYPE_STRING);
    DOC_PARAM("theFilterParams", "Filter parameter", DOC_TYPE_VECTOROFFLOAT);
    DOC_END;
    try {
        if (argc != 3) {
			JS_ReportError(cx, "applyImageFilter(): expects at least three arguments : image node,"
                               "filter name, filter params");
            return JS_FALSE;
        }
		dom::NodePtr myImageNode;
		if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myImageNode)) {
            JS_ReportError(cx, "applyImageFilter(): argument #1 must be an image node");
            return JS_FALSE;
        }

        std::string myFilterName;
        if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myFilterName)) {
            JS_ReportError(cx, "applyImageFilter(): argument #2 must be a vector of strings. (theFilterName)");
            return JS_FALSE;
        }

        VectorOfFloat myFilterParams;
        if (JSVAL_IS_VOID(argv[2]) || !convertFrom(cx, argv[2], myFilterParams)) {
            JS_ReportError(cx, "applyImageFilter(): argument #3 must be a vector of VectorOfFloat. (theFilterParams)");
            return JS_FALSE;
        }

		ImagePtr myImage = myImageNode->getFacade<y60::Image>();
		myImage->applyFilter(myFilterName, myFilterParams);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
SaveImageFiltered(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Apply a filter to an image and save it to disk");
    DOC_PARAM("theImageNode", "A image node in the X60-DOM to save", DOC_TYPE_NODE);
    DOC_PARAM("theFileName", "The filename to save to", DOC_TYPE_STRING);
    DOC_PARAM("theFilterName", "List of the filters to apply to the image", DOC_TYPE_STRING);
    DOC_PARAM("theFilterParams", "List of filter parameters", DOC_TYPE_VECTOROFFLOAT);
    DOC_END;
    try {
        if (argc != 4) {
			JS_ReportError(cx, "saveImageFiltered(): expects at least four arguments : image node,"
                               "file name, filter name, filter params");
            return JS_FALSE;
        }

		dom::NodePtr myImageNode;
		if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myImageNode)) {
            JS_ReportError(cx, "saveImageFiltered(): argument #1 must be an image node");
            return JS_FALSE;
        }

        string myFileName;
        if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myFileName)) {
            JS_ReportError(cx, "saveImageFiltered(): argument #2 must be a string. (theFilename)");
            return JS_FALSE;
        }

        VectorOfString myFilterName;
        if (JSVAL_IS_VOID(argv[2]) || !convertFrom(cx, argv[2], myFilterName)) {
            JS_ReportError(cx, "saveImageFiltered(): argument #3 must be a vector of strings. (theFilterName)");
            return JS_FALSE;
        }

        VectorOfVectorOfFloat myFilterParams;
        if (JSVAL_IS_VOID(argv[3]) || !convertFrom(cx, argv[3], myFilterParams)) {
            JS_ReportError(cx, "saveImageFiltered(): argument #4 must be a vector of VectorOfFloat. (theFilterParams)");
            return JS_FALSE;
        }

		ImagePtr myImage = myImageNode->getFacade<y60::Image>();
		myImage->saveToFileFiltered(myFileName, myFilterName, myFilterParams);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
Reuse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Recomiles all included (used) files.");
    DOC_END;
    try {
        for (unsigned i = 0; i < ourIncludeGuard.size(); ++i) {
            JSScript * script = JS_CompileFile(cx, obj, ourIncludeGuard[i].c_str());
            if (!script) {
                return JS_FALSE;
            } else {
                jsval result;
                JSBool ok = JS_ExecuteScript(cx, obj, script, &result);
                JS_DestroyScript(cx, script);
                if (!ok) {
                    return JS_FALSE;
                }
            }
        }

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
Use(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Include directive for JavaScript. Includes a specified .js file to the current script. "\
              "Searches in the given include paths, including the current scripts path.");
    DOC_PARAM("theFilename", "Path to a file to use. Relative to the search path set with the '-I' option", DOC_TYPE_STRING);
    DOC_PARAM("...", "You can provide any number of files to use", DOC_TYPE_STRING);
    DOC_END;
    try {
        uintN i;
        string myIncludeFile;
        JSScript *script;
        JSBool ok;
        jsval result;
        const char * myCurrentFile;
        int myLine;

        for (i = 0; i < argc; i++) {

            if (!convertFrom(cx, argv[0], myIncludeFile)) {
                JS_ReportError(cx, "Use could not convert argument value to string.");
                return JS_FALSE;
            }
            AC_DEBUG << "use: myIncludeFile=" << myIncludeFile;

            // Compute file path relative to file in which the use() statement was called
            if (!getFileLine(cx, obj, argc, argv, myCurrentFile, myLine)) {
                return JS_FALSE;
            }

            std::string myIncludePath = asl::getDirectoryPart(myCurrentFile);
            AC_DEBUG << "use: myIncludePath=" << myIncludePath;
            std::string myIncludeFileWithPath = asl::searchFile(myIncludeFile, myIncludePath);

            if (myIncludeFileWithPath.empty()) {
                // Try looking in the package manager:
                myIncludeFileWithPath = JSApp::getPackageManager()->searchFile(myIncludeFile);
            }
            AC_DEBUG << "use myIncludeFileWithPath=" << myIncludeFileWithPath;
            if (myIncludeFileWithPath.empty()) {
                AC_ERROR << "File '" << myIncludeFile << "' not found in " << myIncludePath << ";" << JSApp::getPackageManager()->getSearchPath() << std::endl;
                return JS_FALSE;
            } else {
                // Only include files once
                IncludeGuardVector::const_iterator it = find(ourIncludeGuard.begin(), ourIncludeGuard.end(), myIncludeFileWithPath);
                if (it == ourIncludeGuard.end()) {
                    script = JS_CompileFile(cx, obj, myIncludeFileWithPath.c_str());
                    ourIncludeGuard.push_back(myIncludeFileWithPath);
                    AC_DEBUG << "use: compiled: " << myIncludeFileWithPath;
                } else {
                    AC_DEBUG << "use: has been already included: " << myIncludeFileWithPath;
                    return JS_TRUE;
                }
            }

            if (!script) {
                AC_WARNING << "use: compilation of " << myIncludeFileWithPath << " failed.";
                ok = JS_FALSE;
            } else {
                ok = JS_ExecuteScript(cx, obj, script, &result);
                AC_DEBUG << "use: executed: " << myIncludeFileWithPath <<", status = "<<ok;
                JS_DestroyScript(cx, script);
            }

            if (!ok) {
                return JS_FALSE;
            }
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
Version(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Get the JavaScript version.");
    DOC_RVAL("The current JavaScript version", DOC_TYPE_INTEGER);
    DOC_END;
    try {
        *rval = INT_TO_JSVAL(JS_GetVersion(cx));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
BuildDate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Prints the build date and time of acxpshell/acgtkshell.");
    DOC_END;
    try {
        AC_PRINT << "built on " << __DATE__<< " at " << __TIME__;
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
Revision(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Get the SVN revision number of pro60 at time of build.");
    DOC_RVAL("The SVN revision number", DOC_TYPE_STRING);
    DOC_END;
    try {
        *rval = as_jsval(cx, asl::ourRevision);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
Exit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Stops the interpreter and exits with a given exit code.");
    DOC_PARAM_OPT("theExitCode", "Specifies the exit code (default is 0).", DOC_TYPE_INTEGER, 0);
    DOC_END;
    try {
        if (argc > 0) {
            JS_ConvertArguments(cx, argc, argv,"/ i", &ourExitCode);
        } else {
            ourExitCode = 0;
        }
        JSApp::setQuitFlag(JS_TRUE);
        // Returning JS_FALSE makes the Interpreter stop!
        return JS_FALSE;
    } HANDLE_CPP_EXCEPTION;
}
#ifdef GC_MARK_DEBUG
extern "C" JS_FRIEND_DATA(FILE *) js_DumpGCHeap;
#endif

JS_STATIC_DLL_CALLBACK(JSBool)
GC(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    DOC_BEGIN("Enforces garbage collection.");
    DOC_PARAM_OPT("theFileName", "If GC_MARK_DEBUG is defined a filename can be given to dump the GC heap for debugging reasons.", DOC_TYPE_STRING, "null");
    DOC_END;
    try {
        JSRuntime *rt;
        uint32 preBytes;

        rt = cx->runtime;
        preBytes = rt->gcBytes;
#ifdef GC_MARK_DEBUG
        if (argc && JSVAL_IS_STRING(argv[0])) {
            string myName;
            convertFrom(cx, argv[0], myName);
            FILE *file = fopen(myName.c_str(), "w");
            if (!file) {
                fprintf(gErrFile, "gc: can't open %s: %s\n", strerror(errno));
                return JS_FALSE;
            }
            js_DumpGCHeap = file;
        } else {
            js_DumpGCHeap = stdout;
        }
#endif
        JS_GC(cx);
#ifdef GC_MARK_DEBUG
        if (js_DumpGCHeap != stdout)
            fclose(js_DumpGCHeap);
        js_DumpGCHeap = NULL;
#endif

#ifdef JS_GCMETER
        js_DumpGCStats(rt, stdout);
#endif
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
Clear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Clears a given object.");
    DOC_PARAM("theObject", "Specifies the object to clear", DOC_TYPE_OBJECT);
    DOC_END;
    try {
        if (argc > 0 && !JSVAL_IS_PRIMITIVE(argv[0])) {
            JS_ClearScope(cx, JSVAL_TO_OBJECT(argv[0]));
        } else {
            JS_ReportError(cx, "'clear' requires an object");
            return JS_FALSE;
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
MilliSec(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the time since program start in milliseconds.");
    DOC_RVAL("The time in milliseconds", DOC_TYPE_FLOAT);
    DOC_END;
    try {
        asl::NanoTime myTime = asl::NanoTime();
        jsdouble z = myTime.millis();
        return JS_NewDoubleValue(cx, z, rval);
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
MSleep(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sleeps for a certain time.");
    DOC_PARAM("theMilliSecs", "Specifies the number of milliseconds to sleep", DOC_TYPE_INTEGER);
    DOC_END;
    try {
        int32 myMilliSecs = 0;
        if (argc > 0) {
            if (!JS_ValueToInt32(cx, argv[0], &myMilliSecs)) {
                return JS_FALSE;
            }
        }

        msleep(myMilliSecs);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
ExpandEnvironment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Expands embedded environment variable of the type '${MYENVVAR}' in a given string.");
    DOC_PARAM("theString", "A string, that contains variables to expand", DOC_TYPE_STRING);
    DOC_RVAL("The string with expanded environment variables.", DOC_TYPE_STRING);
    DOC_END;
    try {
        if (argc != 1) {
            JS_ReportError(cx, "'expandEnvironment' takes a string as argument");
            return JS_FALSE;
        }

        string myString;
        if (!convertFrom(cx, argv[0], myString)) {
            JS_ReportError(cx, "expandEnvironment() could not convert argument value to string.");
            return JS_FALSE;
        }

        string myExpandedString = asl::expandEnvironment(myString);

		*rval = as_jsval(cx, myExpandedString);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}



JS_STATIC_DLL_CALLBACK(JSBool)
HostName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the fully qualified DNS name (NOTE: Only NetBIOS name will be returned under WIN9x \
while all other Windows versions return the fully qualified DNS name).");
    DOC_RVAL("The hostname", DOC_TYPE_STRING);
    DOC_END;
    try {
        if (argc != 0) {
            JS_ReportError(cx, "'hostname' does not take any arguments");
            return JS_FALSE;
        }

        string myHostName = hostname();
        *rval = as_jsval(cx, myHostName);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

jsval as_jsval(JSContext *cx, const ParameterDescription & theDocumentation) {
    JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
    jsval rval = OBJECT_TO_JSVAL(myReturnObject);
    if (!JS_DefineProperty(cx, myReturnObject, "name",  as_jsval(cx, theDocumentation.name), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "description", as_jsval(cx, theDocumentation.description), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "type", as_jsval(cx, theDocumentation.type), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "default_value",  as_jsval(cx, theDocumentation.default_value), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    return rval;
}

jsval as_jsval(JSContext *cx, const FunctionDescription & theDocumentation) {
    JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
    jsval rval = OBJECT_TO_JSVAL(myReturnObject);
    if (!JS_DefineProperty(cx, myReturnObject, "name",  as_jsval(cx, theDocumentation.name), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "description", as_jsval(cx, theDocumentation.description), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "parameters",  as_jsval(cx, theDocumentation.parameters), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "return_value",  as_jsval(cx, theDocumentation.return_value), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "return_type",  as_jsval(cx, theDocumentation.return_type), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    return rval;
}

jsval as_jsval(JSContext *cx, const ObjectDocumentation & theDocumentation) {
    JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
    jsval rval = OBJECT_TO_JSVAL(myReturnObject);
    if (!JS_DefineProperty(cx, myReturnObject, "constructors",  as_jsval(cx, theDocumentation.constructors), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "functions",  as_jsval(cx, theDocumentation.functions), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "properties", as_jsval(cx, theDocumentation.properties), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "static_functions",  as_jsval(cx, theDocumentation.static_functions), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "static_properties",  as_jsval(cx, theDocumentation.static_properties), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "constants",  as_jsval(cx, theDocumentation.constants), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    if (!JS_DefineProperty(cx, myReturnObject, "base_class",  as_jsval(cx, theDocumentation.base_class), 0,0, JSPROP_ENUMERATE)) return JSVAL_VOID;
    return rval;
}

template jsval as_jsval(JSContext *cx, const std::vector<ParameterDescription> & theVector);
template jsval as_jsval(JSContext *cx, const std::vector<FunctionDescription> & theVector);
    
JS_STATIC_DLL_CALLBACK(JSBool)
getDocumentedModules(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Collects all currently documented modules.");
    DOC_RVAL("An array of modulenames.", DOC_TYPE_ARRAY);
    DOC_END;
    try {
        // First collect all module names in a set
        set<string> myModules;
        for (DocumentationMap::iterator it = DocumentationSingleton::get().begin(); it != DocumentationSingleton::get().end(); ++it) {
            myModules.insert(it->first.first);
        }

        JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
        unsigned i = 0;
        for (set<string>::iterator it = myModules.begin(); it != myModules.end(); ++it) {
            jsval myValue = as_jsval(cx, *it);
            if (!JS_SetElement(cx, myReturnObject, i, &myValue)) {
                return JS_FALSE;
            }
            ++i;
        }
        *rval = OBJECT_TO_JSVAL(myReturnObject);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
getDocumentation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Collects all documentation for a given module.");
    DOC_PARAM("theModuleName", "Name of the Module to get the Documenation for.", DOC_TYPE_STRING);
    DOC_RVAL("An array of classes and functions, contained in the module", DOC_TYPE_ARRAY);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        std::string myModule;
        convertFrom(cx, argv[0], myModule);

        JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);

        for (DocumentationMap::iterator it = DocumentationSingleton::get().begin(); it != DocumentationSingleton::get().end(); ++it) {
            if (it->first.first == myModule) {
                string myClassName = it->first.second;
                if (!JS_DefineProperty(cx, myReturnObject, myClassName.c_str(),
                            as_jsval(cx, it->second), 0,0, JSPROP_ENUMERATE)) {
                    return JS_FALSE;
                }
            }
        }

        *rval = OBJECT_TO_JSVAL(myReturnObject);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
GetProgramName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the name of the executable, e.g. y60.exe");
    DOC_RVAL("The Programmname", DOC_TYPE_STRING);
    DOC_END;
    *rval = as_jsval(cx, JSApp::getProgramName());
    return JS_TRUE;
}

static JSBool
createUniqueId(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Generates an almost universally unique id string, containing a timestamp and current process-id, encoded as base65 string");
    DOC_RVAL("The unique id", DOC_TYPE_STRING);
    DOC_END;
    *rval = as_jsval(cx, IdTag::getDefault());
    return JS_TRUE;
}

static JSBool
fromHexString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Converts hex-characters into an integer. (e.g. 'FF' -> 255");
    DOC_PARAM("theHexValue", "A string of two hex-characters", DOC_TYPE_STRING);
    DOC_RVAL("An integer represented by thouse characters", DOC_TYPE_INTEGER);
    DOC_END;
    if (argc == 1) {
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "fromHexString(): Argument #%d is undefined", 1);
            return JS_FALSE;
        }

        string myString;
        if (!convertFrom(cx, argv[0], myString)) {
            JS_ReportError(cx, "fromHexString(): argument #1 must be a string");
            return JS_FALSE;
        }

        unsigned char myData[1];
        if (!asl::stringToBin(myString, (unsigned char *)&myData, 1)) {
            JS_ReportError(cx, "fromHexString(): stringToBin failed");
            return JS_FALSE;
        }

        *rval = INT_TO_JSVAL(myData[0]);
        return JS_TRUE;
    }
    JS_ReportError(cx,"fromHexString: bad number of arguments should be one, got %d", argc);
    return JS_FALSE;
}

static JSBool
asHexString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Converts a byte into a string of two hex-characters. (e.g. 255 -> 'FF'");
    DOC_PARAM("theNumber", "A integer between zero and 255", DOC_TYPE_INTEGER);
    DOC_RVAL("A string of two hex-characters representing the byte", DOC_TYPE_STRING);
    DOC_END;
    if (argc == 1) {
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "asHexString(): Argument #%d is undefined", 1);
            return JS_FALSE;
        }

        unsigned char myByte;
        if (!convertFrom(cx, argv[0], myByte)) {
            JS_ReportError(cx, "asHexString(): argument #1 must be a byte");
            return JS_FALSE;
        }

        string myString;
        binToString(&myByte, 1, myString);

        *rval = as_jsval(cx, myString);
        return JS_TRUE;
    }
    JS_ReportError(cx,"asHexString: bad number of arguments should be one, got %d", argc);
    return JS_FALSE;
}

static JSBool
urlEncode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Escapes URL strings (converts all letters consider illegal in URLs to their %XX versions).");
    DOC_PARAM("theUrl", "The url string to encode", DOC_TYPE_STRING);
    DOC_RVAL("The encoded url string", DOC_TYPE_STRING);
    DOC_END;
    if (argc == 1) {
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "urlEncode(): Argument #%d is undefined", 1);
            return JS_FALSE;
        }

        string myString;
        if (!convertFrom(cx, argv[0], myString)) {
            JS_ReportError(cx, "urlEncode(): argument #1 must be a string");
            return JS_FALSE;
        }

        *rval = as_jsval(cx, inet::Request::urlEncode(myString));
        return JS_TRUE;
    }
    JS_ReportError(cx,"urlEncode: bad number of arguments - should be one, got %d", argc);
    return JS_FALSE;
}

static JSBool
urlDecode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Unescapes URL encoding in strings (converts all %XX codes to their 8bit versions)");
    DOC_PARAM("theUrl", "The url string to decode", DOC_TYPE_STRING);
    DOC_RVAL("The decoded url string", DOC_TYPE_STRING);
    DOC_END;
    if (argc == 1) {
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "urlDecode(): Argument #%d is undefined", 1);
            return JS_FALSE;
        }

        string myString;
        if (!convertFrom(cx, argv[0], myString)) {
            JS_ReportError(cx, "urlDecode(): argument #1 must be a string");
            return JS_FALSE;
        }

        string myDecodedString = inet::Request::urlDecode(myString);
#if 1
        // KH,UH: the other code is not symmetric with urlEncode and results in non-UTF8 strings
        *rval = as_jsval(cx, myDecodedString);
#else
        // Here we cannot use the JS_NewUCStringCopyN function, because the result from urlDecode is encoded in Latin-1
        JSString * myJsString = JS_NewStringCopyN(cx, myDecodedString.c_str(), myDecodedString.size());
        *rval = STRING_TO_JSVAL(myJsString);
#endif
        return JS_TRUE;
    }
    JS_ReportError(cx,"urlDecode: bad number of arguments - should be one, got %d", argc);
    return JS_FALSE;
}

static JSBool
execute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Executes a command on the system");
    DOC_PARAM("theCommand", "The command to execute", DOC_TYPE_STRING);
    DOC_PARAM_OPT("theArgs", "Arguments", DOC_TYPE_STRING, "");
    DOC_PARAM_OPT("theBlockMode", "Blocking mode", DOC_TYPE_BOOLEAN, true);    
    DOC_RVAL("The return code of the command", DOC_TYPE_INTEGER);
    DOC_END;
    ensureParamCount(argc, 1, 3);
    for (unsigned i = 0; i < argc; ++i) {
        if (JSVAL_IS_VOID(argv[i])) {
            JS_ReportError(cx, "JSSerial::open(): Argument #%d is undefined", i + 1);
            return JS_FALSE;
        }
    }

    string myCommandString;
    if ( ! convertFrom(cx, argv[0], myCommandString)) {
        JS_ReportError(cx, "exec(): argument #1 must be a string");
        return JS_FALSE;
    }
    string myArgs("");
    string myCommand;
    bool myCommandIsBlocking = true;
    if (argc > 1) {
        if ( ! convertFrom(cx, argv[1], myArgs)) {
            JS_ReportError(cx, "exec(): argument #2 must be a string");
            return JS_FALSE;
        }
        myCommand = myCommandString;
        if (argc > 2) {
            if ( ! convertFrom(cx, argv[2], myCommandIsBlocking)) {
                JS_ReportError(cx, "exec(): argument #3 must be a bool");
                return JS_FALSE;
            }            
        }
    }
    if ( myArgs == "") {
        std::string::size_type myIndex = myCommandString.find(" ");
        if (myIndex != string::npos) {
            myCommand = myCommandString.substr(0, myIndex);
            myArgs = myCommandString.substr(myIndex + 1, myCommandString.size() - myIndex);
        } else {
            myCommand = myCommandString;
        }
    }
    int myRetVal;
    string myExecutionCommand = myCommand + " " + myArgs;
#ifdef WIN32
    string myCWD(".");
    STARTUPINFO myStartupInfo =
    {
        sizeof(STARTUPINFO),
        NULL, NULL, NULL, 0, 0, 0, 0, 0, 0,
        0, STARTF_USESHOWWINDOW, SW_SHOWDEFAULT,
        0, NULL, NULL, NULL, NULL
    };    
    PROCESS_INFORMATION myProcessInfo;    
    DWORD myProcessResult = 0;    
    bool myResult = CreateProcess(
            NULL, &myExecutionCommand[0],
            NULL, NULL, TRUE, 0,
            NULL, myCWD.c_str(), 
            &myStartupInfo,
            &myProcessInfo);
    if (myCommandIsBlocking) {
        myProcessResult = WaitForSingleObject(myProcessInfo.hProcess, INFINITE);
        GetExitCodeProcess(myProcessInfo.hProcess, &myProcessResult);
    }
    CloseHandle(myProcessInfo.hThread);
    CloseHandle(myProcessInfo.hProcess);
    myRetVal = myProcessResult;
    if (myRetVal > 32) {
        myRetVal = 0;
    }
#else
    myRetVal = system(myExecutionCommand.c_str());
    if (myRetVal && (myRetVal != -1)) {
        myRetVal = WEXITSTATUS(myRetVal);
    }
#endif
    *rval = as_jsval(cx, myRetVal);
    return JS_TRUE;
}

static JSBool
operatingSystem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the name of the operating system");
    DOC_RVAL("Win32, Linux or MacOsX", DOC_TYPE_STRING);
    DOC_END;
#ifdef WIN32
    string myOsName("Win32");
#endif
#ifdef LINUX
    string myOsName("Linux");
#endif
#ifdef OSX
    string myOsName("MacOsX");
#endif
    *rval = as_jsval(cx, myOsName);
    return JS_TRUE;
};


static JSFunctionSpec glob_functions[] = {
    {"print",             Print,          0},
    {"readStdIn",         ReadStdIn,      1},
    {"use",               Use,            1},
    {"reuse",             Reuse,          0},
    {"parseArguments",    ParseArguments, 2},
    {"plug",              Plug,           1},
    {"saveImage",         SaveImage,		2},
    {"saveImageFiltered", SaveImageFiltered,	4},
    {"applyImageFilter",  ApplyImageFilter,	3},
    {"blitImage",         BlitImage,	3},
    {"exit",              Exit,           0},
    {"version",           Version,        1},
    {"build",             BuildDate,      0},
    {"revision",          Revision,       0},
    {"gc",                GC,             0},
    {"clear",             Clear,          1},
    {"__FILE__",          File,           0},
    {"__LINE__",          Line,           0},
    {"fileline",          FileLine,       0},
    {"dumpstack",         DumpStack,      0},
    {"millisec",          MilliSec,       1},
    {"msleep",            MSleep,         1},
    {"getProgramName",    GetProgramName, 0},
    {"hostname",          HostName,       1},
    {"expandEnvironment", ExpandEnvironment, 1},

    {"getDocumentation",     getDocumentation, 1},
    {"getDocumentedModules", getDocumentedModules, 0},

    {"createUniqueId",  createUniqueId, 0},

    {"fromHexString",   fromHexString,   1},
    {"asHexString",     asHexString,     1},
    {"urlEncode",       urlEncode,       1},
    {"urlDecode",       urlDecode,       1},

    {"exec",            execute,         2},
    {"operatingSystem", operatingSystem, 0},
    {0}
};

static JSClass global_class = {
    "global", 0,
    JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,   JS_FinalizeStub
};

void
JSApp::Process(JSContext *cx, JSObject *obj, const char *filename) {
    JSScript *script;
    jsval result;
    DoBeginRequest(cx);
    script = JS_CompileFile(cx, obj, filename);
    if (script) {
        (void)JS_ExecuteScript(cx, obj, script, &result);
        JS_DestroyScript(cx, script);
    }
    DoEndRequest(cx);
}

void
JSApp::setupPath(const std::string & theIncludePath) {
    asl::PackageManagerPtr myPacketManager = getPackageManager();

    // Notice: Pathes are searched in reverse order as they are added!

    // Add the current working directory
    myPacketManager->add(asl::IPackagePtr(new asl::DirectoryPackage("")));

    // Add the application directory (the directoy y60.exe is located)
    myPacketManager->add(asl::getAppDirectory());

    // Add the Y60_PATH if set
    std::string myY60Path = asl::expandEnvironment("${Y60_PATH}");
    if (!myY60Path.empty()) {
        myPacketManager->add(myY60Path);
    }

    // Add the "-I" path
    if (!theIncludePath.empty()) {
        myPacketManager->add(theIncludePath);
    }

    PlugInManager::get().setSearchPath(myPacketManager->getSearchPath());
}

int
JSApp::processArguments(JSContext * theContext, JSObject * theGlobalObject,
                        const std::string & theScriptFilename,
                        const std::string & theIncludePath,
                        const std::vector<std::string> & theScriptArgs)
{
    if (ourJSVersion > 0) {
        JS_SetVersion(theContext, JSVersion(ourJSVersion));
    }

    if (ourStrictFlag) {
        JS_ToggleOptions(theContext, JSOPTION_STRICT);
    }

    vector<string> myArgs = theScriptArgs;
    jsval myArgumentsValue = as_jsval(theContext, myArgs);

    if (!myArgumentsValue) {
        return 1;
    }

    if (!JS_SetProperty(theContext, theGlobalObject, "arguments", &myArgumentsValue)) {
        return 1;
    }

    setupPath(theIncludePath);
    string myScriptFile = getPackageManager()->searchFile(theScriptFilename);

    if (myScriptFile.empty()) {
        AC_ERROR << "File '" << theScriptFilename << "' not found in " << getPackageManager()->getSearchPath() << std::endl;
        return 1;
    }

    Process(theContext, theGlobalObject, myScriptFile.c_str());
    return ourExitCode;
}

bool
JSApp::initClasses(JSContext * theContext, JSObject * theGlobalObject) {
    if (!JS_InitStandardClasses(theContext, theGlobalObject)) {
        return false;
    }

    if (!initCppClasses(theContext, theGlobalObject)) {
        return false;
    }
    return true;
}

int
JSApp::run(const std::string & theScriptFilename,
           const std::string & theIncludePath,
           const std::vector<std::string> & theScriptArgs)
{
    JSObject *glob;
    JSRuntime * rt = JS_NewRuntime(1024 * 1024 * 128); // Bytes allocated before garbage collection
    int result;

    gErrFile = stderr;
    gOutFile = stderr;

    ourJSContext = JS_NewContext(rt, 8192);
    if (!ourJSContext) {
        AC_FATAL << "JS_NewContext failed!";
        return 1;
    }

    ourIncludeGuard.clear();

    JS_SetErrorReporter(ourJSContext, ShellErrorReporter);

    glob = JS_NewObject(ourJSContext, &global_class, NULL, NULL);
    if (!glob)
        return 1;

    if (!JS_DefineFunctions(ourJSContext, glob, glob_functions)) {
        return 1;
    }

    if (!initClasses(ourJSContext, glob)) {
        return 1;
    }

    createFunctionDocumentation("GlobalFunctions", glob_functions);

    ourTopScriptFilename = theScriptFilename;
    result = processArguments(ourJSContext, glob, ourTopScriptFilename, theIncludePath, theScriptArgs);

    JS_ClearScope(ourJSContext, glob);
    JS_GC(ourJSContext);
    JS_DestroyContext(ourJSContext);
    JS_DestroyRuntime(rt);
    ourLoadedPlugIns.clear();
    return result;
}

}
