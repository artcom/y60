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
// Description: 
//     Classes for networked or local communication between processes
//
// Last Review:  ms 2007-08-15
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      ok
//    documentation           :      ok
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      ok
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      fair
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
//    recommendations: add high-level documentation, improve doxygen documentation 
*/
#ifndef __asl_ConduitAcceptor_included
#define __asl_ConduitAcceptor_included

#include "asl_ipc_settings.h"

#include "ConduitServer.h"
#include <asl/base/Ptr.h>
#include <asl/base/Logger.h>
#include <algorithm>
#include <list>

#define DB(x) x;

namespace asl {
    
/*! \addtogroup aslipc */
/* @{ */

//! Listens on an endpoint and spawns new servers when clients request a new connection. 
/** 
    The acceptor has its own listener thread. When a client connects, a new ConduitServer
    is created to handle the connection. Each server also has its own thread.
**/
template <class POLICY>
class ConduitAcceptor {
    public:
        typedef typename ConduitServer<POLICY>::Ptr (*FACTORYPROC) (typename POLICY::Handle);
        typedef std::list<typename ConduitServer<POLICY>::Ptr> ServerList;
        /// create a new acceptor to accept incoming connections.
        /** @param theLocalEndpoint the endpoint to listen on
          * @param theFactoryMethod factory used to create new servers
          * @param theMaxConnectionCount maximum simutaneous clients
          **/
        ConduitAcceptor(typename POLICY::Endpoint theLocalEndpoint, 
                        FACTORYPROC theFactoryMethod, 
                        unsigned theMaxConnectionCount=32)
            :
            _myThread(0),
            _myMaxConnectionCount(theMaxConnectionCount),
            _myLocalEndpoint(theLocalEndpoint),
            _createServerProc(theFactoryMethod)
            {
                _myListenHandle = POLICY::startListening(_myLocalEndpoint, _myMaxConnectionCount);
            };
        virtual ~ConduitAcceptor() {
            DB(AC_TRACE << "ACCEPTOR: dtor\n");
            if (_myThread) {
                stop();
            }
            POLICY::stopListening(_myListenHandle);
            DB(AC_TRACE << "ACCEPTOR: dtor\n");
        }
        /** start listening for incoming connections */
        bool start() {
            return pthread_create(&_myThread, NULL, mainAcceptorLoop, (void *)this) == 0;
        }
        /** stop listening for incoming connections. Existing servers are stopped. */
        bool stop() {
            pthread_cancel(_myThread);
            bool mySuccessFlag = !pthread_join(_myThread, NULL);
            _myThread = 0;
            return mySuccessFlag;
        }

        /** @returns number of active servers. */
        int getActiveServerCount() const {
            return _myServers.size();
        }

    protected:
        ServerList _myServers;

    private:
        ConduitAcceptor();
        ConduitAcceptor(const ConduitAcceptor<POLICY> & theOther);
        ConduitAcceptor<POLICY> & operator=(const ConduitAcceptor<POLICY> & theOther);

        pthread_t _myThread;
        const unsigned _myMaxConnectionCount;
        typename POLICY::Endpoint _myLocalEndpoint;
        typename POLICY::Handle _myListenHandle;
        // factory method pointer
        FACTORYPROC _createServerProc;
        
        static bool isBroken(typename ConduitServer<POLICY>::Ptr & S) {
            return !S->isValid();
        }
       
        void stopAllServers() {
            DB(AC_TRACE << "ACCEPTOR: stopping all servers\n");
			//int myServerCount = _myServers.size();
            for (typename ServerList::iterator i = _myServers.begin(); i != _myServers.end(); ++i) {
                (*i)->stop();
            }
            _myServers.clear();
            DB(AC_TRACE << "ACCEPTOR: servers stopped\n");
        }
        
        void processNewConnections(int theTimeout) {
            typename POLICY::Handle myServerHandle = POLICY::createOnConnect(
                    _myListenHandle, _myMaxConnectionCount, theTimeout);
            //DB(AC_TRACE << " createOnConnect ret " << myServerHandle << std::endl);
            if (myServerHandle) {
                typename ConduitServer<POLICY>::Ptr 
                        myNewServer(_createServerProc(myServerHandle));
                myNewServer->setSelf(myNewServer);
				myNewServer->setAcceptor(this);
                _myServers.push_back(myNewServer);
                myNewServer->start();
            }
            //DB(AC_TRACE << "ACCEPTOR: purge invalid servers" << std::endl);
            _myServers.remove_if(isBroken);
            //DB(AC_TRACE << "ACCEPTOR: still active servers:" << _myServers.size() << std::endl);
        };
        
        static void * mainAcceptorLoop(void * theThisPointer) {
            int myResult = 0;
            int myOldCancelState = 0;
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &myOldCancelState);
            pthread_cleanup_push(onThreadDone, theThisPointer);
            DB(AC_TRACE << "ACCEPTOR: Thread:: init" << std::endl);
            ConduitAcceptor<POLICY> * mySelf = 
                reinterpret_cast<ConduitAcceptor<POLICY>*>(theThisPointer);
            for(;;) {
                try {
                    pthread_testcancel();
                    mySelf->processNewConnections(100);
                } catch (ConduitException & ex) {
                    AC_ERROR << ex << std::endl;
                }
            }
            pthread_cleanup_pop(1);
            pthread_setcancelstate(myOldCancelState,0);
            DB(AC_TRACE << "ACCEPTOR: Exiting normally" << std::endl);
            return (void*)static_cast<ptrdiff_t>(myResult);
        }
        
        static void onThreadDone(void * theThisPointer) {
            ConduitAcceptor<POLICY> * mySelf = 
                reinterpret_cast<ConduitAcceptor<POLICY>*>(theThisPointer);
            mySelf->stopAllServers();
            DB(AC_TRACE << "ACCEPTOR: cleanup" << std::endl);
        }

};


/* @} */
}

#undef DB

#endif
