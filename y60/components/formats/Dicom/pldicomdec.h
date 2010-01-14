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

#ifndef INCL_PLDICOMDEC
#define INCL_PLDICOMDEC

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#ifndef INCL_PLPICDEC
#include <paintlib/plpicdec.h>
#endif
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

class PLProducer;
class PLInputStream;
class DicomImage;

class PLDICOMDecoder :
    public PLPicDecoder
{
public:
    //! Creates a pDecoder
    PLDICOMDecoder
    ();

    //! Destroys a pDecoder
    virtual ~PLDICOMDecoder
    ();

    //!
    virtual void Open (PLDataSource * pDataSrc);
    virtual void Close ();

    //! Fills the bitmap with the image.
    virtual void GetImage (PLBmpBase & Bmp);

private:
    DicomImage * m_di;
    bool m_bInvert;
    PLProducer * m_pProducer;
    PLInputStream * m_pInputStream;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: pldicomdec.h,v $
|      Revision 1.2  2004/11/09 16:56:34  janbo
|      *** empty log message ***
|
|      Revision 1.1  2004/10/04 10:22:39  janbo
|      added DICOM PlugIn in components
|
|      Revision 1.1  2004/09/15 14:52:09  artcom
|      - added PLPixelformatTest
|      - added DICOM Decoder
|
|      Revision 1.6  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.5  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.4  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.3  2001/10/21 17:12:40  uzadow
|      Added PSD pDecoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.2  2001/10/06 22:37:08  uzadow
|      Linux compatibility.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|
\--------------------------------------------------------------------
*/
