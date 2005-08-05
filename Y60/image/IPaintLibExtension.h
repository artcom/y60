#ifndef _IPAINTLIB_EXTENSION_INCLUDED
#define _IPAINTLIB_EXTENSION_INCLUDED

#include <asl/Ptr.h>
#include <paintlib/plpicdec.h>
#include <paintlib/pldecoderfactory.h>

namespace y60 {

struct IPaintLibExtension {
	virtual PLDecoderFactory * getDecoderFactory() const = 0;
};

typedef asl::Ptr<IPaintLibExtension> IPaintLibExtensionPtr;

}
#endif
