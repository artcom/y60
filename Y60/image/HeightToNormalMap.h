#ifndef AC_Y60_HEIGHT_TO_NORMAL_MAP_INCLUDED
#define AC_Y60_HEIGHT_TO_NORMAL_MAP_INCLUDED

#include <paintlib/plpaintlibdefs.h>
#include <paintlib/Filter/plfilter.h>

class PLBmp;

class HeightToNormalMap : public PLFilter {
    public:
        HeightToNormalMap();
        virtual ~HeightToNormalMap();
        virtual void Apply(PLBmpBase * theSource, PLBmp * theDestination) const;
};
#endif // AC_Y60_HEIGHT_TO_NORMAL_MAP_INCLUDED
