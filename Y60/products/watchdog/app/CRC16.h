//============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: CRC16.h,v 1.1 2004/09/15 15:03:45 ulrich Exp $
//   $Author: ulrich $
//   $Revision: 1.1 $
//   $Date: 2004/09/15 15:03:45 $
//
// CRC 16-bit
//
//=============================================================================

#ifndef _ac_watchdog_CRC16_h_
#define _ac_watchdog_CRC16_h_

namespace asl {
    unsigned short CRC16(unsigned char* pcData, unsigned int nCount);
}

#endif
