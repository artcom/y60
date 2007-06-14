//==============================================================================
//
// Copyright (C) 2007, ART+COM AG Berlin 
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//==============================================================================


#include <asl/UnitTest.h>
#include <asl/Logger.h>
#include <y60/jssettings.h>
#include <y60/JSVector.h>
#include <js/jsapi.h>
#include <js/jsprf.h>
#include <js/jsparse.h>
#include <js/jsscan.h>
#include <js/jsemit.h>
#include <js/jsscript.h>
#include <js/jsarena.h>
#include <js/jscntxt.h>
#include <js/jsdbgapi.h>
#include <js/jsscope.h>

#include <errno.h>

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
    JSRuntime *rt;
    uint32 maxObjects = 0;

    ensureParamCount(argc, 0, 1);

    if (argc >= 1 && JSVAL_IS_INT(argv[0])) {
      maxObjects = JSVAL_TO_INT(argv[0]);
    }

    rt = cx->runtime;

    JS_IncrementalGC(cx, maxObjects);

#ifdef JS_GCMETER
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

char * filenames[] = { "../../emptyScript.js", "../../testJSObjects.js", "../../testManyObjects.js", "../../testNative.js", "../../perfect.js", NULL };

#define MAGIC_NUMBER 137

class GarbageCollectionTest : public UnitTest {
public:
    GarbageCollectionTest() : 
        UnitTest("GarbageCollectionUnitTest")
    {}

    void generateBaseline() {
        FILE *outfile = fopen("../../BASELINE", "w+");
        for (char **fileName = filenames; *fileName; fileName++) {
            init();
            runTestScript(*fileName);
            cleanup(true);
            fprintf(outfile, "%s\t%d\n", *fileName, rt->gcObjects);
        }
        fclose(outfile);
    }

    void run() {

	/*
        generateBaseline();
        return;
	*/

        FILE *infile = fopen("../../BASELINE", "r");

        char fileName[1024];
	int magicNumber;

        while (fscanf(infile, "%s\t%u\n", fileName, &magicNumber) == 2) {

            init();
            checkInit();
            runTestScript(fileName);
            cleanup(true);
            checkCleanup(magicNumber);

            init();
            runTestScript(fileName);
            cleanup(false);
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
        AC_PRINT << "GC objects after initialization: " << rt->gcObjects;

        ENSURE(globalObject);
        ENSURE(JSVector<asl::Vector3f>::initClass(cx, globalObject));
    }        

    void checkInit() {
        // actually, we don't check anything here.
    }

    void cleanup(bool force = true) {
        JS_ClearScope(cx, globalObject);

	if (!force) {
            // XXX überleg dir dazu mal nen Kommentar, Tobi.
	    for (int i = 0; i < 2; i++) {
		int j = 1000;
		while (!(JS_IncrementalGC(cx, 1000)) && --j > 0);
		ENSURE_MSG((j >= 0), "GC cycles < 1000");
	    };
	} else {
	    JS_GC(cx);
	}
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
        AC_PRINT << "GC Objects after execution of test: "
                 << rt->gcObjects << "; theFinalObjectCount = " << theFinalObjectCount;
        ENSURE(rt->gcObjects == theFinalObjectCount);
        JS_DestroyContext(cx);
        ENSURE(rt->gcObjects == 0);
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
            AC_PRINT << "GC Objects after execution of script [" 
                     << theFileName << "]: " << cx->runtime->gcObjects;
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
