#include "CTFile.h"
#include <y60/Image.h>
#include <asl/numeric_functions.h>
#include <y60/PLFilterResizePadded.h>
#include <paintlib/Filter/plfilterresizebilinear.h>
#include <paintlib/plpixelformat.h>
#include <paintlib/planybmp.h>
#include <paintlib/plsubbmp.h>
#include <paintlib/plpngenc.h>

using namespace asl;
using namespace std;

#define DB(x) // x;

namespace y60 {

asl::Vector2f 
CTFile::getDefaultWindow() const {
    switch (getEncoding()) {
        case GRAY:
            return Vector2f(float(1<<7), float(1<<8));
        case GRAY16:
            return Vector2f(float(1<<15), float(1<<16));
        case GRAYS16:
            return Vector2f(0.0f, float(1<<16));
        default:
            throw CTException("unsupported pixel format", PLUS_FILE_LINE);
    }
}
}

