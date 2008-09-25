/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
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
