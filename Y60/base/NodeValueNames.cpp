//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "NodeValueNames.h"

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
IMPLEMENT_ENUM(y60::ImageType, y60::ImageTypeStrings);





