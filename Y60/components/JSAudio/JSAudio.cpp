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
//   $RCSfile: JSAudio.cpp,v $
//   $Author: christian $
//   $Revision: 1.11 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSAudio.h"
#include <y60/JScppUtils.h>

#include <audio/AudioController.h>
#include <iostream>

using namespace std;
using namespace asl;
using namespace AudioApp;
using namespace jslib;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Prints 'JSAudio'. :)");
    DOC_END;
    std::string myStringRep = "JSAudio";
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

// TODO:
// - implement init from xml config
static JSBool
init(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Initialize audiosystem.");
    DOC_PARAM("theSamplerate", "", DOC_TYPE_INTEGER);
    DOC_PARAM_OPT("theLatency", "", DOC_TYPE_FLOAT, 0.1);
    DOC_END;
    try {
        checkForUndefinedArguments("JSAudio::init()", argc, argv);

        if (argc > 2) {
            throw BadArgumentException(string("JSAudio::init(): Wrong number of arguments. Got ") +
                as_string(argc) + ", expected between none and two.", PLUS_FILE_LINE);
        }

        AudioController & myController = AudioController::get();
        if (argc == 0) {
            myController.init();
            return JS_TRUE;
        }

        int mySampleRate = 0;
        if (!convertFrom(cx, argv[0], mySampleRate)) {
            JS_ReportError(cx, "JSAudio::init(): argument #1 must be an integer (Samplerate)");
            return JS_FALSE;
        }

        double myLatency = -1.0;
        if (argc == 2) {
            if (!convertFrom(cx, argv[1], myLatency)) {
                JS_ReportError(cx, "JSAudio::init(): argument #2 must be a double (Latency)");
                return JS_FALSE;
            }
        }

        if (myLatency == -1.0) {
            myController.init(mySampleRate);
        } else {
            myController.init(mySampleRate, myLatency);
        }

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
isRunning(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Check if AudioController currently running.");
    DOC_RVAL("Result", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        checkArguments("JSAudio::isRunning()", argc, argv, 0);

        AudioController & myController = AudioController::get();
        *rval = as_jsval(cx, myController.isRunning());

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

// TODO:
// - implement overloading
static JSBool
play(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Play an audio file.");
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
    DOC_PARAM("theModule", "Destination Module, 'Mixer'", DOC_TYPE_STRING);
    DOC_PARAM("theVolume", "", DOC_TYPE_FLOAT);
    DOC_PARAM("theSeektime", "", DOC_TYPE_FLOAT);
    DOC_PARAM("theLoopflag", "", DOC_TYPE_BOOLEAN);
    DOC_RVAL("Id", DOC_TYPE_STRING);
    DOC_END;
    try {
        checkArguments("JSAudio::play()", argc, argv, 5);

        string myFileName = "";
        if (!convertFrom(cx, argv[0], myFileName)) {
            JS_ReportError(cx, "JSAudio::play(): argument #1 must be a string (Filename)");
            return JS_FALSE;
        }

        string myDestinationModule = "";
        if (!convertFrom(cx, argv[1], myDestinationModule)) {
            JS_ReportError(cx, "JSAudio::play(): argument #2 must be a string (Destination Module)");
            return JS_FALSE;
        }

        double myVolume = 1.0;
        if (!convertFrom(cx, argv[2], myVolume)) {
            JS_ReportError(cx, "JSAudio::play(): argument #3 must be a double (Volume)");
            return JS_FALSE;
        }

        double mySeekTime = 0.0;
        if (!convertFrom(cx, argv[3], mySeekTime)) {
            JS_ReportError(cx, "JSAudio::play(): argument #4 must be a double (Seek Time)");
            return JS_FALSE;
        }

        bool myLoopFlag = false;
        if (!convertFrom(cx, argv[4], myLoopFlag)) {
            JS_ReportError(cx, "JSAudio::play(): argument #5 must be a boolean (Loop Flag)");
            return JS_FALSE;
        }

        AudioController & myController = AudioController::get();
        string mySoundId = "-1";
        try{
            mySoundId = myController.play(myFileName, myDestinationModule, myVolume, mySeekTime, myLoopFlag);
        } catch (asl::Exception & ex) {
            AC_ERROR << "Exception in AudioController when playing " << myFileName << ":" << endl << ex << endl;
        } catch (...) {
            AC_ERROR << "Unknown exception in AudioController when playing: " << myFileName << endl;
        }
        *rval = as_jsval(cx, mySoundId);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Stop an audio file.");
    DOC_PARAM("theId", "", DOC_TYPE_STRING);
    DOC_END;
    try {
        checkArguments("JSAudio::stop()", argc, argv, 1);

        string myId = "";
        if (!convertFrom(cx, argv[0], myId)) {
            JS_ReportError(cx, "JSAudio::stop(): argument #1 must be a string (Id)");
            return JS_FALSE;
        }

        AudioController & myController = AudioController::get();
        myController.stop(myId);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
setSeekOffset(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Set seektime");
    DOC_PARAM("thId", "", DOC_TYPE_STRING);
    DOC_PARAM("theSeektime", "", DOC_TYPE_FLOAT);
    DOC_END;
    try {
        checkArguments("JSAudio::stop()", argc, argv, 2);

        string myId = "";
        if (!convertFrom(cx, argv[0], myId)) {
            JS_ReportError(cx, "JSAudio::play(): argument #1 must be a string (Id)");
            return JS_FALSE;
        }
        double mySeekOffset = 0.0;
        if (!convertFrom(cx, argv[1], mySeekOffset)) {
            JS_ReportError(cx, "JSAudio::setSeekOffset(): argument #2 must be a double (SeekOffset)");
            return JS_FALSE;
        }

        AudioController & myController = AudioController::get();
        myController.setSeekOffset(myId, mySeekOffset);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}


static JSBool
pause(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Pauses an audio file.");
    DOC_PARAM("theId", "", DOC_TYPE_STRING);
    DOC_END;
    try {
        checkArguments("JSAudio::pause()", argc, argv, 1);

        string myId = "";
        if (!convertFrom(cx, argv[0], myId)) {
            JS_ReportError(cx, "JSAudio::pause(): argument #1 must be a string (Id)");
            return JS_FALSE;
        }

        AudioController & myController = AudioController::get();
        myController.pause(myId);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
isPlaying(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Check if an audio file is playing.");
    DOC_PARAM("theId", "", DOC_TYPE_STRING);
    DOC_RVAL("Result", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        checkArguments("JSAudio::isPlaying()", argc, argv, 1);

        string myId = "";
        if (!convertFrom(cx, argv[0], myId)) {
            JS_ReportError(cx, "JSAudio::isPlaying(): argument #1 must be a string (Id)");
            return JS_FALSE;
        }

        AudioController & myController = AudioController::get();
        *rval = as_jsval(cx, myController.isPlaying(myId));

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
stopAllSounds(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Stop all audio files.");
    DOC_END;
    try {
        checkArguments("JSAudio::stopAllSounds()", argc, argv, 0);

        AudioController & myController = AudioController::get();
        myController.stopAllSounds();

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
setVolume(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Set volume for audio file");
    DOC_PARAM("theId", "", DOC_TYPE_STRING);
    DOC_PARAM("theVolume", "", DOC_TYPE_FLOAT);
    DOC_END;
    try {
        checkArguments("JSAudio::setVolume()", argc, argv, 2);

        string myId = "";
        if (!convertFrom(cx, argv[0], myId)) {
            JS_ReportError(cx, "JSAudio::setVolume(): argument #1 must be a string (Id)");
            return JS_FALSE;
        }

        double myVolume = 1.0;
        if (!convertFrom(cx, argv[1], myVolume)) {
            JS_ReportError(cx, "JSAudio::setVolume(): argument #2 must be a double (Volume)");
            return JS_FALSE;
        }

        AudioController & myController = AudioController::get();
        myController.setVolume(myId, myVolume);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
getVolume(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Get volume of audio file");
    DOC_PARAM("tehId", "", DOC_TYPE_STRING);
    DOC_RVAL("Volume", DOC_TYPE_FLOAT);
    DOC_END;
    try {
        checkArguments("JSAudio::getVolume()", argc, argv, 1);

        string myId = "";
        if (!convertFrom(cx, argv[0], myId)) {
            JS_ReportError(cx, "JSAudio::getVolume(): argument #1 must be a string (Id)");
            return JS_FALSE;
        }

        AudioController & myController = AudioController::get();
        *rval = as_jsval(cx, myController.getVolume(myId));

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
getDuration(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Get duration of audio file");
    DOC_PARAM("theId", "", DOC_TYPE_STRING);
    DOC_RVAL("Duration", DOC_TYPE_FLOAT);
    DOC_END;
    try {
        checkArguments("JSAudio::getDuration()", argc, argv, 1);

        string myId = "";
        if (!convertFrom(cx, argv[0], myId)) {
            JS_ReportError(cx, "JSAudio::getDuration(): argument #1 must be a string (Id)");
            return JS_FALSE;
        }

        AudioController & myController = AudioController::get();
        double myDuration = -1.0;
        try {
            myDuration = myController.getDuration(myId);
        } catch (...) {
            myDuration = -1.0;
        }
        *rval = as_jsval(cx, myDuration);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSAudio::Functions() {
    AC_DEBUG << "Registering class '" << ClassName() << "'" << endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"init",                 init,                    2},
        {"isRunning",            isRunning,               0},
        {"play",                 play,                    5},
        {"pause",                pause,                   1},
        {"stop",                 stop,                    1},
        {"setSeekOffset",        setSeekOffset,           2},
        {"isPlaying",            isPlaying,               1},
        {"stopAllSounds",        stopAllSounds,           0},
        {"setVolume",            setVolume,               2},
        {"getVolume",            getVolume,               1},
        {"getDuration",          getDuration,             1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSAudio::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSAudio::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSAudio::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSAudio::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

JSBool
JSAudio::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Constructs an AudioController");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSAudio * myNewObject = 0;

    OWNERPTR myNewNative = OWNERPTR(new int);
    myNewObject = new JSAudio(myNewNative, &(*myNewNative));

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSAudio::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSAudio::initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
        DOC_CREATE(JSAudio);
        return myClass;
}


