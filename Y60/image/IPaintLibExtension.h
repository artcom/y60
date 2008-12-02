#ifndef _IPAINTLIB_EXTENSION_INCLUDED
#define _IPAINTLIB_EXTENSION_INCLUDED

#include <asl/base/Ptr.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpicdec.h>
#include <paintlib/pldecoderfactory.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

namespace y60 {

struct IPaintLibExtension {
	virtual PLDecoderFactory * getDecoderFactory() const = 0;
};

typedef asl::Ptr<IPaintLibExtension> IPaintLibExtensionPtr;

}
#endif
