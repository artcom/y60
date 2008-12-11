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
// Description:  ??
//
// Last Review:  ms & ab 2007-08-14
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      poor
//    formatting              :      ok
//    documentation           :      poor
//    test coverage           :      disaster
//    names                   :      ok
//    style guide conformance :      fair
//    technological soundness :      notapp
//    dead code               :      disaster
//    readability             :      ok
//    understandability       :      notapp
//    interfaces              :      notapp
//    confidence              :      notapp
//    integration             :      notapp
//    dependencies            :      ok
//    error handling          :      notapp
//    logging                 :      notapp
//    cheesyness              :      disaster
//
//    overall review status   :      disaster
//
//    recommendations:  delete this class 
*/
#ifndef INCL_HOP
#define INCL_HOP

#include <string>
#include <vector>

namespace inet {

    class Hop {
        public:
            Hop() {};
            Hop(const std::string & theHostname, const std::string & theIp, const std::string & theNiftyStatus);
            virtual ~Hop();
            void setIp(const std::string & theIp) {
                _myIp = theIp;
            }
            const std::string & getIp() {
                return _myIp;
            }
            const std::string & getHostname(){
                return _myHostname;
            }
            void setHostname(const std::string & theHostname) {
                _myHostname = theHostname;
            }
            const std::string & getNiftyStatus(){
                return _myNiftyStatus;
            }
            void setNiftyStatus(const std::string & theNiftyStatus) {
                _myNiftyStatus = theNiftyStatus; 
            }
        friend std::ostream& operator<<(std::ostream&, const Hop&);
        
        private:
            std::string _myIp;
            std::string _myHostname;
            std::string _myNiftyStatus;
    };
    
    typedef std::vector<Hop> Route;

    std::ostream& operator<<(std::ostream&, const Hop &);
    std::ostream& operator<<(std::ostream&, const Route &);
}
#endif
