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
*/

#ifndef _Y60_PANGO_JSLAYOUT_INCLUDED_
#define _Y60_PANGO_JSLAYOUT_INCLUDED_

#include <pango/pango-layout.h>

#include <asl/dom/Nodes.h>

#include <y60/jsbase/JSWrapper.h>

#include <y60/components/graphics/Cairo/JSCairoContext.h>

namespace jslib {

    namespace pango {

        //use layout with CairoContext
        class PangoCairo {
            public:
                PangoCairo(PangoLayout * theLayout, cairo_t * theCairoContext) :
                    _myPangoLayout(theLayout),
                    _myCairoContext(theCairoContext) { 
                    };
                ~PangoCairo() {
                    if (_myPangoLayout) {
                        g_object_unref(_myPangoLayout);
                    }
                    cairo_destroy(_myCairoContext);
                }
                PangoLayout* getLayout() {
                    return _myPangoLayout;
                }
                cairo_t* getCairoContext() {
                    return _myCairoContext;
                }
            private:
                PangoLayout* _myPangoLayout;
                cairo_t* _myCairoContext;
        };

        // a quick wrapper to ensure the correct *_free is called on destruction
        class LayoutWrapper {
            public:
                LayoutWrapper(PangoCairo * theNative) :
                    _myNative(theNative) {};
                ~LayoutWrapper() {
                    if (_myNative) {
                        delete _myNative;
                    }
                }
                PangoCairo * get() const { return _myNative; };
            private:
                // not copy-safe: hide copy ctor & assignment op
                LayoutWrapper(const LayoutWrapper &);
                LayoutWrapper & operator=(const LayoutWrapper &);
                PangoCairo * _myNative;
        };
        class JSLayout : public JSWrapper<LayoutWrapper, asl::Ptr< LayoutWrapper >, StaticAccessProtocol> {
            JSLayout();  // hide default constructor
            public:

            virtual ~JSLayout() {
            }

            typedef LayoutWrapper NATIVE;
            typedef asl::Ptr< LayoutWrapper > OWNERPTR;

            typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

            static const char * ClassName() {
                return "Layout";
            }

            static JSFunctionSpec * Functions();

            enum PropertyNumbers {
                PROP_font_description = -100,
                PROP_text,
                PROP_context,
                PROP_END
            };
            static JSPropertySpec * Properties();

            virtual unsigned long length() const {
                return 1;
            }

            static JSBool getPropertySwitch(NATIVE & theNative, unsigned long theID,
                    ::JSContext *cx, JSObject *obj, jsval id, jsval *vp);
            virtual JSBool getPropertySwitch(unsigned long theID, ::JSContext *cx, JSObject *obj, jsval id, jsval *vp);
            static JSBool setPropertySwitch(NATIVE & theNative, unsigned long theID, ::JSContext *cx,
                    JSObject *obj, jsval id, jsval *vp);
            virtual JSBool setPropertySwitch(unsigned long theID, ::JSContext *cx, JSObject *obj, jsval id, jsval *vp);

            static JSBool
                Constructor(::JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

            static
                JSObject * Construct(::JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
                    return Base::Construct(cx, theOwner, theNative);
                }

            JSLayout(OWNERPTR theOwner, NATIVE * theNative)
                : Base(theOwner, theNative)
            { }

            static JSConstIntPropertySpec * ConstIntProperties();
            static JSObject * initClass(::JSContext *cx, JSObject *theGlobalObject);
            static void addClassProperties(::JSContext * cx, JSObject * theClassProto);

            static JSLayout & getObject(::JSContext *cx, JSObject * obj) {
                return dynamic_cast<JSLayout &>(JSLayout::getJSWrapper(cx,obj));
            }

        };
    };

    template <>
    struct JSClassTraits<pango::JSLayout::NATIVE>
        : public JSClassTraitsWrapper<pango::JSLayout::NATIVE, pango::JSLayout> {};

    jsval as_jsval(JSContext *cx, pango::JSLayout::OWNERPTR & theOwner);

    bool convertFrom(JSContext *cx, jsval theValue, pango::JSLayout::OWNERPTR & theOwner);
}

#endif
