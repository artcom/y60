#ifndef AC_Y60_HEIGHT_TO_NORMAL_MAP_INCLUDED
#define AC_Y60_HEIGHT_TO_NORMAL_MAP_INCLUDED

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpaintlibdefs.h>
#include <paintlib/Filter/plfilter.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

class PLBmp;

class HeightToNormalMap : public PLFilter {
    public:
        HeightToNormalMap();
        virtual ~HeightToNormalMap();
        virtual void Apply(PLBmpBase * theSource, PLBmp * theDestination) const;
};
#endif // AC_Y60_HEIGHT_TO_NORMAL_MAP_INCLUDED
