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
//   $RCSfile: Text.h,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2004/11/07 10:01:43 $
//
//  Description: A simple renderer.
//
//=============================================================================

#ifndef AC_Y60_TEXT
#define AC_Y60_TEXT

#include <asl/Ptr.h>
#include <asl/Vector234.h>

#include <map>
#include <string>
namespace y60 {

    class TextRenderer;

    class Text {
        public:
            enum RENDERSTYLE {
                BLENDED_TEXT,
                SHADED_TEXT,
                SOLID_TEXT
            };
            Text(TextRenderer * theRenderer,
                 const asl::Vector2f & thePos,
                 const asl::Vector4f & theTextColor,
                 const asl::Vector4f & theBackColor,
                 const std::string & theString,
                 const std::string theFont);
            virtual ~Text();
            void render();
            const RENDERSTYLE & getRenderStyle() { return _myRenderStyle; }
            const asl::Vector2f _myPos;
            const std::string   _myString;
   	        const asl::Vector4f _myTextColor;
   	        const asl::Vector4f _myBackColor;
   	        const std::string   _myFont;
   	        TextRenderer*      _myRenderer;
        private:
   	        RENDERSTYLE _myRenderStyle;
    };
    typedef asl::Ptr<Text> TextPtr;

} // namespace y60

#endif // AC_Y60_TEXT
