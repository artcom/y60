
#include <asl/raster/pixels.h>

#if defined(_MSC_VER)
#pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <half.h> // ILM OpenEXR 16Bit floating-point implementation
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

namespace y60 {

typedef asl::RGBA_t<half> RASTER_PIXEL_TYPE_RGBA_HALF;
typedef asl::RGB_t<half>  RASTER_PIXEL_TYPE_RGB_HALF;

}
