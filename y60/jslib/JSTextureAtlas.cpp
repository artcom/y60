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

#include "JSTextureAtlas.h"
#include "JSApp.h"

#include <y60/jsbase/JSMatrix.h>
#include <y60/jsbase/JSWrapper.impl>

#include <y60/image/ImageLoader.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace y60;

namespace jslib {

template class JSWrapper<y60::TextureAtlas, asl::Ptr<y60::TextureAtlas>, StaticAccessProtocol>;

typedef y60::TextureAtlas NATIVE;

static JSBool
FindTextureTranslation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the UV translation matrix for a given subtexture");
    DOC_PARAM("theSubTextureName", "", DOC_TYPE_STRING);
    DOC_RVAL("translation", DOC_TYPE_MATRIX4F);
    DOC_END;
    try {
        ensureParamCount(argc, 1, PLUS_FILE_LINE);

        JSTextureAtlas::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        string theSubTextureName;
        convertFrom(cx, argv[0], theSubTextureName);

        Matrix4f translationMatrix;
        if ( myNative->findTextureTranslation(theSubTextureName, translationMatrix) ) {
            *rval = as_jsval(cx, translationMatrix);
        } else {
            *rval = JSVAL_NULL;
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
FindTextureSize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the original size (in pixels) of a given subtexture");
    DOC_PARAM("theSubTextureName", "", DOC_TYPE_STRING);
    DOC_RVAL("size", DOC_TYPE_VECTOR2I);
    DOC_END;
    try {
        ensureParamCount(argc, 1, PLUS_FILE_LINE);

        JSTextureAtlas::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        string theSubTextureName;
        convertFrom(cx, argv[0], theSubTextureName);

        Vector2<AC_SIZE_TYPE> textureSize;
        if ( myNative->findTextureSize(theSubTextureName, textureSize) ) {
            *rval = as_jsval(cx, textureSize);
        } else {
            *rval = JSVAL_NULL;
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
SaveToFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Saves the Atlas to the given file");
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
    DOC_END;
    try {
        ensureParamCount(argc, 1, PLUS_FILE_LINE);

        JSTextureAtlas::OWNERPTR myNative;
        convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative);

        string theSaveToFilename;
        convertFrom(cx, argv[0], theSaveToFilename);

        myNative->saveToFile(Path(theSaveToFilename, UTF8));
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSTextureAtlas::Functions() {
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"findTextureTranslation",   FindTextureTranslation,          1},
        {"findTextureSize",          FindTextureSize,                 1},
        {"saveToFile",               SaveToFile            ,          1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSTextureAtlas::Properties() {
    static JSPropertySpec myProperties[] = {
        {"imagePath", PROP_IMAGE_PATH, JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSTextureAtlas::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSTextureAtlas::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSTextureAtlas::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSTextureAtlas::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_IMAGE_PATH:
            *vp = as_jsval(cx, getNative().getRasterPath().toUTF8());
            return JS_TRUE;
        default:
            JS_ReportError(cx,"JSTextureAtlas::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}


// setproperty handling
JSBool
JSTextureAtlas::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    /*switch (theID) {
        default:*/
            JS_ReportError(cx,"JSTextureAtlas::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    //}
}

TextureAtlas::Subtextures
JSTextureAtlas::createSubtexturesFromJSObject(JSContext *cx, JSObject * theSubtexturesObject) {
    TextureAtlas::Subtextures mySubtextures;

    JSIdArray * myPropIds = JS_Enumerate(cx, theSubtexturesObject);

    for (int i = 0; i < myPropIds->length; ++i) {
        jsval myIdVal;
        if (JS_IdToValue(cx,myPropIds->vector[i],&myIdVal)) {
            string textureName;
            convertFrom(cx, myIdVal, textureName);
            jsval myValue;
            JS_GetProperty(cx, theSubtexturesObject, textureName.c_str(), &myValue);
            string texturePath;
            if (!convertFrom(cx, myValue, texturePath)) {
                throw ArgumentConversionFailed("JSTextureAtlas: value for '"+textureName+"' must be a file path (string)", PLUS_FILE_LINE);
            };
            ImageLoader imageLoader(Path(texturePath, UTF8).toLocale(), JSApp::getPackageManager());
            
            dom::ResizeableRasterPtr myRaster = imageLoader.getRaster();
            mySubtextures.insert(make_pair(textureName, myRaster));
        }
    }
    if (myPropIds) {
        JS_DestroyIdArray(cx, myPropIds);
        myPropIds = 0;
    }
    return mySubtextures;
}


JSBool
JSTextureAtlas::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Constructs a TextureAtlas from an atlas definition file");
    DOC_PARAM("theAtlasFile", "Filename of an atlas xml file", DOC_TYPE_STRING);
    DOC_END;
    try {
        if (JSA_GetClass(cx,obj) != Class()) {
            JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
            return JS_FALSE;
        }
        JSTextureAtlas * myNewObject = 0;


        ensureParamCount(argc, 1, 3, PLUS_FILE_LINE);
        if (argc == 0) {
            // can we skip this?
            AC_WARNING << "creating temp TextureAtlas";
            OWNERPTR newTextureAtlas;
            myNewObject = new JSTextureAtlas(newTextureAtlas, newTextureAtlas.get());
        } else if (argc >= 1) {
            if (JSVAL_IS_VOID(argv[0])) {
                JS_ReportError(cx, "JSTextureAtlas::Constructor: argument #1 must be filename (string) or a hash");
                return JS_FALSE;
            }
            std::string atlasFilename;
            OWNERPTR newTextureAtlas;

            // first argument
            if (argc == 1 && JSVAL_IS_STRING(argv[0]) && convertFrom(cx, argv[0], atlasFilename)) {
                // load xml & bitmap from files
                newTextureAtlas = OWNERPTR(new TextureAtlas(Path(atlasFilename, UTF8), JSApp::getPackageManager()));
            } else if (JSVAL_IS_OBJECT(argv[0])) {
                // generate a new texture atlas
                TextureAtlas::Subtextures subtextures = createSubtexturesFromJSObject(cx, JSVAL_TO_OBJECT(argv[0]));

                if (argc == 1) {
                    newTextureAtlas = OWNERPTR(new TextureAtlas(subtextures));
                } else {
                    bool myBorderFlag;
                    if (!convertFrom(cx, argv[1], myBorderFlag)) {
                        JS_ReportError(cx, "JSTextureAtlas::Constructor: argument #2 must be boolean (border-flag)");
                        return JS_FALSE;
                    }
                    if (argc == 2) {
                        newTextureAtlas = OWNERPTR(new TextureAtlas(subtextures, myBorderFlag));
                    } else {
                        bool myPowerOfTwoFlag;
                        if (!convertFrom(cx, argv[1], myPowerOfTwoFlag)) {
                            JS_ReportError(cx, "JSTextureAtlas::Constructor: argument #3 must be boolean (power-of-two-flag)");
                            return JS_FALSE;
                        }
                        newTextureAtlas = OWNERPTR(new TextureAtlas(subtextures, myBorderFlag, myPowerOfTwoFlag));
                    }
                }
            }
            myNewObject = new JSTextureAtlas(newTextureAtlas, newTextureAtlas.get());

        } else {
            JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 1,2 or 3: (filename | subtextures-hash [, border-flag [, power-of-two-flag]]), got %d",ClassName(), argc);
            return JS_FALSE;
        }

        if (myNewObject) {
            JS_SetPrivate(cx,obj,myNewObject);
            return JS_TRUE;
        }
        JS_ReportError(cx,"JSTextureAtlas::Constructor: bad parameters");
    } HANDLE_CPP_EXCEPTION;
    return JS_FALSE;
}

JSObject *
JSTextureAtlas::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
    DOC_CREATE(JSTextureAtlas);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<y60::TextureAtlas> & theTextureAtlas) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<y60::TextureAtlas>::Class()) {
                theTextureAtlas = JSClassTraits<y60::TextureAtlas>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

}


