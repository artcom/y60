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
    return Vector2f(0.0f,0.0f);
}
}

