//============================================================================
//
// Copyright (C) 2000-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "Matrix4.h"

static const char * MatrixTypeStrings[] =
{
    "identity",
    "x_rotating",
    "y_rotating",
    "z_rotating",
    "rotating",
    "scaling",
    "orthogonal",
    "linear",
    "translating",
    "affine",
    "unknown",
    ""
};

IMPLEMENT_ENUM( asl::MatrixType, MatrixTypeStrings );
