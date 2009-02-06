#ifndef ASL_BASE_REVISION_INFO_INCLUDED
#define ASL_BASE_REVISION_INFO_INCLUDED

#include <string>
#include "Singleton.h"

namespace asl {

class RevisionInfo{
    public:
        RevisionInfo(const char * theName, const char * theURL,
                const char * theRevision, const char * theTargetType) :
                _myName( theName ), _myURL( theURL ), _myRevision( theRevision ),
                _myTargetType( theTargetType )
        {}
        std::string const& name() const { return _myName; }
        std::string const& url() const { return _myURL; }
        std::string const& revision() const { return _myRevision; }
        std::string const& type() const { return _myTargetType; }

        std::ostream & print( std::ostream & os ) const {
            os << _myName << " " << _myRevision << " [" << _myURL << "] [" << _myTargetType << "]";
            return os;
        }
    private:
        std::string _myName;
        std::string _myURL;
        std::string _myRevision;
        std::string _myTargetType;
};

inline
std::ostream &
operator<<(std::ostream & os, RevisionInfo const& info) {
    return info.print( os );
}

class RevisionInfoPool : public Singleton<RevisionInfoPool>,
                         public std::map<std::string, RevisionInfo>
{
    private:
        typedef std::map<std::string, RevisionInfo> RevisionMap;
    public:
        std::ostream & print( std::ostream & os ) const {
            os << "Revision Info:" << std::endl;
            for(RevisionMap::const_iterator it = begin(); it != end(); ++it) {
                os << "\t" << it->second << std::endl;
            }
            return os;
        }
};

inline
std::ostream &
operator<<(std::ostream & os, RevisionInfoPool const& pool) {
    return pool.print( os );
}

class RevisionInfoInitializer {
    public:
        RevisionInfoInitializer(const char * theName, const char * theURL,
                const char * theRevision, const char * theTargetType)
        {
            RevisionInfoPool::get().insert( std::make_pair( theName,
                        RevisionInfo( theName, theURL, theRevision, theTargetType)));
        }
};

} // end of namespace asl

#define AC_SVN_REVISION(name, url, revision, type)                          \
    namespace {                                                             \
        asl::RevisionInfoInitializer revision_info =                        \
                asl::RevisionInfoInitializer( name, url, revision, type);   \
    }

#endif // ASL_BASE_REVISION_INFO_INCLUDED
