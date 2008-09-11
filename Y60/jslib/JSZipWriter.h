//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSZIPWRITER_INCLUDED_
#define _Y60_ACXPSHELL_JSZIPWRITER_INCLUDED_

#include <y60/jsbase/JSWrapper.h>

#include <asl/zip/ZipWriter.h>

namespace jslib {

class JSZipWriter : public JSWrapper<asl::ZipWriter, asl::Ptr<asl::ZipWriter>, StaticAccessProtocol>
{
        JSZipWriter() {}
    public:
        typedef asl::ZipWriter NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "ZipWriter";
        }
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_END = -100,
        };

        static JSPropertySpec * Properties();
        static JSFunctionSpec * StaticFunctions();
        static JSPropertySpec * StaticProperties();

        virtual unsigned long length() const {
            return 1;
        }

        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSZipWriter(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSZipWriter & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSZipWriter &>(JSZipWriter::getJSWrapper(cx,obj));
        }
};

template <>
struct JSClassTraits<asl::ZipWriter> : public JSClassTraitsWrapper<asl::ZipWriter, JSZipWriter> {};

bool convertFrom(JSContext *cx, jsval theValue, JSZipWriter::NATIVE *& theZipWriter);

jsval as_jsval(JSContext *cx, JSZipWriter::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSZipWriter::OWNERPTR theOwner, JSZipWriter::NATIVE * theZipWriter);

}

#endif

