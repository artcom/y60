//============================================================================
//
// Copyright (C) 2002, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: TextureCompressor.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.8 $
//
// Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef AC_Y60_GLTEXTURE_COMPRESSOR_INCLUDED
#define AC_Y60_GLTEXTURE_COMPRESSOR_INCLUDED

#include <y60/PixelEncoding.h>

#include <asl/Block.h>
#include <asl/Exception.h>
#include <paintlib/plbitmap.h>
#include <paintlib/planybmp.h>

#include <stdio.h>
namespace y60 {

    DEFINE_EXCEPTION(GLTextureCompressionException, asl::Exception);
    class TextureCompressor {
        public:
            TextureCompressor(PixelEncoding myFormat);
            virtual ~TextureCompressor();

            int compress(const PLAnyBmp& AnyBmp, asl::VariableCapacityBlock & theDestBlock);
        private:
            PixelEncoding _myFormat;
    };
};

#endif
