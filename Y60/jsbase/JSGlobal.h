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
//   $RCSfile: JSGlobal.h,v $
//   $Author: ulrich $
//   $Revision: 1.3 $
//   $Date: 2004/10/06 10:12:55 $
//
//
//=============================================================================


#include <asl/Exception.h>

#include "JScppUtils.h"

namespace jslib {

DEFINE_EXCEPTION(DoesNotExist, asl::Exception);

template <class T> struct JSClassTraits;

struct Global {
    static JSFunctionSpec * Functions();
};

}
