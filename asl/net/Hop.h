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
