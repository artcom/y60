//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include <stdlib.h>

#include "Value.h"

namespace xpath {

    StringValue
    NumberValue::toString() const {
        std::ostringstream stream;
        stream << value;
        return StringValue(stream.str());
    }

}
