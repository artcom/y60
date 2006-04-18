//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_render_GLBufferAdapter_h_
#define _ac_render_GLBufferAdapter_h_

#include <asl/Exception.h>
#include <asl/Block.h>

#include <paintlib/planydec.h>

namespace y60 {
    
    enum GLSourceBuffer {
        FRAME_BUFFER,
        DEPTH_BUFFER
    };   

    DEFINE_EXCEPTION(GLBufferAdapterException, asl::Exception);
    class BufferAdaptor {
        public:
            BufferAdaptor(unsigned theWidth, unsigned theHeight, unsigned theComponents);
            virtual ~BufferAdaptor();
            virtual void performAction(GLSourceBuffer theSourceBuffer);

            asl::Block & getBlock();
            unsigned getWidth() const;
            unsigned getHeight() const;
            unsigned getComponents() const;
        protected:
            void alloc(const unsigned myMemorySize);
        private:
            asl::Block _myData;
            unsigned _myWidth;
            unsigned _myHeight;
            unsigned _myComponents;
    };

/* // paintlib image formats
#define PL_FT_WINBMP  1
#define PL_FT_MACPICT 2
#define PL_FT_TARGA   3
#define PL_FT_TIFF    4
#define PL_FT_JPEG    5
#define PL_FT_PNG     6 // supported
#define PL_FT_EPSTIFF 7
#define PL_FT_WMF     8
#define PL_FT_EMF     9
#define PL_FT_PCX    10
#define PL_FT_PGM    11
#define PL_FT_GIF    12
#define PL_FT_PPM    13
#define PL_FT_PSD    14
#define PL_FT_SGI    15
*/
    
    class BufferToFile : public BufferAdaptor{
        public:
            BufferToFile (unsigned theWidth, unsigned theHeight, unsigned theComponents);
            virtual ~BufferToFile();
            bool saveBufferAsImage(unsigned theFormat, const std::string & theFileName);
            
    };
    
}

#endif // _ac_render_GLBufferAdapter_h_

