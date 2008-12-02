#include "CTFile.h"

#include <asl/math/numeric_functions.h>

using namespace asl;
using namespace std;

#define DB(x) // x;

namespace y60 {

asl::Vector2f 
CTFile::getDefaultWindow() const {
    return Vector2f(0.0f,0.0f);
}
}

