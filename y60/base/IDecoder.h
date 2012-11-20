/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#ifndef _IDECODER_INCLUDED
#define _IDECODER_INCLUDED

#include "y60_base_settings.h"

#include <asl/base/Ptr.h>
#include <asl/base/Stream.h>

#include <string>
#include <set>

namespace y60 {

class IDecoder;

typedef asl::Ptr<IDecoder> IDecoderPtr;

class Y60_BASE_DECL IDecoder {
    public:
        typedef asl::WeakPtr<IDecoder> WPtr;

        /**
        * Tests if the file/stream can be decoded by this decoder.
        * @param theUrl can be used to check for known file extensions.
        * @param theStream should also be provided if possible, to check for magic numbers, etc.
        * @return the Mimetype of the decoder, or an empty string, if it cannot decode the file.
        */
        virtual std::string canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream = asl::Ptr<asl::ReadableStreamHandle>() ) = 0;
        virtual void shutdown() {}
        IDecoder();
        virtual ~IDecoder();
        static void shutdownAllInstances();
    private:
        static std::set<IDecoder*> _myInstances;
};


}
#endif
