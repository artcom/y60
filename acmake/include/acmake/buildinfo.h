/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: Utilities to provide build related information at runtime.
//              
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef ASL_BASE_BUILD_INFORMATION_INCLUDED
#define ASL_BASE_BUILD_INFORMATION_INCLUDED

#include <map>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace acmake {

template <unsigned Major, unsigned Minor, unsigned Patchlevel>
class gcc_compiler {
    public:
        enum {
            version = Major * 10000 + Minor * 100 + Patchlevel,
            value   = version
        };
        static std::string const& name() {
            static std::string compiler_name("GCC");
            return compiler_name;
        }
        static std::string const& version_str() {
            static std::string v = init_version_string();
            return v;
        }
    private:
        static std::string init_version_string() {
            std::ostringstream os;
            os << major << '.' << minor << '.' << patchlevel;
            return os.str();
        }
        enum {
            major = Major,
            minor = Minor,
            patchlevel = Patchlevel
        };
};

// TODO ask Hendrik about the names and version formatting 
template <unsigned MSCVersion>
class msvc_compiler {
    public:
        enum {
            version     = MSCVersion,
            value       = version
        };
        static std::string const& name() {
            static std::string compiler_name("MSVC cl");
            return compiler_name;
        }
        static std::string const& version_str() {
            static std::string v = init_version_string();
            return v;
        }
    private:
        enum { major_version    = MSCVersion / 100,
            point_release = (MSCVersion - major_version*100),
        };
        static std::string init_version_string() {
            std::ostringstream os;
            os << major_version << '.'
               << std::setfill('0') << std::setw(2) << point_release;
            return os.str();
        }
};

struct unknown_compiler {
    enum {
        version = 0,
        value = version
    };
    static std::string const& name() {
        static std::string n("unknown");
        return n;
    }
    static std::string const& version_str() { static std::string n; return n; }
};

#if defined( __GNUC__ )
    typedef gcc_compiler<__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__> detected_compiler;
#elif defined( _MSC_VER )
    typedef msvc_compiler<_MSC_VER> detected_compiler;
#else
    typedef unknown_compiler detected_compiler;
#endif


class svn_revision {
    public:
        svn_revision(const char * url, const char * revision) :
            url_(url), revision_(revision) {}
        const char * history_id() const { return revision_.c_str(); }
        const char * repository_id() const { return url_.c_str(); }
        const char * name() const { return "SVN"; }
    private:
        std::string url_;
        std::string revision_;
};

struct no_scm_data {
    const char * history_id() const { return ""; }
    const char * repository_id() const { return ""; }
    const char * name() const { return ""; }
};

inline
void
space( std::ostream & os, size_t w) {
    for(size_t i = 0; i < w; ++i) { os.put(' '); }
}

class build_target_info {
    private:
        struct max_key_len {
            max_key_len() : result(0) {}
            void operator()(std::pair<std::string,std::string> const& p) {
                result = std::max(p.first.size(), result);
            }
            size_t result;
        };
    public:
        enum type {
            LIBRARY,
            EXECUTABLE,
            PLUGIN
        };
        template <typename SCMInfo>
        build_target_info(const char * name, const type type,
                const char * build_date, const char * build_time,
                SCMInfo const& scm_info) :
            name_( name ), target_type_( type ),
            build_date_( build_date ), build_time_( build_time ),
            scm_name_( scm_info.name() ), scm_history_id_( scm_info.history_id() ),
            scm_repository_id_( scm_info.repository_id())
        {
            
        }
        std::string const& name() const { return name_; }
        type const& target_type() const { return target_type_; }

        std::string const& scm_name() const { return scm_name_; }
        std::string const& history_id() const { return scm_history_id_; }
        std::string const& repository_id() const { return scm_repository_id_; }

        std::string type_str() const {
            switch (target_type_) {
                case LIBRARY:    return "library";
                case EXECUTABLE: return "executable";
                case PLUGIN:     return "plugin";
            }
        }
        std::string const& build_date() const { return build_date_; }
        std::string const& build_time() const { return build_time_; }

        std::ostream & print( std::ostream & os ) const {
            os << "=== Build Information =========================" << std::endl
               << "Name    : " << name_      << std::endl
               << "Type    : " << type_str()   << std::endl
               << "Revision: " << scm_history_id_ << std::endl
               << "URL     : " << scm_repository_id_ << std::endl
               << "Build on: " << build_date_ << " at "
                               << build_time_ << std::endl
               << "Compiler: " << detected_compiler::name() << ' '
                               << detected_compiler::version_str() << std::endl;
            return os;
        }
    private:
        std::string name_;
        type        target_type_;
        std::string build_date_;
        std::string build_time_;
        std::string scm_name_;
        std::string scm_history_id_;
        std::string scm_repository_id_;
};

inline
std::ostream &
operator<<(std::ostream & os, build_target_info const& info) {
    return info.print( os );
}

class build_information : //public Singleton<build_information>,
                         public std::map<std::string, build_target_info>
{
    private:
        typedef std::map<std::string, build_target_info> RevisionMap;

        struct column_width {
            column_width() : name(0), url(0), revision(0) {}
            void operator()(std::pair<std::string,build_target_info> const& c) {
                name     = std::max( name, c.second.name().size() );
                url      = std::max( url , c.second.repository_id().size() );
                revision = std::max( revision, c.second.history_id().size() );
                type     = std::max( type, c.second.type_str().size() );
            }
            size_t name;
            size_t url;
            size_t revision;
            size_t type;
        };
    public:
        static build_information & get() {
            static build_information pool;
            return pool;
        }
        std::ostream & print( std::ostream & os ) const {
            if ( empty() ) {
                os << "Revision info disabled at compile time." << std::endl;
            } else {
                os << "=== Revision Information ======================" << std::endl;
                column_width cw = std::for_each(begin(),end(), column_width());
                const size_t spacing = 3;
                for(RevisionMap::const_iterator it = begin(); it != end(); ++it) {
                    space(os,4);
                    os << it->second.name();
                    space(os, cw.name + spacing - it->second.name().size());
                    os << it->second.history_id();
                    space(os, cw.revision + spacing - it->second.history_id().size());
                    os << it->second.repository_id();
                    os << std::endl;
                }
            }
            return os;
        }

        build_target_info const& executable() const {
            static build_target_info exe = find_executable();
            return exe;
        }
    private:
        build_target_info find_executable() const {
            for(RevisionMap::const_iterator it = begin(); it != end(); ++it) {
                if (it->second.target_type() == build_target_info::EXECUTABLE) {
                    return it->second;
                }
            }
            // Hmmm
            return build_target_info("unknown", build_target_info::EXECUTABLE,
                    __DATE__, __TIME__, no_scm_data());
        }
        
};

inline
std::ostream &
operator<<(std::ostream & os, build_information const& pool) {
    return pool.print( os );
}

class target_info_initializer {
    public:
        template <typename SCMInfo>
        target_info_initializer(const char * name,
                build_target_info::type type,
                const char * date, const char * time,
                SCMInfo const& scm_info)
        {
            build_information::get().insert( std::make_pair( name,
                        build_target_info( name, type, date, time,
                        scm_info)));
        }
};

} // end of namespace acmake

#define ACMAKE_SVN_REVISION(url, revision) \
    acmake::svn_revision(url, revision)

#define ACMAKE_NO_SCM_DATA() \
    acmake::no_scm_data()

#define ACMAKE_BUILDINFO(name, target_type, scm_info)                      \
        namespace {                                                        \
            acmake::target_info_initializer revision_info =                \
                    acmake::target_info_initializer( name,                 \
                            acmake::build_target_info:: target_type,       \
                            __DATE__, __TIME__,                            \
                            scm_info);                                     \
        }

#endif // ASL_BASE_BUILD_INFORMATION_INCLUDED
