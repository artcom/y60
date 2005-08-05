//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Conduit.h,v $
//   $Author: david $
//   $Revision: 1.12 $
//   $Date: 2004/11/22 11:03:54 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================

#ifndef __asl_Conduit_included_
#define __asl_Conduit_included_

#include "ConduitPolicy.h"

#include <asl/Exception.h>
#include <asl/Ptr.h>
#include <asl/Time.h>
#include <asl/Block.h>

namespace asl {

/*! \addtogroup aslipc */
/* @{ */

template <class POLICY>
class Conduit {
    public:
        typedef asl::Ptr<Conduit> Ptr;
        typedef asl::WeakPtr<Conduit> WPtr;

        Conduit(typename POLICY::Endpoint theRemoteEndpoint) : _myValidFlag(false) {
            _myHandle = POLICY::connectTo(theRemoteEndpoint);
            _myValidFlag = true;
        };

        virtual ~Conduit() {
            disconnect();
        }

        void disconnect() {
            if (_myHandle) {
                POLICY::disconnect(_myHandle);
            }
        }

        virtual bool handleIO(int theTimeout = 0) {
            if (_myValidFlag) {
                _myValidFlag = POLICY::handleIO(_myHandle, _myInQueue, _myOutQueue, theTimeout); 
            }
            return _myValidFlag;
        }
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

        /// sends a string as a block (framing) 
        virtual bool send(const std::string & theString) {
            const unsigned char * myStart = reinterpret_cast<const unsigned char *>(theString.c_str()); 
            return send(Block(myStart, myStart+theString.length()));
        }
        
        /// sends a string as a block (framing) 
        virtual bool send(const ReadableBlock & theBlock) {
            unsigned long mySize = theBlock.size();
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

        /// receives a string (framing) 
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
        /// receives a string 
        virtual bool receive(std::string & myReceivedString) {
            Block myInBlock(0);
            if (receive(myInBlock)) {
                myReceivedString.assign(reinterpret_cast<const char*>(myInBlock.begin()),
                        size_t(myInBlock.size()));
                return true;
            }
            return false;
        }
        
        /// receives a Block of data. The new data is appended to myReceivedBlock
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
                unsigned long myBlockSize = *(reinterpret_cast<unsigned char*>(myStringBegin++));
                int myHeaderSize = 1;
                if (myBlockSize == 0xFF) {
                    if (myStringBuffer->size() < 1+sizeof(myBlockSize)) {
                        // we didn't even received the length field,
                        // go back for more data
                        continue;
                    }
                    myBlockSize = *(reinterpret_cast<unsigned long*>(myStringBegin));
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
       
        bool isValid() const {
            return _myValidFlag;
        }
    protected:
        /// create a new conduit using an already connected I/O handle
        Conduit(typename POLICY::Handle theHandle) : _myHandle(theHandle) {
            _myValidFlag = (bool)theHandle;
        };

        Conduit(const Conduit<POLICY> &);
        Conduit & operator=(const Conduit<POLICY> &);
        typename POLICY::Handle _myHandle;
        BufferQueue _myInQueue;
        BufferQueue _myOutQueue;
        bool _myValidFlag;
};

/* @} */
}
#endif

