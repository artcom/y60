
#ifndef _Y60_CAIRO_JSCAIROCONTEXT_INCLUDED_
#define _Y60_CAIRO_JSCAIROCONTEXT_INCLUDED_

#include <y60/JSWrapper.h>

#include <cairomm/cairomm.h>

namespace jslib {

    class JSCairoContext : public JSWrapper<Cairo::Context, asl::Ptr<Cairo::Context>, StaticAccessProtocol> {
        JSCairoContext();  // hide default constructor
    public:

        virtual ~JSCairoContext() {
        }

        typedef Cairo::Context NATIVE;
        typedef asl::Ptr<Cairo::Context> OWNERPTR;

        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "CairoContext";
        }

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_END
        };
        static JSPropertySpec * Properties();

        virtual unsigned long length() const {
            return 1;
        }

        static JSBool getPropertySwitch(NATIVE & theNative, unsigned long theID,
                                        JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        static JSBool setPropertySwitch(NATIVE & theNative, unsigned long theID, JSContext *cx,
                                        JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
            Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSCairoContext(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
            { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSCairoContext & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSCairoContext &>(JSCairoContext::getJSWrapper(cx,obj));
        }

    private:
    };

    template <>
    struct JSClassTraits<JSCairoContext::NATIVE>
        : public JSClassTraitsWrapper<JSCairoContext::NATIVE, JSCairoContext> {};

    jsval as_jsval(JSContext *cx, JSCairoContext::OWNERPTR theOwner, JSCairoContext::NATIVE * theButton);
    
}

#endif /* !_Y60_CAIRO_JSCAIROCONTEXT_INCLUDED_ */
