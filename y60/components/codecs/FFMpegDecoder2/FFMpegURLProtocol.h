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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include <asl/base/Ptr.h>
#include <asl/base/Stream.h>

#include "y60_ffmpegdecoder2_settings.h"

#ifdef OSX
    extern "C" {
#       include <libavformat/avformat.h>
    }
#   undef AV_NOPTS_VALUE
#   define AV_NOPTS_VALUE 0x8000000000000000LL
#else
#   if defined(_MSC_VER)
#       pragma warning(push,1)
#   endif
    extern "C" {
#   include <avformat.h>
    }
#   if defined(_MSC_VER)
#       pragma warning(pop)
#   endif
#endif

#ifndef AV_VERSION_INT
#define AV_VERSION_INT(a,b,c) (a<<16 | b<<8 | c)
#endif

namespace y60 {

    /**
     * registers an open stream so that ffmpeg can use it.
     * the stream will be de-registered automatically by ffmpeg on close.
     * @param theUrl identifies the stream
     * @param theSource
     */
    void registerStream(std::string theUrl, asl::Ptr<asl::ReadableStream> theSource);


    /**
     * @ingroup Y60video
     * A generic ReadableStream adapter for streams
     * that need to remember the current stream position
     * it behaves like the file protocol in C. supports read & seek
     *
     */
    class RelativeReadableStream {
        public:
            RelativeReadableStream() : _mySource() {}
            RelativeReadableStream( asl::Ptr<asl::ReadableStream> theSource)
                : _mySource(theSource), _myOffset(0) {}

            unsigned long read(void *theDest, unsigned long theSize);
            unsigned long seek(int pos, int whence);

        private:
            asl::Ptr<asl::ReadableStream> _mySource;
            unsigned long _myOffset;
    };

}

