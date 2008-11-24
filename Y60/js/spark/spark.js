//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("common.js");

/**
 * The SPARK namespace.
 * 
 * Everything defined by this file goes in here.
 */
var spark = Namespace("spark");

use("meta.js");
use("load.js");
use("text.js");
use("cache.js");
use("components.js");

if (useSparkWidgets)
    use("widgets.js");
