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
//   $RCSfile: BitmapTextRenderer.h,v $
//   $Author: janbo $
//   $Revision: 1.4 $
//   $Date: 2004/10/18 16:22:56 $
//
//  Description: A simple renderer.
//
//=============================================================================

#ifndef AC_Y60_BITMAPTEXTRENDERER
#define AC_Y60_BITMAPTEXTRENDERER

#include "TextRenderer.h"

#include <asl/base/Exception.h>
#include <asl/base/Ptr.h>
#include <asl/math/Vector234.h>

namespace y60 {
    enum BITMAPFONT {
        BOOKMAN72_FONT,
        COURIER24_FONT,
        HELV24_FONT,
        SCREEN8_FONT,
        SCREEN13_FONT,
        SCREEN15_FONT,
        SYNTAXBOLD18_FONT,
        SYNTAXMED144_FONT,
        TIMES12_FONT,
        TIMES14_FONT,
        TIMES144_FONT
    };

    static const char* myBitmapFontStr[] = {
        "Bookman72",
        "Courier24",
        "Helv24",
        "Screen8",
        "Screen13",
        "Screen15",
        "SyntaxBold18",
        "SyntaxMed144",
        "Times12",
        "Times14",
        "Times144",
        0
    };

    class BitmapTextRenderer : public TextRenderer{
    	public:
    		BitmapTextRenderer();
    		~BitmapTextRenderer();

            virtual void renderText(TextPtr & theText);
            virtual bool haveFont(const std::string theFontName);

    	private:
    };
    typedef asl::Ptr<BitmapTextRenderer> BitmapTextRendererPtr;

} // namespace y60

#endif
