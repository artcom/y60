//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: c4d_include.h,v $
//   $Author: ulrich $
//   $Revision: 1.2 $
//   $Date: 2005/04/29 08:46:12 $
//
//  Description: This class implements a exporter plugin for Cinema 4D.
//
//=============================================================================

#ifndef _ac_c4d_include_h_
#define _ac_c4d_include_h_

#include <windows.h>
#include <mmsystem.h>

#undef COLOR_BACKGROUND
#undef CreateDialog
#define Polygon CPolygon
#undef min
#undef max

#undef API_VERSION
#include <c4d.h>
#undef offsetof

#endif


