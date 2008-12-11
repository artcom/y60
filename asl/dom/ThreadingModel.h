/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: ThreadingModel.h,v $
//
//   $Revision: 1.2 $
//
// Description: tiny fast XML-Parser and DOM
// 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#ifndef _asl_xml_ThreadingModel_included_
#define _asl_xml_ThreadingModel_included_

#include <asl/base/AtomicCount.h>


namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

    typedef asl::SingleThreaded ThreadingModel;
    
    /* @} */
} //Namespace dom

#endif
