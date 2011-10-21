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

#include "FFMpegURLProtocol.h"

//#include <CG/glext.h> //offset_h

#include <asl/base/Ptr.h>
#include <asl/base/Block.h>
#include <asl/base/MappedBlock.h>

using namespace asl;
using namespace std;

namespace y60 {

unsigned long RelativeReadableStream::read(void *theDest, unsigned long theSize) {

    unsigned long myPrevOffset = _myOffset;
    AC_DEBUG << "reading... " << theSize;
    try {
        _myOffset = _mySource->readBytes(theDest, theSize, _myOffset);

    } catch (BlockTooSmall ex) {
        //read up to EOS
        _myOffset = _mySource->readBytes(theDest, _mySource->size()-myPrevOffset, myPrevOffset);
    }
    AC_DEBUG << "read " << _myOffset - myPrevOffset << " bytes, fp " << _myOffset;
    return _myOffset - myPrevOffset;
}

unsigned long RelativeReadableStream::seek(int pos, int whence) {
    switch (whence) {
        case SEEK_SET:
            _myOffset = pos;
            break;
        case SEEK_CUR:
            _myOffset += pos;
            break;
        case SEEK_END:
            _myOffset = _mySource->size() + pos;
            break;
        default:
            ;
    }
    AC_DEBUG << " seeked to " << _myOffset << " whence " << whence;
    return _myOffset;
}



map<string,RelativeReadableStream> ourOpenStreams;
void freeStream(RelativeReadableStream * theSource);


static int acstream_open(URLContext *h, const char *filename, int flags) {

    if (ourOpenStreams.find(filename) != ourOpenStreams.end()) {
        RelativeReadableStream * myStream = & ourOpenStreams[filename];
        AC_DEBUG << " got a stream for " << filename;
        h->priv_data = reinterpret_cast<void*>(myStream);
        return 0;

    } else {
        AC_FATAL << " no stream registered as " << filename;
        return -1;
    }
}

static int acstream_read(URLContext *h, unsigned char *buf, int size) {
    RelativeReadableStream * mySource
        = reinterpret_cast<RelativeReadableStream*>(h->priv_data);

    return mySource->read(buf, size);
}

static int64_t acstream_seek(URLContext *h, int64_t pos, int whence) {
    //int64_t myOffset = 0;
    RelativeReadableStream * mySource
        = reinterpret_cast<RelativeReadableStream*>(h->priv_data);

    return mySource->seek( static_cast<int>(pos), whence);

}

static int acstream_close(URLContext *h) {
    RelativeReadableStream * mySource
        = reinterpret_cast<RelativeReadableStream*>(h->priv_data);

    freeStream(mySource);
    return 0;
}

// FFMpeg URLProtocol api
URLProtocol acstream_protocol = {
    "acstream",
    acstream_open,
    acstream_read,
    NULL, /* acstream_write */
    acstream_seek,
    acstream_close,
};


void registerStream(string theUrl, asl::Ptr<ReadableStream> theSource) {

    static bool avRegistered = false;
    if (!avRegistered) {
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 69, 0)
        av_register_protocol2(&acstream_protocol, sizeof(&acstream_protocol));
#elif LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(52,29,0)
        av_register_protocol(&acstream_protocol);
#else
        register_protocol(&acstream_protocol);
#endif
        avRegistered = true;
    }

    //TODO support opening the same stream multiple times
    ourOpenStreams[theUrl] = RelativeReadableStream(theSource);
}

void freeStream(RelativeReadableStream * theSource) {
    map<string,RelativeReadableStream>::iterator s;
    for (s = ourOpenStreams.begin();s != ourOpenStreams.end(); ++s) {
        if ( &(s->second) == theSource) {
            ourOpenStreams.erase(s);
            return;
        }
    }
}

}
