#ifndef Y60_APE_MODULE_INCLUDED
#define Y60_APE_MODULE_INCLUDED

#include "y60_ape_settings.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

//#include <boost/mpl/vector/vector10.hpp>

#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IJSModuleLoader.h>

#include "detail/ape_exceptions.h"
#include "detail/monkey_utilities.h"
#include "detail/scope.h"
#include "detail/context_decorator.h"

namespace y60 { namespace ape {

#if 0
template <typename UserModule>
class binding {
    private:
        template <typename LocalUniqueId>
        struct unique_id : boost::mpl::vector2<LocalUniqueId, UserModule> {};

    protected:
        template <typename LocalUniqueId>
        detail::context_decorator< typename unique_id<LocalUniqueId>::type >
        namespace_scope() {
            typedef typename unique_id<LocalUniqueId>::type id;
            return detail::context_decorator<id>();
        }

};
#endif 

template <typename Binding>
class module : public detail::ape_thing {
    private:
        template <typename LocalUniqueId>
        struct unique_id : boost::mpl::vector2<LocalUniqueId, Binding> {};
    public:
        module(const char * name) : detail::ape_thing(detail::ape_module, name) {}
        void import(JSContext * cx, JSObject * ns) {
            import(cx, ns, ns_scope);
        }
        void import(JSContext * cx, JSObject * ns, detail::monkey_data & /*ape_ctx*/) {
            static bool imported = false;
            if ( imported ) {
                std::ostringstream os;
                os << "module '" << get_name() << "' is already imported.";
                throw script_error(os.str(), PLUS_FILE_LINE);
            }

            static_cast<Binding&>(*this).bind();

            import_children(cx, ns, ns_scope);
            if ( ! ns_scope.static_functions.empty()) {
                if ( ! JS_DefineFunctions(cx, ns, ns_scope.static_functions.ptr())) {
                    throw monkey_error("failed to define functions", PLUS_FILE_LINE);
                }
            }
            imported = true;
        }
    protected:
        template <typename LocalUniqueId>
        detail::context_decorator< typename unique_id<LocalUniqueId>::type >
        namespace_scope() {
            typedef typename unique_id<LocalUniqueId>::type id;
            return detail::context_decorator<id>();
        }
    private:
        static detail::monkey_data ns_scope;
};

template <typename Binding>
detail::monkey_data module<Binding>::ns_scope;

template <typename ModuleBinding>
class module_importer {
    public:
        typedef module<ModuleBinding> module_type;
        typedef boost::shared_ptr<module_type> module_ptr_type;

        module_importer(const char * name) : module_name_( name ) {}

        void import(JSContext * cx, JSObject * ns) {
            init_module(cx, ns);
        }
    private:
        void
        init_module(JSContext * cx, JSObject * ns) {
            APE_LOG(log::import,log::inf,log::dev) 
                << "importing module " << module_name_;
            module_ptr_type mod(new module<ModuleBinding>(module_name_));
            detail::scope module_scope(mod);

            mod->import(cx, ns);
        }

        const char * module_name_;
};

template <typename ModuleBinding>
class module_adapter : public module_importer<ModuleBinding>,
                      public asl::PlugInBase,
                      public jslib::IJSModuleLoader
{
    public:
        module_adapter(const char * name) :
            module_importer<ModuleBinding>( name ),
            asl::PlugInBase( NULL ) {}

        // implement IJSModuleLoader
        void initClasses(JSContext * cx, JSObject * ns) {
            this->import(cx, ns);
        }
};

typedef jslib::IJSModuleLoader * (*init_function)(asl::DLHandle theDLHandle);

}} // end of namespace ape, y60

#define Y60_APE_NS_SCOPE() \
        namespace_scope<y60::ape::detail::line_number<__LINE__> >()

#if defined(WIN32)
#   define Y60_APE_MODULE_EXPORT __declspec( dllexport )
#else
#   define Y60_APE_MODULE_EXPORT 
#endif

#define Y60_APE_BNAME( name ) \
    name ## _binding

#define Y60_APE_MODULE_TYPE( name ) \
    Y60_APE_BNAME( name )

#define Y60_APE_IMPORT_MODULE( cx, obj, name )                      \
{                                                                   \
    y60::ape::module_importer<Y60_APE_BNAME( name )> m( #name );    \
    m.import(cx, obj);                                              \
}


#define Y60_APE_MODULE_DECL( name )                                             \
class Y60_APE_BNAME(name) : public y60::ape::module< Y60_APE_BNAME(name) > {    \
    public:                                                                     \
        friend class y60::ape::module< Y60_APE_BNAME(name) >;                   \
        void bind();                                                            \
    protected:                                                                  \
        Y60_APE_BNAME(name)() :                                                 \
                y60::ape::module< Y60_APE_BNAME(name) >(#name) {}               \
};

// TODO: deprecate this
#define Y60_APE_MODULE_ASL_PLUGIN_ADAPTER(name)                                 \
extern "C" Y60_APE_MODULE_EXPORT                                                \
asl::PlugInBase * name ## _instantiatePlugIn(asl::DLHandle theDLHandle) {       \
    using y60::ape::module_adapter;                                             \
    return new module_adapter< Y60_APE_BNAME(name) >(#name);                    \
}

#define Y60_APE_MODULE_IMPL( name )                                             \
extern "C" Y60_APE_MODULE_EXPORT                                                \
jslib::IJSModuleLoader * name ## _init_ape_module(asl::DLHandle theDLHandle) {  \
    using y60::ape::module_adapter;                                             \
    return new module_adapter< Y60_APE_BNAME(name) >(#name);                    \
}                                                                               \
                                                                                \
Y60_APE_MODULE_ASL_PLUGIN_ADAPTER(name)                                         \
                                                                                \
void Y60_APE_BNAME( name ) ::bind()

#define Y60_APE_MODULE( name )  \
Y60_APE_MODULE_DECL( name )     \
Y60_APE_MODULE_IMPL( name )

#endif // Y60_APE_MODULE_INCLUDED
