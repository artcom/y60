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
//   $RCSfile: ExportExceptions.h,v $
//   $Author: christian $
//   $Revision: 1.5 $
//   $Date: 2005/04/13 17:23:23 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_maya_xport_exceptions_h_
#define _ac_maya_xport_exceptions_h_

#include <asl/base/Exception.h>

DEFINE_EXCEPTION(ExportException, asl::Exception)
DEFINE_EXCEPTION(MayaException, ExportException)
DEFINE_EXCEPTION(IOError, ExportException)
DEFINE_EXCEPTION(OpenFailed, ExportException)
DEFINE_EXCEPTION(WriteFailed, ExportException)
DEFINE_EXCEPTION(ExportCancelled, ExportException)

#endif // _ac_maya_xport_exceptions_h_
