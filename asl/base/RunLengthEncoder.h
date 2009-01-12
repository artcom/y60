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
//   $RCSfile: RunLengthEncoder.h,v $
//
//   $Revision: 1.6 $
//
//   Description: Run Length Encoder for memory blocks
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_RunLengthEncoder_
#define _included_asl_RunLengthEncoder_

#include "asl_base_settings.h"

#include <cstring>

#include "Block.h"
#include "Exception.h"


/*! \addtogroup aslbase */
/* @{ */

/*
    Sample usage:

    // Adapt a block to a given bitmap
    asl::ReadableBlockAdapter myImageBlock(theBitmap.getData(), theBitmap.getData() +
        theBitmap.getWidth() * theBitmap.getHeight() * theBitmap.getBytesPerPixel());

    // Create a result container
    asl::Block myCompressedImageBlock;

    // Compress it
    asl::RLE::compress(myImageBlock, myCompressedImageBlock, theImage.getBytesPerPixel());

    // Create a uncompress result container
    asl::Block myUncompressedBlock;
    asl::RLE::uncompress(myImageBlock, myUncompressedBlock);
*/

namespace asl {

DEFINE_EXCEPTION(RunLengthEncodingException, Exception);

class RLE {
    public:
        /*
            Encodes a number with continuation bits as variable length byte sequence
            and appends it to theBuffer
        */
        static
        void writeNumber(asl::VariableCapacityBlock & theBuffer, unsigned long theNumber) {
            do {
                unsigned char myByte = static_cast<unsigned char>(theNumber % 128);
    		    theNumber /= 128;
                if (theNumber > 0) {
                    myByte |= 128;
                }
                theBuffer.appendUnsigned8(myByte);
            } while (theNumber > 0);
        }

        /*
            Reads an encoded number form theBufferIt and advances the buffer iterator to the next data.
        */
        static
        unsigned long readNumber(const unsigned char * & theBufferIt) {
            unsigned long myResult = 0;
            unsigned long myPower  = 1;
            unsigned char myChar   = 0;

            do {
                myChar = *(theBufferIt++);
                myResult += (myChar & 127) * myPower;
                myPower  *= 128;
            } while (myChar & 128);

            return myResult;
        }

        /*
            Compresses a readable block (theData) into a result block (theResult).
            If you encode datagrams other than bytes, you should provide theDatagrammSize, e.g. for RGB-Pixel encoding
            supply a datagram size of 3.
            There is also a template version below which is about twice as fast as this version.
        */
        static
        void compress(const asl::ReadableBlock & theData, asl::VariableCapacityBlock & theResult, unsigned theDatagramSize = 1) {
            if ((theData.size() % theDatagramSize) != 0) {
                throw RunLengthEncodingException(std::string("Data size has to be a multiple of theDatagramSize (theData.size(): ") +
                    asl::as_string(theData.size()) + " theDatagramSize: " + asl::as_string(theDatagramSize) + ")", "RLE::compress()");
            }

    	    theResult.reserve(theData.size());

            // Write header with original data size and datagram size
            writeNumber(theResult, theData.size());
            writeNumber(theResult, theDatagramSize);

            const unsigned char * myDataIt = theData.begin();
            const unsigned char * myEnd    = theData.end();

    	    if (theData.size() <= theDatagramSize * 2) {
                addCode(true, 2, theResult);
                theResult.append(myDataIt, 2 * theDatagramSize);
                return;
    	    }

    	    // Bootstrap
    	    bool myDirtyFlag             = (memcmp(myDataIt, myDataIt + theDatagramSize, theDatagramSize) != 0);
    	    unsigned mySequenceLength    = 2;
            myDataIt += theDatagramSize;

    	    while (myDataIt + theDatagramSize != myEnd) {
    		    if (myDirtyFlag) {
                    if (memcmp(myDataIt, myDataIt + theDatagramSize, theDatagramSize) != 0) {
    				    ++mySequenceLength;
    			    } else {
    				    addCode(true, mySequenceLength - 1, theResult);
                        theResult.append(myDataIt - (mySequenceLength - 1) * theDatagramSize, (mySequenceLength - 1) * theDatagramSize);
    				    mySequenceLength = 2;
    				    myDirtyFlag = false;
    			    }
    		    } else {
    			    if (memcmp(myDataIt, myDataIt + theDatagramSize, theDatagramSize) == 0) {
    				    ++mySequenceLength;
    			    } else {
    				    addCode(false, mySequenceLength, theResult);
    				    theResult.append(myDataIt, theDatagramSize);
                        myDataIt += theDatagramSize;
    				    mySequenceLength = 1;
    				    if (myDataIt + theDatagramSize != myEnd) {
                            ++mySequenceLength;
                            if (memcmp(myDataIt, myDataIt + theDatagramSize, theDatagramSize) != 0) {
    						    myDirtyFlag = true;
    					    }
                        } else {
                            break;
                        }
    			    }
    		    }
                myDataIt += theDatagramSize;
    	    }

    	    if (mySequenceLength > 0) {
    		    addCode(myDirtyFlag, mySequenceLength, theResult);

    		    if (myDirtyFlag) {
                    theResult.append(myDataIt - (mySequenceLength - 1) * theDatagramSize, mySequenceLength * theDatagramSize);
    		    } else {
    			    theResult.append(myDataIt, theDatagramSize);
    		    }
    	    }
        }

        /*
            Compresses a readable block (theData) into a result block (theReuslt).
            The template parameter is used to determine the size of a datagram.
            If you use datagrams other than char/short/unsigned (i.e. 3 byte pixels), you must
            provide your own pixel class with comparison and assignment operator or use the compress-version above.
            This version is about 2x as fast as the version above for native types.
        */
        template <class T>
        static
        void compress(const asl::ReadableBlock & theData, asl::Block & theResult) {
            unsigned myDatagramSize = sizeof(T);
            if ((theData.size() % myDatagramSize) != 0) {
                throw RunLengthEncodingException(std::string("Data size has to be a multiple of theDatagramSize (theData.size(): ") +
                    asl::as_string(theData.size()) + " theDatagramSize: " + asl::as_string(myDatagramSize) + ")", "RLE::compress()");
            }

    	    theResult.reserve(theData.size());

            // Write header with original data size and datagram size
            writeNumber(theResult, theData.size());
            writeNumber(theResult, myDatagramSize);

            const T * myDataIt = reinterpret_cast<const T *>(theData.begin());
            const T * myEnd    = reinterpret_cast<const T *>(theData.end());

            if (theData.size() <= myDatagramSize * 2) {
                addCode(true, 2, theResult);
                theResult.append(myDataIt, 2 * myDatagramSize);
                return;
    	    }

    	    // Bootstrap
            T myPreviousByte           = *(myDataIt++);
            T myByte                   = *(myDataIt++);
    	    bool myDirtyFlag           = (myPreviousByte != myByte);
    	    unsigned mySequenceLength  = 2;
            myPreviousByte             = myByte;

    	    while (myDataIt != myEnd) {
                myByte = *(myDataIt++);
    		    if (myDirtyFlag) {
    			    if (myByte != myPreviousByte) {
    				    ++mySequenceLength;
    			    } else {
    				    addCode(true, mySequenceLength - 1, theResult);
                        theResult.append(myDataIt - mySequenceLength - 1, (mySequenceLength - 1) * myDatagramSize);
    				    mySequenceLength = 2;
    				    myDirtyFlag = false;
    			    }
    		    } else {
    			    if (myByte == myPreviousByte) {
    				    ++mySequenceLength;
    			    } else {
    				    addCode(false, mySequenceLength, theResult);
    				    theResult.appendData(myPreviousByte);

    				    mySequenceLength = 1;
    				    if (myDataIt != myEnd) {
                            ++mySequenceLength;
                            myPreviousByte = myByte;
                            myByte = *(myDataIt++);
                            if (myByte != myPreviousByte) {
    						    myDirtyFlag = true;
    					    }
    				    }
    			    }
    		    }
    		    myPreviousByte = myByte;
    	    }

    	    if (mySequenceLength > 0) {
    		    addCode(myDirtyFlag, mySequenceLength, theResult);
    		    if (myDirtyFlag) {
                    theResult.append(myDataIt - mySequenceLength, mySequenceLength * myDatagramSize);
    		    } else {
    			    theResult.appendData(myByte);
    		    }
    	    }
        }

        /*
           Uncompresses a compressed block (theData) in a result block (theResult).
           The datagram size is encoded inside the block.
           This is very speedy (around 150 MB per second)
        */
        static
        void uncompress(const asl::ReadableBlock & theData, asl::VariableCapacityBlock & theResult,
                unsigned long theResultOffset = 0) {
    	    const unsigned char * myDataIt = theData.begin();

            // Uncompressed data size is written to the start of the compressed data
            theResult.resize(theResultOffset+readNumber(myDataIt));
            unsigned myDatagramSize = readNumber(myDataIt);

      	    unsigned char * myResultIt = theResult.begin() + theResultOffset;
            unsigned long myLength = 0;
            const unsigned char * myEnd = theData.end();

            while (myDataIt != myEnd) {
    		    if (readCode(myDataIt, myLength)) {
                    unsigned myByteLength = myLength * myDatagramSize;
                    std::copy(myDataIt, myDataIt + myByteLength, myResultIt);
                    myDataIt   += myByteLength;
                    myResultIt += myByteLength;
    		    } else {
                    for (unsigned i = 0; i < myLength; ++i) {
                        for (unsigned j = 0; j < myDatagramSize; ++j) {
                            *(myResultIt++) = myDataIt[j];
                        }
                    }
                    myDataIt += myDatagramSize;
    		    }
    	    }
        }

    private:
        static
        void addCode(bool theDirtyFlag, unsigned long theLength, asl::VariableCapacityBlock & theResult) {
            theLength *= 2;
            if (theDirtyFlag) {
                theLength |= 1;
            }
            writeNumber(theResult, theLength);
        }

        // Returns true if theCode has dirty flag set
        static
        bool readCode(const unsigned char * & theCode, unsigned long & theLength) {
            unsigned long myDecodedNumber = readNumber(theCode);
            theLength = myDecodedNumber / 2;
            return (myDecodedNumber & 1);
        }
    };
}

/* @} */


#endif
