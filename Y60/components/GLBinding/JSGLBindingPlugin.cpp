//===========================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: GLBinding.cpp,v $
//
//     $Author: valentin $
//
//   $Revision: 1.6 $
//
// Description:
//
//=============================================================================


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
    		    static const char * myClassName = "GLBinding";
    		    return myClassName;
    		}
	};
	
    JS_STATIC_DLL_CALLBACK(JSBool)
    glEnable(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
        try {
            DOC_BEGIN("");
            DOC_END;
            ensureParamCount(argc, 1);

            unsigned  myGLFlag;
            if (!convertFrom(cx, argv[0], myGLFlag)) {
                JS_ReportError(cx,"glEnable: argument 1 is not a unsigned");
                return JS_FALSE;
            }            
            ::glEnable(myGLFlag);
            return JS_TRUE;
    
        } HANDLE_CPP_EXCEPTION;
    }
    JS_STATIC_DLL_CALLBACK(JSBool)
    glDisable(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
        try {
            DOC_BEGIN("");
            DOC_END;
            ensureParamCount(argc, 1);

            unsigned  myGLFlag;
            if (!convertFrom(cx, argv[0], myGLFlag)) {
                JS_ReportError(cx,"glDisable: argument 1 is not a unsigned");
                return JS_FALSE;
            }            
            ::glDisable(myGLFlag);
            return JS_TRUE;
    
        } HANDLE_CPP_EXCEPTION;
    }


    enum PropertyNumbers {
        PROP_BLENDING = GL_BLEND,
    };
    #define DEFINE_PROPERTY(NAME) { #NAME, PROP_ ## NAME , NAME }

    JSConstIntPropertySpec *
    GLBinding::ConstIntProperties() {
        const unsigned short BLENDING        = GL_BLEND;
        
        static JSConstIntPropertySpec myProperties[] = {
                DEFINE_PROPERTY(BLENDING),
        {0}
    };
    return myProperties;
};



	
    JSFunctionSpec * GLBinding::StaticFunctions() {
        static JSFunctionSpec myFunctions[] = {
            // name                         native                       nargs    
            {"glEnable",             glEnable,             1},
            {"glDisable",            glDisable,             1},
            {0}
        };
        return myFunctions;
    }    
}

extern "C"
EXPORT asl::PlugInBase * GLBinding_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new jslib::GLBinding(myDLHandle);
}
