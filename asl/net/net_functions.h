/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2002, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: net_functions.h,v $
//
//   $Revision: 1.1 $
//
// Description: misc operating system & environment helper functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _included_asl_net_functions_
#define _included_asl_net_functions_

#include <asl/Exception.h>

#include <string>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef LINUX
    #include <unistd.h>
#endif

#ifdef WIN32
    #include <winsock2.h>
#endif


namespace asl {

    std::string localhostname();
    std::string hostname(unsigned long hostAddress);
    unsigned long hostaddress(const std::string & theHost);

    unsigned long from_dotted_address(const std::string & dottedAddress);
    std::string as_dotted_address(unsigned long theAdress);

} //Namespace asl

#endif

