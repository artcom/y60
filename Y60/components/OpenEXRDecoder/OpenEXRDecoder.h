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

#ifndef _ac_y60_OpenEXRDecoder_h_
#define _ac_y60_OpenEXRDecoder_h_

#include <asl/Exception.h>
#include <paintlib/plpicdec.h>
#include <OpenEXR/IlmImf/ImfRgbaFile.h>

DEFINE_EXCEPTION(OpenEXRException, asl::Exception);

class OpenEXRDecoder : public PLPicDecoder {
    public:
        //! Creates a pDecoder
        OpenEXRDecoder();

        //! Destroys a pDecoder
        virtual ~OpenEXRDecoder();

        //!
        virtual void Open (PLDataSource * pDataSrc);
        virtual void Close ();

        //! Fills the bitmap with the image.
        virtual void GetImage (PLBmpBase & Bmp);
    private:
        Imf::RgbaInputFile * _myOpenEXRFile;
};

#endif
