#ifndef Y60_APE_DYNAMIC_LOADER_INCLUDED
#define Y60_APE_DYNAMIC_LOADER_INCLUDED

//PlugIn- Base and Manager: The Good Parts

#include "y60_ape_settings.h"

#ifdef _WIN32
#   include "windows.h"
#else
#   include "dlfcn.h"
#endif

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace y60 { namespace ape {

class dynamic_loader : boost::noncopyable {
    public:
        typedef boost::shared_ptr<dynamic_loader> pointer;
#       ifdef _WIN32
            typedef HINSTANCE handle_type;
#       else
            typedef void *    handle_type;
#       endif

        dynamic_loader();
        dynamic_loader(const char * filename);
        ~dynamic_loader();
        void open(const char * filename);
        inline bool is_open() const { return lib_; }
        void close();

        template <typename F>
        F
        load_function(const char * name) {
            return reinterpret_cast<F>( do_load_function(name));
        }
        inline handle_type handle() { return lib_; }
    private:
        void * do_load_function(const char * name);
        handle_type lib_;
};

}} // end of namespace ape, y60

#endif // Y60_APE_DYNAMIC_LOADER_INCLUDED
