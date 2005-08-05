////////////////////////////////////////////////////////////////////////
//                                                                    //
//     LitTerrainTexGen.cpp                                           //
//                                                                    // 
////////////////////////////////////////////////////////////////////////

#include "LitTerrainTexGen.h"

#include <paintlib/plpixel24.h>
#include <paintlib/plbitmap.h>
#include <paintlib/plpoint.h>
#include <paintlib/plrect.h>
#include <paintlib/planybmp.h>
#include <paintlib/Filter/plfiltercrop.h>

#include <assert.h>
#include <stdexcept>
#include <math.h>

namespace TexGen {

    using namespace std;

    void generateMultiplicationMap (const PLBmp & lightMap, PLBmp & destMap) {
        assert(lightMap.GetWidth() == destMap.GetWidth() );
        assert(lightMap.GetHeight() == destMap.GetHeight() );
        assert(lightMap.GetBitsPerPixel() == 8);
        assert(destMap.GetBitsPerPixel() == 8);

        PLBYTE ** myLightMapLines   = lightMap.GetLineArray(); 
        PLBYTE ** myDestMapLines = destMap.GetLineArray();

        PLBYTE myScale;
        for (int y=0;y<lightMap.GetHeight(); y++) {
            for (int x=0;x<lightMap.GetWidth(); x++) {
                PLBYTE& col = myDestMapLines[y][x];
                myScale =  PLBYTE(myLightMapLines[y][x]);
                int tempInt = (col*myScale)/255;
                col = min(tempInt, 255); 
            }
        }
    }

    void applyMultiplicationMap (PLBmp & textureBmp, const PLBmp & attnBmp, 
            const PLRect & srcRect, double myLightingFactor) 
    {  
        assert(attnBmp.GetBitsPerPixel() == 8);
        assert(textureBmp.GetBitsPerPixel() == 32);
        assert (srcRect.Width() > 0 && srcRect.Height() > 0);
        
        double Scale = double(srcRect.Width())/textureBmp.GetWidth();
        double YScale = double(srcRect.Height())/textureBmp.GetHeight();
        assert (Scale == YScale);

        PLBYTE ** myAttnBmpLines   = attnBmp.GetLineArray(); 
        PLPixel32 ** myTextureBmpLines = (PLPixel32**)textureBmp.GetLineArray();

        if (Scale > 0.99) {
            for (int y=0;y<textureBmp.GetHeight(); y++) {
                for (int x=0;x<textureBmp.GetWidth(); x++) {
                    PLPixel32& col = myTextureBmpLines[y][x];

                    PLBYTE myScale;
                    myScale =  PLBYTE(myAttnBmpLines[int(Scale*y+srcRect.tl.y)]
                                        [int(Scale*x+srcRect.tl.x)]);
                    int tempIntR = int((myLightingFactor * col.GetR() * myScale)/256);
                    int tempIntG = int((myLightingFactor * col.GetG() * myScale)/256);
                    int tempIntB = int((myLightingFactor * col.GetB() * myScale)/256);
                    col.Set(min(tempIntR,255), 
                            min(tempIntG,255), 
                            min(tempIntB,255),
                            255);
                }
            }
        } else {
            int MaxY = attnBmp.GetHeight()-1;
            int MaxX = attnBmp.GetWidth()-1;
            for (int y=0;y<textureBmp.GetHeight(); y++) {
                double ySrcPos = Scale*y+srcRect.tl.y;
                int yLightPos = int (ySrcPos);
                double yLightFade = fmod (ySrcPos, 1);
                if (yLightPos >= MaxY) {
                    yLightPos = MaxY - 1;
                    yLightFade = 1;
                }
                for (int x=0;x<textureBmp.GetWidth(); x++) {
                    PLPixel32& col = myTextureBmpLines[y][x];
                    double xSrcPos = Scale*x+srcRect.tl.x;
                    int xLightPos = int (xSrcPos); 
                    double xLightFade = fmod (xSrcPos, 1);
                    if (xLightPos >= MaxX) {
                        xLightPos = MaxX - 1;
                        xLightFade = 1;
                    }

                    double myScale = double(myAttnBmpLines[yLightPos][xLightPos])*
                                      (1-xLightFade)*(1-yLightFade) + 
                                  double(myAttnBmpLines[yLightPos][xLightPos+1])*
                                      xLightFade*(1-yLightFade) + 
                                  double(myAttnBmpLines[yLightPos+1][xLightPos])*
                                      (1-xLightFade)*yLightFade + 
                                  double(myAttnBmpLines[yLightPos+1][xLightPos+1])*
                                      xLightFade*yLightFade;
                    
                    int tempIntR = int((myLightingFactor * col.GetR() * myScale) /256);
                    int tempIntG = int((myLightingFactor * col.GetG() * myScale) /256);
                    int tempIntB = int((myLightingFactor * col.GetB() * myScale) /256);
                    col.Set(min(tempIntR,255), 
                            min(tempIntG,255), 
                            min(tempIntB,255),
                            255);
                }
            }
        }
    }


}

