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


#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSWrapper.h>

#include <y60/glutil/GLUtils.h>

using namespace asl;
using namespace y60;
namespace jslib {
    typedef bool DummyT;

	class GLBinding : public asl::PlugInBase, public jslib::IScriptablePlugin {
    	public:
            typedef DummyT NATIVE;
            typedef asl::Ptr<DummyT> OWNERPTR;
            typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;

    		GLBinding(asl::DLHandle theDLHandle) : asl::PlugInBase(theDLHandle) {}

            virtual JSFunctionSpec * StaticFunctions();
            virtual JSConstIntPropertySpec * ConstIntProperties();

    		const char * ClassName() {
    		    static const char * myClassName = "gl";
    		    return myClassName;
    		}
	};

    JS_STATIC_DLL_CALLBACK(JSBool)
    Enable(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
        try {
            DOC_BEGIN("");
            DOC_END;
            ensureParamCount(argc, 1);

            unsigned  myGLFlag;
            if (!convertFrom(cx, argv[0], myGLFlag)) {
                JS_ReportError(cx,"Enable: argument 1 is not a unsigned");
                return JS_FALSE;
            }
            ::glEnable(myGLFlag);
            return JS_TRUE;

        } HANDLE_CPP_EXCEPTION;
    }
    JS_STATIC_DLL_CALLBACK(JSBool)
    Disable(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
        try {
            DOC_BEGIN("");
            DOC_END;
            ensureParamCount(argc, 1);

            unsigned  myGLFlag;
            if (!convertFrom(cx, argv[0], myGLFlag)) {
                JS_ReportError(cx,"Disable: argument 1 is not a unsigned");
                return JS_FALSE;
            }
            ::glDisable(myGLFlag);
            return JS_TRUE;

        } HANDLE_CPP_EXCEPTION;
    }

    JS_STATIC_DLL_CALLBACK(JSBool)
    GetTotalMem(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
        try {
            DOC_BEGIN("");
            DOC_END;
            GLint total_mem_kb = 0;
            if (y60::hasCap("GL_NVX_gpu_memory_info")) {
                #define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
                glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb);
            } else {
                throw asl::Exception(std::string("GL::GetTotalMem, sorry GL_NVX_gpu_memory_info extension not available."), PLUS_FILE_LINE);
            }
            *rval = as_jsval(cx, total_mem_kb);
            return JS_TRUE;

        } HANDLE_CPP_EXCEPTION;
    }

    JS_STATIC_DLL_CALLBACK(JSBool)
    GetFreeMem(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
        try {
            DOC_BEGIN("");
            DOC_END;
            GLint cur_avail_mem_kb = 0;
            if (y60::hasCap("GL_NVX_gpu_memory_info")) {
                #define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
                glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb);
            } else {
                throw asl::Exception(std::string("GL::GetFreeMem, sorry GL_NVX_gpu_memory_info extension not available."), PLUS_FILE_LINE);
            }
            *rval = as_jsval(cx, cur_avail_mem_kb);
            return JS_TRUE;

        } HANDLE_CPP_EXCEPTION;
    }




    enum PropertyNumbers {
        PROP_BLENDING = GL_BLEND,
    };
    #define DEFINE_PROPERTY(NAME) { #NAME, PROP_ ## NAME , NAME }

    JSConstIntPropertySpec *
    GLBinding::ConstIntProperties() {
        const unsigned short BLENDING = GL_BLEND;

        static JSConstIntPropertySpec myProperties[] = {
                DEFINE_PROPERTY(BLENDING),
        {0}
    };
    return myProperties;
};




    JSFunctionSpec * GLBinding::StaticFunctions() {
        static JSFunctionSpec myFunctions[] = {
            // name                         native                       nargs
            {"Enable",             Enable,             1},
            {"Disable",            Disable,            1},
            {"GetTotalMem",    GetTotalMem,    1},
            {"GetFreeMem",         GetFreeMem,         1},
            {0}
        };
        return myFunctions;
    }
}

extern "C"
EXPORT asl::PlugInBase * GLBinding_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new jslib::GLBinding(myDLHandle);
}
