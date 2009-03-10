#ifndef Y60_APE_APE_THING_INCLUDED
#define Y60_APE_APE_THING_INCLUDED

#include <boost/shared_ptr.hpp>
#include <boost/mpl/long.hpp>
#include <boost/mpl/push_back.hpp>


#include "monkey_utilities.h"

namespace y60 { namespace ape { namespace detail {

enum ape_type {
    ape_function,
    ape_class
};

class ape_thing {
    public:
        ape_thing(ape_type t, const char * name) : type_( t ),  name_( name ) {}
        virtual ~ape_thing() {}
        
        virtual void import(JSContext * cx, JSObject * ns, js_functions & free_functions) = 0;

        const char * get_name() const { return name_; }
        ape_type get_type() const { return type_; }
    private:
        ape_type     type_;
        const char * name_;
};

typedef boost::shared_ptr<ape_thing> ape_thing_ptr;

// does not really belong here ...
template <long LineNumber>
struct line_number_tag : boost::mpl::long_<LineNumber>{};

template <typename Id, typename Appendix>
struct append_to_id {
    typedef typename boost::mpl::push_back< Id, Appendix>::type type;
};

typedef std::vector<detail::ape_thing_ptr> ape_list;

}}} // end of namespace detail, ape, y60

#endif // Y60_APE_APE_THING_INCLUDED
