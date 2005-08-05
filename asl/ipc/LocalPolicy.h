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
//   $RCSfile: LocalPolicy.h,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2004/10/17 16:12:09 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================

#ifndef __asl_LocalPolicy_included

#ifdef WIN32

#include "NamedPipePolicy.h"
namespace asl {
    typedef NamedPipePolicy LocalPolicy;
}

#else

#include "UnixSocketPolicy.h"
namespace asl {
    typedef UnixSocketPolicy LocalPolicy;
}

#endif

#endif
