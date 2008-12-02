#include "HeightToNormalMap.h"

#include <iostream>

#include <asl/math/numeric_functions.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpixel8.h>
#include <paintlib/plbitmap.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

using namespace std;
using namespace asl;

HeightToNormalMap::HeightToNormalMap() {
}

HeightToNormalMap::~HeightToNormalMap() {
}

void
HeightToNormalMap::Apply(PLBmpBase * theSource, PLBmp * theDestination) const {
  theDestination->Create(theSource->GetWidth(), theSource->GetHeight(), PLPixelFormat::A8R8G8B8);
    //theDestination->SetHasAlpha(true);
    PLPixel8 ** mySourceLines = (PLPixel8**)theSource->GetLineArray();
    PLPixel32 ** myDestinationLines = (PLPixel32**)theDestination->GetLineArray();

    for (int y = 0; y < theSource->GetHeight(); ++y) {
        PLPixel8  * mySourceLine = mySourceLines[y];
        PLPixel32 * myDestinationLine = myDestinationLines[y];
        
        PLPixel8 * myNextLine;
        if (y + 1 == theSource->GetHeight()) {
            myNextLine = mySourceLines[y];
        } else {
            myNextLine = mySourceLines[y + 1];
        }
        for (int x = 0; x < theSource->GetWidth(); ++x) {
            float myHeight = mySourceLine->Get();
            float aboveNeighbourHeight = myNextLine->Get();
            float rightNeighbourHeight;
            if (x + 1 == theSource->GetWidth()) {
                rightNeighbourHeight = mySourceLine->Get();
            } else {
                rightNeighbourHeight = (mySourceLine + 1)->Get();
            }
            myHeight /= 255;
            aboveNeighbourHeight /= 255;
            rightNeighbourHeight /= 255;
            float myZ = 1.0f / float((sqrt(pow(myHeight - aboveNeighbourHeight, 2) + 
                                    pow(myHeight - rightNeighbourHeight, 2) + 1)));
            float myX = (myHeight - rightNeighbourHeight) * myZ;
            float myY = (myHeight - aboveNeighbourHeight) * myZ;

            myDestinationLine->SetR((PLBYTE)minimum(128.0 * myX + 128, 255.0));
            myDestinationLine->SetG((PLBYTE)minimum(128.0 * -myY + 128, 255.0));
            myDestinationLine->SetB((PLBYTE)minimum(128.0 * myZ + 128, 255.0));
            myDestinationLine->SetA(255);

            mySourceLine++;
            myDestinationLine++;
            myNextLine++;
        }
    }
}
