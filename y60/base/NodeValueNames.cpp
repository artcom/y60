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
*/

//own header
#include "NodeValueNames.h"

#include <asl/base/Logger.h>
#ifdef OSX
#undef verify
#endif

// [DS, TS] This can't be in the y60 namespace because the ms compiler
// b0rks. So just keep it in global space which does no harm anyway
// since it only creates a class which itself will be in y60.

IMPLEMENT_BITSET(y60::TargetBuffers, y60::TargetBuffer, y60::TargetBuffersStrings);
IMPLEMENT_ENUM(y60::BlendEquation, y60::BlendEquationStrings);
IMPLEMENT_BITSET(y60::RenderStyles, y60::RenderStyle, y60::RenderStyleStrings);
IMPLEMENT_ENUM(y60::VertexBufferUsage, y60::VertexBufferUsageStrings);
IMPLEMENT_ENUM(y60::BlendFunction, y60::BlendFunctionStrings);

IMPLEMENT_ENUM(y60::TextureApplyMode, y60::TextureApplyModeStrings);
IMPLEMENT_ENUM(y60::TextureSampleFilter, y60::TextureSampleFilterStrings);
IMPLEMENT_ENUM(y60::TextureWrapMode, y60::TextureWrapModeStrings);
IMPLEMENT_ENUM(y60::TextureUsage, y60::TextureUsageStrings);
IMPLEMENT_ENUM(y60::TextureType, y60::TextureTypeStrings);
IMPLEMENT_ENUM(y60::PrimitiveType, y60::PrimitiveTypeStrings);

IMPLEMENT_ENUM(y60::ImageType, y60::ImageTypeStrings);

namespace y60 {

    static VertexBufferUsage
    initDefaultVertexBufferUsage() {
        const char * myDefaultUsageString = ::getenv("Y60_DEFAULT_VBO_USAGE");
        if (myDefaultUsageString) {
            AC_INFO << "Y60_DEFAULT_VBO_USAGE is set to "<< myDefaultUsageString;
            VertexBufferUsage myDefaultUsage;
            myDefaultUsage.fromString(myDefaultUsageString);
            return myDefaultUsage;
        } else {
            return VERTEX_USAGE_STATIC_DRAW;
        }
    };

    VertexBufferUsage getDefaultVertexBufferUsage() {
        static VertexBufferUsage myDefault =  initDefaultVertexBufferUsage();
        return myDefault;
    }
}

