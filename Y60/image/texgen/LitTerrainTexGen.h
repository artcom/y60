////////////////////////////////////////////////////////////////////////
//                                                                    //
//     LitTerrainTexGen.h                                             //
//                                                                    // 
////////////////////////////////////////////////////////////////////////

#ifndef INCL_LITTERRAINGENERATOR
#define INCL_LITTERRAINGENERATOR

class PLBmp;
class PLRect;

namespace TexGen {

void generateMultiplicationMap (const PLBmp & lightMap, PLBmp & destMap);

void applyMultiplicationMap (PLBmp & textureBmp, const PLBmp & multBmp, 
            const PLRect & srcRect, double myLightingFactor); 

}
#endif

