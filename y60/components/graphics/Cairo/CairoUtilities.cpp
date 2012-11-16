/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

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
    theMatrix[0][0] = static_cast<float>(theCairoMatrix->xx);
    theMatrix[0][1] = static_cast<float>(theCairoMatrix->yx);
    theMatrix[0][2] = 0.0;
    theMatrix[0][3] = 0.0;

    theMatrix[1][0] = static_cast<float>(theCairoMatrix->xy);
    theMatrix[1][1] = static_cast<float>(theCairoMatrix->yy);
    theMatrix[1][2] = 0.0;
    theMatrix[1][3] = 0.0;

    theMatrix[2][0] = 0.0;
    theMatrix[2][1] = 0.0;
    theMatrix[2][2] = 1.0;
    theMatrix[2][3] = 0.0;

    theMatrix[3][0] = static_cast<float>(theCairoMatrix->x0);
    theMatrix[3][1] = static_cast<float>(theCairoMatrix->y0);
    theMatrix[3][2] = 0.0;
    theMatrix[3][3] = 1.0;
}

}
