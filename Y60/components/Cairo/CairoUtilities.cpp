//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "CairoUtilities.h"

namespace jslib {

void
convertMatrixToCairo(Matrix4f &theMatrix, cairo_matrix_t *theCairoMatrix) {
    theCairoMatrix->xx = theMatrix[0][0];
    theCairoMatrix->yx = theMatrix[0][1];
    theCairoMatrix->xy = theMatrix[1][0];
    theCairoMatrix->yy = theMatrix[1][1];
    theCairoMatrix->x0 = theMatrix[3][0];
    theCairoMatrix->y0 = theMatrix[3][1];
}

void
convertMatrixFromCairo(Matrix4f &theMatrix, cairo_matrix_t *theCairoMatrix) {
    theMatrix[0][0] = theCairoMatrix->xx;
    theMatrix[0][1] = theCairoMatrix->yx;
    theMatrix[0][2] = 0.0;
    theMatrix[0][3] = 0.0;

    theMatrix[1][0] = theCairoMatrix->xy;
    theMatrix[1][1] = theCairoMatrix->yy;
    theMatrix[1][2] = 0.0;
    theMatrix[1][3] = 0.0;

    theMatrix[2][0] = 0.0;
    theMatrix[2][1] = 0.0;
    theMatrix[2][2] = 1.0;
    theMatrix[2][3] = 0.0;

    theMatrix[3][0] = theCairoMatrix->x0;
    theMatrix[3][1] = theCairoMatrix->y0;
    theMatrix[3][2] = 0.0;
    theMatrix[3][3] = 1.0;
}

}
