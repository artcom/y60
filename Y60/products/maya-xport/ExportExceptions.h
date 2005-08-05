//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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

#include <asl/Exception.h>

DEFINE_EXCEPTION(ExportException, asl::Exception)
DEFINE_EXCEPTION(MayaException, ExportException)
DEFINE_EXCEPTION(IOError, ExportException)
DEFINE_EXCEPTION(OpenFailed, ExportException)
DEFINE_EXCEPTION(WriteFailed, ExportException)
DEFINE_EXCEPTION(ExportCancelled, ExportException)

#endif // _ac_maya_xport_exceptions_h_
