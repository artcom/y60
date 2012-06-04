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

#include <asl/base/UnitTest.h>
#include <asl/base/Logger.h>
#include <y60/jsbase/jssettings.h>
#include <y60/jsbase/JSVector.h>

#ifdef USE_LEGACY_SPIDERMONKEY
#include <js/spidermonkey/jsapi.h>
#include <js/spidermonkey/jsprf.h>
#include <js/spidermonkey/jsparse.h>
#include <js/spidermonkey/jsscan.h>
#include <js/spidermonkey/jsemit.h>
#include <js/spidermonkey/jsscript.h>
#include <js/spidermonkey/jsarena.h>
#include <js/spidermonkey/jscntxt.h>
#include <js/spidermonkey/jsdbgapi.h>
#include <js/spidermonkey/jsscope.h>
#else
#include <js/jsapi.h>
#include <js/jsprf.h>
//#include <js/jsparse.h>
//#include <js/jsscan.h>
#include <js/jsemit.h>
#include <js/jsscript.h>
#include <js/jsarena.h>
#include <js/jscntxt.h>
#include <js/jsdbgapi.h>
#include <js/jsscope.h>
#endif

#include <errno.h>

#include <acmake/y60jsbase_paths.h>
#define TEST_DATA_DIR CMAKE_CURRENT_SOURCE_DIR

FILE *gErrFile = stderr;
FILE *gOutFile = stdout;
FILE *js_DumpGCHeap = NULL;


using namespace std;
using namespace asl;
using namespace jslib;

static JSClass global_class = {
    "global", 0,
    JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,   JS_FinalizeStub
};

static JSBool
GC(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uint32 maxObjects = 0;

    ensureParamCount(argc, 0, 1);

    if (argc >= 1 && JSVAL_IS_INT(argv[0])) {
      maxObjects = JSVAL_TO_INT(argv[0]);
    }

#ifdef USE_SPIDERMONKEY_INCREMENTAL_GC
    JS_IncrementalGC(cx, maxObjects);
#endif

#ifdef JS_GCMETER
    JSRuntime *rt;
    rt = cx->runtime;
    js_DumpGCStats(rt, stdout);
#endif
    return JS_TRUE;
}


static JSBool
Print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uintN i, n;
    JSString *str;

    for (i = n = 0; i < argc; i++) {
        str = JS_ValueToString(cx, argv[i]);
        if (!str)
            return JS_FALSE;
        fprintf(gOutFile, "%s%s", i ? " " : "", JS_GetStringBytes(str));
    }
    n++;
    if (n)
        fputc('\n', gOutFile);
    return JS_TRUE;
}

static JSFunctionSpec global_functions[] = {
    {"print",             Print,          1},
    {"gc",                GC,             1},
    {0}
};

const char * filenames[] = {
    TEST_DATA_DIR "/emptyScript.js",
    TEST_DATA_DIR "/testJSObjects.js",
    TEST_DATA_DIR "/testManyObjects.js",
    TEST_DATA_DIR "/testNative.js",
    TEST_DATA_DIR "/perfect.js",
    NULL
};

#define MAGIC_NUMBER 137

class GarbageCollectionTest : public UnitTest {
public:
    GarbageCollectionTest() :
        UnitTest("GarbageCollectionUnitTest")
    {}

    void generateBaseline() {
        FILE *outfile = fopen( TEST_DATA_DIR "/BASELINE", "w+");
        for (const char **fileName = filenames; *fileName; fileName++) {
            init();
            runTestScript(*fileName);
            cleanup(false);
#ifdef USE_SPIDERMONKEY_INCREMENTAL_GC
            fprintf(outfile, "%s\t%d\n", *fileName, rt->gcObjects);
#endif
        }
        fclose(outfile);
    }

    void run() {

        // Store a new baseline in the source tree
        /*
        generateBaseline();
        return;
        */

        FILE *infile = fopen(TEST_DATA_DIR "/BASELINE", "r");
        if( !infile ) {
            AC_PRINT << "Cannot open baseline";
            return;
        }

        char fileName[1024];
        int magicNumber;

        while (fscanf(infile, "%s\t%u\n", fileName, &magicNumber) == 2) {

            init();
            checkInit();
            runTestScript((std::string(TEST_DATA_DIR) + "/" + fileName).c_str());
            cleanup(false);
            checkCleanup(magicNumber);

            init();
            runTestScript((std::string(TEST_DATA_DIR) + "/" + fileName).c_str());
            cleanup(true);
            checkCleanup(magicNumber);
        }
    }

    void init() {
        rt = JS_NewRuntime(1024 * 1024 * 128);
        ENSURE(rt);
        cx = JS_NewContext(rt, 8129);
        ENSURE(cx);
        globalObject = JS_NewObject(cx, &global_class, NULL, NULL);
        ENSURE(globalObject);
        ENSURE(JS_DefineFunctions(cx, globalObject, global_functions));
        ENSURE(JS_InitStandardClasses(cx, globalObject));
#ifdef USE_SPIDERMONKEY_INCREMENTAL_GC
        AC_PRINT << "GC objects after initialization: " << rt->gcObjects;
#endif

        ENSURE(globalObject);
        ENSURE(JSVector<asl::Vector3f>::initClass(cx, globalObject));
    }

    void checkInit() {
        // actually, we don't check anything here.
    }

    void cleanup(bool incremental = false) {
        JS_ClearScope(cx, globalObject);

	if (incremental) {
        int j = 10000;
#ifdef USE_SPIDERMONKEY_INCREMENTAL_GC
	    while (!(JS_IncrementalGC(cx, 100)) && --j > 0);
#endif
	    ENSURE_MSG((j >= 0), "GC cycles < 1000");
	}
        // finally, do a full forced gc to get rid of newborns and the like.
        JS_GC(cx);
    }

    void checkCleanup(unsigned theFinalObjectCount) {
#ifdef GC_MARK_DEBUG
        FILE *file = fopen("cleanup.log", "w");
        if (!file) {
            fprintf(gErrFile, "gc: can't open %s: %s\n", strerror(errno));
            return;
        }
        js_DumpGCHeap = file;
#endif
        JS_GC(cx);
#ifdef GC_MARK_DEBUG
        fclose(js_DumpGCHeap);
        js_DumpGCHeap = NULL;
#endif
#ifdef USE_SPIDERMONKEY_INCREMENTAL_GC
        AC_PRINT << "GC Objects after execution of test: "
                 << rt->gcObjects << "; theFinalObjectCount = " << theFinalObjectCount;
        ENSURE(rt->gcObjects == theFinalObjectCount);
#endif
        JS_DestroyContext(cx);
#ifdef USE_SPIDERMONKEY_INCREMENTAL_GC
        ENSURE(rt->gcObjects == 0);
#endif
        JS_DestroyRuntime(rt);
    }

    void runTestScript(const char *theFileName) {
        JSScript *script;
        jsval result;
        script = JS_CompileFile(cx, globalObject, theFileName);
        ENSURE(script);
        if (script) {
            (void)JS_ExecuteScript(cx, globalObject, script, &result);
            JS_DestroyScript(cx, script);
#ifdef USE_SPIDERMONKEY_INCREMENTAL_GC
            AC_PRINT << "GC Objects after execution of script ["
                     << theFileName << "]: " << cx->runtime->gcObjects;
#endif
        } else {
            AC_ERROR << "Could not load testscript [" << theFileName << "]";
        }
        ENSURE(result);
    }

private:

    JSContext *cx;
    JSRuntime *rt;
    JSObject *globalObject;
};


int main(int argc, char *argv[]) {

    UnitTestSuite mySuite(argv[0], argc, argv);

    try {
        mySuite.addTest(new GarbageCollectionTest);
        mySuite.run();
    }
    catch (...) {
        cerr << "## An unknown exception occured during execution." << endl;
        mySuite.incrementFailedCount();
    }

    int returnStatus = -1;
    if (mySuite.getPassedCount() != 0) {
        returnStatus = 0;
    } else {
        cerr << "## No tests." << endl;

    }
    cerr << ">> Finsihed test suite '" << argv[0]
         << "', return status = " << returnStatus << endl;
    return returnStatus;

}
