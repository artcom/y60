//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSDicomImage.h,v $
//   $Author: david $
//   $Revision: 1.4 $
//   $Date: 2005/02/25 16:41:31 $
//
//
//=============================================================================

#ifndef _ac_y60_components_dicomimage_h_
#define _ac_y60_components_dicomimage_h_

#include <y60/JSWrapper.h>
#ifndef WIN32
#include <dicom/osconfig.h>
#endif
#include <dcmimage.h>

class JSDicomImage : public jslib::JSWrapper<DicomImage, asl::Ptr<DicomImage> , jslib::StaticAccessProtocol> {
        JSDicomImage() {}
    public:
        typedef ::DicomImage NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;

        JSDicomImage(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        ~JSDicomImage() {};
        
        static const char * ClassName() {
            return "DicomImage";
        }
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_window = -100,
            PROP_status
        };
        static JSPropertySpec * Properties();

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

        static jslib::JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSDicomImage & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSDicomImage &>(JSDicomImage::getJSWrapper(cx,obj));
        }
};

namespace jslib {
template <>
struct JSClassTraits<JSDicomImage::NATIVE> 
    : public JSClassTraitsWrapper<JSDicomImage::NATIVE , JSDicomImage> {};

} // namespace jslib

// bool convertFrom(JSContext *cx, jsval theValue, JSDicomImage::NATIVE & theSerial);

jsval as_jsval(JSContext *cx, JSDicomImage::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSDicomImage::OWNERPTR theOwner, JSDicomImage::NATIVE * theSerial);

#endif


