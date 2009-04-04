#ifndef Y60_APE_APE_THING_INCLUDED
#define Y60_APE_APE_THING_INCLUDED

#include <y60/components/yape/y60_ape_settings.h>

#include <iostream>
#include <boost/shared_ptr.hpp>

#include "monkey_utilities.h"

namespace y60 { namespace ape { namespace detail {

enum ape_type {
    ape_module,
    ape_function,
    ape_property,
    ape_class
};

inline
std::ostream &
operator<<(std::ostream & os, ape_type type) {
    switch (type) {
        case ape_module:   os << "module";   break;
        case ape_function: os << "function"; break;
        case ape_property: os << "property"; break;
        case ape_class:    os << "class";    break;
    }
    return os;
}

class ape_thing;

typedef boost::shared_ptr<ape_thing> ape_thing_ptr;

class ape_thing {
    protected:
        ape_thing(ape_type t, const char * name, uint8 prop_flags = default_flags) :
            type_( t ),
            name_( name ),
            property_flags_(prop_flags)
        {}
    public:
        virtual ~ape_thing() {}
        
        virtual void import(JSContext * cx, JSObject * ns, monkey_data & ape_ctx) = 0;

        inline void add( ape_thing_ptr child ) {
            children_.push_back( child ); 
        }

        inline const char * get_name() const { return name_; }
        inline ape_type get_type() const { return type_; }

        inline uint8 property_flags() const { return property_flags_; }
        inline void set_property_flags(uint8 flags) { property_flags_ = flags; }
        
        void enumerate(bool flag) {
            if ( flag ) {
                property_flags_ |= JSPROP_ENUMERATE;
            } else {
                property_flags_ &= ~JSPROP_ENUMERATE;
            }
        }
    protected:
        void import_children(JSContext * cx, JSObject * ns, monkey_data & md) {
            for (unsigned i = 0; i < children_.size(); ++i) {
                children_[i]->import(cx, ns, md);
            }
        }

    private:
        ape_type                   type_;
        const char *               name_;
        std::vector<ape_thing_ptr> children_;
        uint8                      property_flags_;

        static const uint8 default_flags = JSPROP_ENUMERATE | JSPROP_PERMANENT |
            JSPROP_SHARED;
};

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_APE_THING_INCLUDED
