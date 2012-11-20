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
//
//   $Id: PLFilterGaussianBlur.h,v 1.1 2005/03/24 23:36:00 christian Exp $
//   $RCSfile: PLFilterGaussianBlur.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:36:00 $
//
//
//  Description:
//
//
//=============================================================================

#ifndef _ac_y60_PLFilterGaussianBlur_h_
#define _ac_y60_PLFilterGaussianBlur_h_

#include "y60_image_settings.h"

#include <vector>
#include <asl/base/Logger.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpaintlibdefs.h>
#include <paintlib/Filter/plfilter.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

class PLBmp;

typedef std::vector<int> KernelVec;

class PLFilterGaussianBlur : public PLFilter {
    public:
        PLFilterGaussianBlur(double theRadius, unsigned theRealWidth, unsigned theRealHeight, double theSigma=1.0)
            : _mySigma(theSigma),
            _myRadius(theRadius),
            _myRealWidth(theRealWidth),
            _myRealHeight(theRealHeight)
        {
            calcKernel();
        }
    virtual ~PLFilterGaussianBlur() {};
    virtual void Apply(PLBmpBase * theSource, PLBmp * theDestination) const;

    private:
    void calcKernel() const;

    mutable double _mySigma;
    mutable double _myRadius;
    mutable int _myKernelWidth;
    mutable KernelVec _myKernel;
    unsigned _myRealWidth;
    unsigned _myRealHeight;
};

#endif
