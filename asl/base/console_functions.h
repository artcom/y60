/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: UnitTest.cpp,v $
//
//   $Revision: 1.5 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _included_asl_console_functions_h_
#define _included_asl_console_functions_h_

#include "settings.h"

#include <iostream>

namespace asl {;

/*! \addtogroup aslbase */
/* @{ */
    bool isTTY(std::ostream & os);

    std::ostream & TTYGREEN (std::ostream & os );

    std::ostream & TTYRED (std::ostream & os );
    std::ostream & TTYYELLOW (std::ostream & os );
    std::ostream & TTYBLUE (std::ostream & os );

    std::ostream & ENDCOLOR (std::ostream & os );
/* @} */
}

#endif
