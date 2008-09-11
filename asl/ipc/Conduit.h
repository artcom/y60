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
#ifndef __asl_Conduit_included_
#define __asl_Conduit_included_

#include "ConduitPolicy.h"

#include <asl/base/Exception.h>
#include <asl/base/Ptr.h>
#include <asl/base/Time.h>
#include <asl/base/Block.h>

namespace asl {

/*! \addtogroup aslipc */
/* @{ */

//! Basic Conduit. Can be used as-is for a non-threaded, client conduit. 
template <class POLICY>
class Conduit {
    public:
        typedef asl::Ptr<Conduit> Ptr;
        typedef asl::WeakPtr<Conduit> WPtr;

        /// Create a new conduit, connecting to a given remote endpoint
        Conduit(typename POLICY::Endpoint theRemoteEndpoint) {
            _myHandle = POLICY::connectTo(theRemoteEndpoint);
        };

        virtual ~Conduit() {
            disconnect();
        }

        /// disconnect from the remote endpoint 
        void disconnect() {
            if (_myHandle) {
                POLICY::disconnect(_myHandle);
            }
        }

        /// must be called periodically to handle I/O. 
        virtual bool handleIO(int theTimeout = 0) {
            if (!isValid()) {
                return false;
            }
            return POLICY::handleIO(_myHandle, _myInQueue, _myOutQueue, theTimeout); 
        }
        /// Send all outgoing data 
        virtual bool flush(int theTimeout) {
            Time myTimer;
            long long myEndTime = myTimer.millis() + theTimeout;
            long long myTimeLeft = myEndTime - myTimer.millis();
            while (!_myOutQueue.empty() && myTimeLeft > 0 ) {
                handleIO(static_cast<int>(myTimeLeft));
                myTimer.setNow();
                long long myTimeLeft = myEndTime - myTimer.millis();
            }
            return _myOutQueue.empty();
        }
        
        /// check status of the conduit.
        ///@returns true if the conduit is still operational, otherwise false.
        bool isValid() const {
            return POLICY::isValid(_myHandle);
        }

        /** @name sending and receiving raw data.
         */
        //@{
        
        /// receive raw data (no framing)
        virtual bool receiveData(CharBuffer & theBuffer) {
            if (!handleIO()) {
                return false;
            }
            if (_myInQueue.empty()) {
                return false;
            }
            theBuffer = *(_myInQueue.front());
            _myInQueue.pop_front();
            return true; 
        }

        /// send raw data (no framing)
        virtual bool sendData(const char * theBuffer, unsigned theSize) {
            return sendData(CharBuffer(theBuffer, theBuffer+theSize));
        }

        /// send raw data (no framing)
        virtual bool sendData(const CharBuffer & theBuffer) {
            return sendData(CharBufferPtr(new CharBuffer(theBuffer)));
        }

        /// send raw data (no framing)
        virtual bool sendData(const CharBufferPtr & theBuffer) {
            _myOutQueue.push_back(theBuffer);
            if (!handleIO()) {
                return false;
            }
            return true;
        }
        //@}

        /** @name sending and receiving framed data.
         * Each frame starts with a one-byte (if smaller than 255 octets) or a 5-byte header (0xff + Unsigned32) 
         * followed by the payload.
         */
        //@{

        /// sends a string as a block (framing). 
        /**The other side should use a framing call to receive the data */
        virtual bool send(const std::string & theString) {
            const unsigned char * myStart = reinterpret_cast<const unsigned char *>(theString.c_str()); 
            return send(Block(myStart, myStart+theString.length()));
        }
        
        /// sends a block of data (framing).
        /**The other side should use a framing call to receive the data */
        virtual bool send(const ReadableBlock & theBlock) {
            asl::Unsigned32 mySize = theBlock.size();
            CharBufferPtr myBuffer(new CharBuffer(mySize+5)); 
            char * myDataPos = 0;
            if (mySize < 0xff) {
                myBuffer->resize(1+mySize);
                (*myBuffer)[0] = ((unsigned char)(mySize));
                myDataPos = &(*myBuffer)[1];
            } else {
                myBuffer->resize(1+sizeof(mySize)+mySize);
                (*myBuffer)[0] = (unsigned char)(0xff);
                memcpy(&(*myBuffer)[1], &mySize, sizeof(mySize));
                myDataPos = &(*myBuffer)[1+sizeof(mySize)];
            }
            theBlock.readBytes(myDataPos, mySize, 0);
            return sendData(myBuffer);
        }

        /// receives a string (framing).
        /**The other side should use a framing call to receive the data */
        virtual bool receive(std::string & myReceivedString, int theTimeout) {
            Time myTimer;
            long long myEndTime = myTimer.millis() + theTimeout;
            long long myTimeLeft = myEndTime - myTimer.millis();
            bool hasReceived = receive(myReceivedString); 
            while (!hasReceived && myTimeLeft > 0 ) {
                handleIO(static_cast<int>(myTimeLeft));
                hasReceived = receive(myReceivedString); 
                myTimer.setNow();
                long long myTimeLeft = myEndTime - myTimer.millis();
            }
            return hasReceived;
        }
        /// receives a string (framing).
        /**The other side should use a framing call to receive the data */
        virtual bool receive(std::string & myReceivedString) {
            Block myInBlock(0);
            if (receive(myInBlock)) {
                myReceivedString.assign(reinterpret_cast<const char*>(myInBlock.begin()),
                        size_t(myInBlock.size()));
                return true;
            }
            return false;
        }
        
        /// receives a Block of data (framing).
        /**The other side should use a framing call to receive the data */
        virtual bool receive(ResizeableBlock & myReceivedBlock) {
            bool dataReceived = false;
            CharBufferPtr myStringBuffer(new CharBuffer(0));
            CharBuffer myTempBuffer(0);
            while (receiveData(myTempBuffer)) {
                myStringBuffer->insert(myStringBuffer->end(), myTempBuffer.begin(), myTempBuffer.end());
                if (myStringBuffer->empty()) {
                    continue;
                }
                char * myStringBegin = &(*myStringBuffer)[0];
                // peek block size
                asl::Unsigned32 myBlockSize = *(reinterpret_cast<unsigned char*>(myStringBegin++));
                int myHeaderSize = 1;
                if (myBlockSize == 0xFF) {
                    if (myStringBuffer->size() < 1+sizeof(myBlockSize)) {
                        // we didn't even received the length field,
                        // go back for more data
                        continue;
                    }
                    myBlockSize = *(reinterpret_cast<asl::Unsigned32*>(myStringBegin));
                    myStringBegin += sizeof(myBlockSize);
                    myHeaderSize = sizeof(myBlockSize)+1;
                }
                // do we have at least myBlockSize bytes following the length header?
                if (myHeaderSize+myBlockSize <= myStringBuffer->size()) {
                    myReceivedBlock.append(myStringBegin, myBlockSize);
                    myStringBuffer->erase(myStringBuffer->begin(), myStringBuffer->begin()+myBlockSize+myHeaderSize);
                    dataReceived = true;
                    break;
                }
            }
            // re-insert the rest of the data back onto the queue
            if (!myStringBuffer->empty()) {
                _myInQueue.push_front(myStringBuffer);
            }
            return dataReceived;
        }
        //@}
       
    protected:
        /// create a new conduit using an already connected I/O handle
        Conduit(typename POLICY::Handle theHandle) : _myHandle(theHandle) {

        }

        Conduit(const Conduit<POLICY> &);
        Conduit & operator=(const Conduit<POLICY> &);
        typename POLICY::Handle _myHandle;
        BufferQueue _myInQueue;
        BufferQueue _myOutQueue;
};

/* @} */
}
#endif

