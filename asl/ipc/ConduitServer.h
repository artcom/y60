//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef __asl_Conduit_server_included_
#define __asl_Conduit_server_included_

#include "ThreadedConduit.h"

#include <string>

namespace asl {

#define DB(x) // x;

/*! \addtogroup aslipc */
/* @{ */

//! Abstract Server.  
/** 
    Each server runs in its own thread. Implement ThreadedConduit<POLICY>::processData() in your derived class.
**/
template <class POLICY> class ConduitAcceptor;
    
template <class POLICY> 
class ConduitServer : public ThreadedConduit<POLICY> {
        friend class ConduitAcceptor<POLICY>;
    public:        
        typedef asl::Ptr<ConduitServer<POLICY> > Ptr;
        typedef asl::WeakPtr<ConduitServer<POLICY> > WPtr;
       
        WPtr getSelf() const {
            return _mySelf;
        }

    protected:
        ConduitServer(typename POLICY::Handle theHandle) :
            ThreadedConduit<POLICY>(theHandle), _myAcceptor(0) { }
        
        ConduitAcceptor<POLICY> * getAcceptor() const {
            return _myAcceptor;
        }

        void virtual setAcceptor(ConduitAcceptor<POLICY> * theAcceptor) {
            _myAcceptor = theAcceptor;
        }
    
    private:
        // hide ctors - can only be constructed by factory method
        ConduitServer();
        ConduitServer(const ConduitServer &);
        ConduitServer operator=(const ConduitServer &);


        void setSelf (Ptr theSelfPtr) {
            _mySelf = theSelfPtr;
        }
        WPtr _mySelf;
        ConduitAcceptor<POLICY> * _myAcceptor;
};

/* @} */
}

#undef DB
#endif
