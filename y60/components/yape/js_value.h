#ifndef Y60_APE_JS_VALUE_INCLUDED
#define Y60_APE_JS_VALUE_INCLUDED

#include "y60_ape_settings.h"

#ifdef check
#   undef check
#endif

namespace y60 { namespace ape {

class value {
    public:
        explicit value(JSContext * cx) : v_(JSVAL_VOID), cx_(cx) {};
        value(JSContext * cx, jsval v) : v_(v), cx_(cx) {};

        std::string toString() {
            JSString * str = JS_ValueToString(cx_, v_);
            if ( ! str ) {
                throw monkey_error("string conversion failed",
                        PLUS_FILE_LINE);
            }
            return std::string( JS_GetStringBytes( str ));
        }

        inline bool is_void() const { return v_ == JSVAL_VOID; }
        inline bool is_null() const { return JSVAL_IS_NULL( v_ ); }

        jsval * val_ptr() { return &v_; }
        jsval & val() { return v_; }
        jsval const& val() const { return v_; }
        JSContext * ctx() const { return cx_; }
    private:
        jsval v_;
        JSContext * cx_;
};

template <typename T> inline bool js_type_check( jsval v );

template <>
inline
bool
js_type_check<int>( jsval v ) {
    return JSVAL_IS_NUMBER( v );
}

template <>
inline
bool
js_type_check<bool>( jsval v ) {
    return JSVAL_IS_BOOLEAN( v );
}

template <typename T> inline T jsval_to(JSContext * cx, jsval const& v );

template <>
inline
int
jsval_to<int>(JSContext * cx, jsval const& v ) {
    int r;
    if ( ! JS_ValueToInt32(cx, v, & r) ) {
        throw ape_error("conversion failed", PLUS_FILE_LINE);
    }
    return r;
}

template <>
inline
bool
jsval_to<bool>(JSContext * cx, jsval const& v ) {
    JSBool r;
    if ( ! JS_ValueToBoolean(cx, v, & r ) ) {
        throw ape_error("conversion failed", PLUS_FILE_LINE);
    }
    if (JS_TRUE == r) {
        return true;
    } else {
        return false;
    }
}

template <typename T>
class extract {
    public:
        extract(value const& v) : v_(v.val()), cx_(v.ctx()) {}
        bool check() {
            return js_type_check<T>( v_ );
        }
        T operator()() {
            return jsval_to<T>(cx_, v_);
        }
    private:
        jsval       v_;
        JSContext * cx_;

};

template <typename T> struct to_js_converter;

template <>
struct to_js_converter<std::string> {
    to_js_converter(JSContext * cx) : cx_(cx) {}

    jsval operator()(std::string const& s) {
        JSString * str = JS_NewStringCopyZ(cx_, s.c_str());
        if ( ! str ) {
            throw monkey_error("string conversion failed",
                    PLUS_FILE_LINE);
        }
        return STRING_TO_JSVAL( str );
    }
    JSContext * cx_;
};

template <typename T>
struct to_js_converter<std::vector<T> > {
    to_js_converter(JSContext * cx) : cx_(cx) {}

    jsval operator()(std::vector<T> const& v) {
        std::vector<jsval> vec(v.size());

        std::transform(v.begin(), v.end(),
                vec.begin(), to_js_converter<T>(cx_) ); 

        JSObject * array =
            JS_NewArrayObject(cx_, vec.size(), &*vec.begin());
        if ( ! array) {
            throw monkey_error("failed to create array",
                    PLUS_FILE_LINE);
        }
        return OBJECT_TO_JSVAL( array );
    }
    JSContext * cx_;
};

class property_handle {
    public:
        property_handle(JSContext * cx, JSObject * obj, const char * name) :
            cx_(cx), obj_(obj), name_(name) {}

        jsval
        operator=(jsval v) {
            if ( ! JS_SetProperty(cx_, obj_, name_, & v)) {
                throw monkey_error("failed to set property",
                        PLUS_FILE_LINE);
            }
            return v;
        }
        operator value() const {
            jsval v;
            if ( ! JS_GetProperty(cx_, obj_, name_, & v)) {
                throw monkey_error("failed to get property",
                        PLUS_FILE_LINE);
            }
            return value(cx_,v);
        }
        JSContext * ctx() const { return cx_; }
    private:

    JSContext * cx_;
    JSObject  * obj_;
    const char * name_;
};

class Object {
    public:
        Object(JSContext * cx, jsval value) : cx_(cx), obj_(0) {
            assign( value );
        }
        Object(JSContext * cx, JSObject * obj) : cx_(cx), obj_(obj) {}
        Object(value const& v) : cx_(v.ctx()), obj_(0) {
            assign( v.val() );
        }

        property_handle
        operator[](const char * prop) {
            return property_handle(cx_, obj_, prop);
        }
        Object & operator=(value const& v) {
            cx_ = v.ctx();
            assign(v.val());
            return *this;
        }

    protected:
        inline
        void
        assign(jsval val) {
            if ( ! JSVAL_IS_OBJECT( val ) ) {
                throw ape_error("value is not an object", PLUS_FILE_LINE);
            }
            obj_ = JSVAL_TO_OBJECT( val );
        }
        inline
        void
        assign(value v) {
            cx_ = v.ctx();
            if ( ! JSVAL_IS_OBJECT( v.val() ) ) {
                throw ape_error("value is not an object", PLUS_FILE_LINE);
            }
            obj_ = JSVAL_TO_OBJECT( v.val() );
        }

        JSContext * cx_;
        JSObject  * obj_;
};

class Array : public Object {
    public:
        Array(JSContext * cx, jsval value) : Object(cx, value) {
            ensure_is_array();
        }
        Array(value const & v) : Object(v) {
            ensure_is_array();
        }
        Array & operator=(value const& v) {
            assign(v);
            ensure_is_array();
            return *this;
        }
        Array & operator=(property_handle const& p) {
            assign(p);
            ensure_is_array();
            return *this;
        }
        jsuint length() {
            jsuint result;
            if ( ! JS_GetArrayLength(cx_, obj_, & result)) {
                throw ape_error("failed to get array length", PLUS_FILE_LINE);
            }
            return result;
        }
        value operator[](jsuint idx) {
            jsval element;
            if ( ! JS_GetElement(cx_, obj_, idx, & element) ) {
                throw ape_error("failed to get array element", PLUS_FILE_LINE);
            }
            return value(cx_, element);
        }
    private:
        inline void ensure_is_array() {
            if ( ! JS_IsArrayObject(cx_, obj_)) {
                throw ape_error("object is not an array", PLUS_FILE_LINE);
            }
        }
};

}} // end of namespace ape, y60

#endif // Y60_APE_JS_VALUE_INCLUDED
