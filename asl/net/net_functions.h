/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2007, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: 
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
// Last Review:  ms & ab 2007-08-14
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      ok
//    documentation           :      disaster
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      ok
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      ok
//    interfaces              :      ok
//    confidence              :      ok
//    integration             :      ok
//    dependencies            :      ok
//    error handling          :      ok
//    logging                 :      notapp
//    cheesyness              :      ok
//
//    overall review status   :      ok
//
//    recommendations: merge with net_functions.h, translate german comments to english 
*/

#ifndef _included_asl_net_functions_
#define _included_asl_net_functions_

#include <asl/Exception.h>
#include <asl/Block.h>

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

    asl::Block getHardwareAddress(const std::string & theInterfaceName = "");
} //Namespace asl

#endif

