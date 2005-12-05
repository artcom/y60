

//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
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

#include <asl/Ptr.h>
#include <asl/Stream.h>

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
            RelativeReadableStream() : _mySource(0) {}
            RelativeReadableStream( asl::Ptr<asl::ReadableStream> theSource) 
                : _mySource(theSource), _myOffset(0) {}

            unsigned long read(void *theDest, unsigned long theSize);         
            unsigned long seek(int pos, int whence);

        private:
            asl::Ptr<asl::ReadableStream> _mySource;
            unsigned long _myOffset;
    };

}

