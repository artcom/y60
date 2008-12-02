//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#if defined(_MSC_VER)
#    pragma warning(push,1)
#endif
#   include <paintlib/pldecoderfactory.h>
#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#include <OpenEXR/ImfVersion.h>

#include <asl/base/PlugInBase.h>
#include <y60/image/IPaintLibExtension.h>

#include "OpenEXRDecoder.h"

using namespace std;
using namespace asl;

namespace y60 {
	class OpenEXRDecoderPlugin :
		public asl::PlugInBase,
        public IPaintLibExtension,
        public ::PLDecoderFactory
	{
	public:
		OpenEXRDecoderPlugin(asl::DLHandle theDLHandle) : PlugInBase(theDLHandle) {
        }

        // IPaintLibExtension
		PLDecoderFactory * getDecoderFactory() const {
		    return const_cast<OpenEXRDecoderPlugin*>(this);
		}

		// PLDecoderFactory
        ::PLPicDecoder * CreateDecoder() const {
            return new ::OpenEXRDecoder;
        }

        bool CanDecode(PLBYTE * pData, int DataLen) const {
            return Imf::isImfMagic((char*)pData);
        }
	};
}

extern "C"
EXPORT PlugInBase * y60OpenEXR_instantiatePlugIn(DLHandle myDLHandle) {
	return new y60::OpenEXRDecoderPlugin(myDLHandle);
}
