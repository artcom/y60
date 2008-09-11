//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: IDecoder.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: IDecoder.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//=============================================================================

#ifndef _IDECODER_INCLUDED
#define _IDECODER_INCLUDED

#include <asl/base/Ptr.h>
#include <asl/base/Stream.h>

#include <string>

namespace y60 {

class IDecoder {
    public:
        typedef asl::WeakPtr<IDecoder> WPtr;

        /**
        * Tests if the file/stream can be decoded by this decoder.
        * @param theUrl can be used to check for known file extensions.
        * @param theStream should also be provided if possible, to check for magic numbers, etc.
        * @return the Mimetype of the decoder, or an empty string, if it cannot decode the file.
        */
        virtual std::string canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream = asl::Ptr<asl::ReadableStreamHandle>(0) ) = 0;
};

typedef asl::Ptr<IDecoder> IDecoderPtr;

}
#endif
