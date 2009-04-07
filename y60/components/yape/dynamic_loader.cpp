#include "dynamic_loader.h"

#include <string>

namespace y60 { namespace ape {

#ifdef _WIN32

inline
HINSTANCE
open_impl(const char * filename) {
    return LoadLibrary(filename);
}

inline
void
close_impl(handle_type lib) {
    FreeLibrary( lib );
}

inline
void *
load_function_impl(handle_type lib, const char * name) {
    return GetProcAddress(lib, name);
}

inline
std::string
last_error_impl() {
    DWORD error_code = GetLastError();
    LPVOID message_buffer;
    if (!FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                error_code,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) & message_buffer,
                0,
                NULL ))
    {
        std::ostringstream os;
        os << error_code;
        return os.str();
    } else {
        std::string error_message((LPCTSTR)message_buffer);
        LocalFree( lpMsgBuf );
        return error_message;
    }
}

#else // _WIN32

inline
void *
open_impl(const char * filename) {
    return dlopen(filename, RTLD_LAZY | RTLD_GLOBAL);
}

inline
void
close_impl(void * lib) {
    dlclose( lib );
}

inline
void *
load_function_impl(void * lib, const char * name) {
    return dlsym(lib, name);
}

inline
std::string
last_error_impl() {
    return std::string( dlerror() );
}
#endif

dynamic_loader::dynamic_loader() : lib_( handle_type() ) {}

dynamic_loader::dynamic_loader(const char * filename) :
    lib_( handle_type() )
{
    open(filename);
}

dynamic_loader::~dynamic_loader() {
    if (lib_) {
        close();
    }
}

void
dynamic_loader::open(const char * filename) {
    lib_ = open_impl(filename);
    if ( ! lib_ ) {
        throw 2; // open failed
    }
}
void
dynamic_loader::close() {
    close_impl( lib_ ); 
    lib_ = handle_type();
}

void *
dynamic_loader::do_load_function(const char * name) {
    if ( ! lib_ ) {
        throw 1; // not open
    }
    return load_function_impl(lib_, name);
}

}} // end of namespace ape, y60

