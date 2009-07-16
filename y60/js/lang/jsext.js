//=============================================================================
// Copyright (C) 2009, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("lang/core.js");
use("lang/ArrayExt.js");
use("lang/StringExt.js");
use("lang/VectorExt.js");

/*
 * TODO: Namespace jsext specific helpers so that you would need to use
 *   jsext.makeExtendable(Object)
 * though
 *   jsext.typeOf(Object)
 * would be cumbersome.
 *
 * Goal should be anyway to NOT have everything in the global namespace.
 * */