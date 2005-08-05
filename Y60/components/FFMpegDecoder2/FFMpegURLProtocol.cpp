
//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: acfile.cpp,v $
//   $Author: martin $
//   $Revision: 1.178 $
//   $Date: 2005/04/29 14:09:10 $
//
//  Description: A simple scene class.
//
//=============================================================================

#include "FFMpegURLProtocol.h"

#include <asl/Ptr.h>
#include <asl/Block.h>
#include <asl/MappedBlock.h>

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4244)
#define EMULATE_INTTYPES
#endif

#include <ffmpeg/avformat.h>


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

static offset_t acstream_seek(URLContext *h, offset_t pos, int whence) {
    offset_t myOffset = 0;
    RelativeReadableStream * mySource 
        = reinterpret_cast<RelativeReadableStream*>(h->priv_data);
    
    return mySource->seek(pos, whence);
        
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


void registerStream(string theUrl, Ptr<ReadableStream> theSource) {
    
    static bool avRegistered = false;
    if (!avRegistered) {
        register_protocol(&acstream_protocol);
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
