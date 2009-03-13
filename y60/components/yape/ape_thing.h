#ifndef Y60_APE_APE_THING_INCLUDED
#define Y60_APE_APE_THING_INCLUDED

#include <boost/shared_ptr.hpp>
#include <boost/mpl/long.hpp>
#include <boost/mpl/push_back.hpp>

#include "monkey_utilities.h"

namespace y60 { namespace ape { namespace detail {

enum ape_type {
    ape_module,
    ape_function,
    ape_property,
    ape_class
};

class ape_thing;

typedef boost::shared_ptr<ape_thing> ape_thing_ptr;
typedef std::vector<ape_thing_ptr> ape_children;

class ape_thing {
        static const uint8 default_flags = JSPROP_ENUMERATE;
    public:
        ape_thing(ape_type t, const char * name) :
            type_( t ),
            name_( name ),
            property_flags_(default_flags),
            function_flags_(default_flags),
            class_flags_(default_flags)
        {}
        virtual ~ape_thing() {}
        
        virtual void import(JSContext * cx, JSObject * ns, monkey_data & ape_ctx) = 0;

        void add( ape_thing_ptr child ) { children_.push_back( child ); }
        const char * get_name() const { return name_; }
        ape_type get_type() const { return type_; }

        uint8 function_flags() const { return function_flags_; }
        uint8 property_flags() const { return property_flags_; }
        uint8 class_flags() const { return class_flags_; }
        
        inline void set_function_flag(uint8 flag, bool v) {
            set_flag( function_flags_, flag, v);
        }
        inline void set_property_flag(uint8 flag, bool v) {
            set_flag( property_flags_, flag, v);
        }
        inline void set_class_flag(uint8 flag, bool v) {
            set_flag( class_flags_, flag, v);
        }

        inline void set_flag(uint8 & flags, uint8 flag, bool v) {
            if (v) {
                flags |= flag;
            } else {
                flags &= ~flag;
            }
        }
    protected:
        ape_children const& children() const { return children_; }

        void import_children(JSContext * cx, JSObject * ns, monkey_data & md) {
            detail::ape_children const& kids = children();
            for (unsigned i = 0; i < kids.size(); ++i) {
                kids[i]->import(cx, ns, md);
            }
        }

    private:
        ape_type     type_;
        const char * name_;
        ape_children children_;
        uint8        property_flags_;
        uint8        function_flags_;
        uint8        class_flags_;
};

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_APE_THING_INCLUDED
