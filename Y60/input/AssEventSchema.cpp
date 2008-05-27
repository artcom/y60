// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "AssEventSchema.h"

extern std::string ourasseventxsd;

namespace y60 {
    std::string getASSSchema() {
        return ourasseventxsd;
    }
}
