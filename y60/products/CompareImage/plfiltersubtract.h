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

#ifndef FILTERSUBTRACT_H
#define FILTERSUBTRACT_H

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plbitmap.h>
#include <paintlib/Filter/plfilter.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)


//! Subtracts one bitmap from another
class PLFilterSubtract : public PLFilter
{
public:
  //!
  PLFilterSubtract(PLBmpBase * pBmpSubtrahend);
  //!
  virtual ~PLFilterSubtract();
  //!
  virtual void Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest)
    const;
private:
	PLBmpBase * m_pBmpSubtrahend;
};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plfiltersubtract.h,v $
|      Revision 1.2  2004/08/03 14:44:36  martin
|      PL include bug workaround
|
|      Revision 1.1  2004/08/03 14:30:09  martin
|      better image compare algorithem
|
|      Revision 1.5  2004/06/15 10:26:13  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.4  2003/02/27 23:01:33  uzadow
|      Linux version of PLFilterRotate, nicer test output
|
|      Revision 1.3  2003/02/27 22:46:57  uzadow
|      Added plfilterrotate.
|
|      Revision 1.1  2002/02/05 09:40:54  david
|      pavel added rotate90 filter
|
\--------------------------------------------------------------------
*/
