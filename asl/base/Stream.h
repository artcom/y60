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
//    $RCSfile: Stream.h,v $
//
//   $Revision: 1.8 $
//
// Description:  Memory Block handling functions; consider to use them whenever
//               you think about using memcpy; here you get typesafe conversion,
//               read-write control, convenient windows api interfaces and
//               also a copy-on-write (COW) implementation if you want.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_Stream_
#define _included_asl_Stream_

#include "asl_base_settings.h"

#include "Exception.h"
#include <asl/base/begin_end.h>
#include "string_functions.h"

#include "Ptr.h"
#include "Path.h"
#include "Enum.h"

#include <fstream>
#include <vector>
#include <algorithm>

namespace asl {

    ASL_BASE_DECL off_t getFileSize(const std::string& theFileName);

    /*! \addtogroup aslbase */
    /* @{ */
    
    DEFINE_EXCEPTION(StreamException,Exception);
	DEFINE_EXCEPTION(StreamPositionMismatch, Exception);
	DEFINE_EXCEPTION(StreamReadFailed, Exception);
	DEFINE_EXCEPTION(StreamWriteFailed, Exception);
	DEFINE_EXCEPTION(StreamReadNumberTooLargeForVariable, Exception);
	DEFINE_EXCEPTION(StreamWriteNumberTooLargeForVariable, Exception);

    template <class InternalFormat, class ExternalFormat>
    class BinaryArranger;

    union Number32 {
	Float32 asFloat;
	Unsigned32 asUnsigned;
    };
    union Number64 {
	Float64 asFloat;
	Unsigned64 asUnsigned;
    };
    
    class SameOrder {
    public:
        typedef SameOrder self;
        template <class T>
        static T get(T theNumber) {
            return theNumber;
        }
        static Float32 getFloatIn(Unsigned32 theNumber) {
	    Number32 myNumber;
	    myNumber.asUnsigned = theNumber;
            return myNumber.asFloat;
        }
        static Float64 getFloatIn(Unsigned64 theNumber) {
	    Number64 myNumber;
	    myNumber.asUnsigned = theNumber;
            return myNumber.asFloat;
        }
        static Unsigned32 getFloatOut(Float32 theNumber) {
	    Number32 myNumber;
	    myNumber.asFloat = theNumber;
            return myNumber.asUnsigned;
        }
        static Unsigned64 getFloatOut(Float64 theNumber) {
	    Number64 myNumber;
	    myNumber.asFloat = theNumber;
            return myNumber.asUnsigned;
        }
    };

#define SWAP_BYTE_ORDER_16(x) (((x)&0xff)<<8 | ((x)&0xff00)>>8)
#define SWAP_BYTE_ORDER_32(x) (((x)&0xff)<<24 | ((x)&0xff00)<<8 | ((x)&0xff0000)>>8 | ((x)&0xff000000)>>24)
#define SWAP_BYTE_ORDER_64(x) (((x)&0xffULL)<<56 | ((x)&0xff00ULL)<<40 | ((x)&0xff0000ULL)<<24 | ((x)&0xff000000ULL)<<8 |\
       ((x)&0xff00000000ULL)>>8 | ((x)&0xff0000000000ULL)>>24 | ((x)&0xff000000000000ULL)>>40 | ((x)&0xff00000000000000ULL)>>56)

    class ReverseOrder {
    public:
        typedef ReverseOrder self;
        static Float32 getFloatIn(Unsigned32 theNumber) {
	    Number32 myNumber;
	    myNumber.asUnsigned = SWAP_BYTE_ORDER_32(theNumber);
            return myNumber.asFloat;
        }
        static Float64 getFloatIn(Unsigned64 theNumber) {
	    Number64 myNumber;
	    myNumber.asUnsigned = SWAP_BYTE_ORDER_64(theNumber);
            return myNumber.asFloat;
        }
        static Unsigned32 getFloatOut(Float32 theNumber) {
	    Number32 myNumber;
	    myNumber.asFloat = theNumber;
            return SWAP_BYTE_ORDER_32(myNumber.asUnsigned);
        }
        static Unsigned64 getFloatOut(Float64 theNumber) {
	    Number64 myNumber;
	    myNumber.asFloat = theNumber;
            return SWAP_BYTE_ORDER_64(myNumber.asUnsigned);
        }
        static Signed16 get(Signed16 theNumber) {
            return Signed16(SWAP_BYTE_ORDER_16(theNumber));
        }
        static Unsigned16 get(Unsigned16 theNumber) {
            return (Unsigned16)(SWAP_BYTE_ORDER_16(theNumber));
        }
        static Signed32 get(Signed32 theNumber) {
            return Signed32(SWAP_BYTE_ORDER_32(theNumber));
        }
        static Unsigned32 get(Unsigned32 theNumber) {
            return (Unsigned32)(SWAP_BYTE_ORDER_32(theNumber));
        }
        static long get(long theNumber) {
            return long(SWAP_BYTE_ORDER_32(theNumber));
        }
        static unsigned long get(unsigned long theNumber) {
            return (unsigned long)(SWAP_BYTE_ORDER_32(theNumber));
        }
        static Signed64 get(Signed64 theNumber) {
            return (Signed64)(SWAP_BYTE_ORDER_64(theNumber));
        }
        static Unsigned64 get(Unsigned64 theNumber) {
            return (Unsigned64)(SWAP_BYTE_ORDER_64(theNumber));
        }
     };

    template <>
    class BinaryArranger<X86ByteOrder, X86ByteOrder> : public SameOrder {};
    template <>
    class BinaryArranger<X86ByteOrder, PowerPCByteOrder> : public ReverseOrder {};
    template <>
    class BinaryArranger<PowerPCByteOrder, X86ByteOrder> : public ReverseOrder {};
    template <>
    class BinaryArranger<PowerPCByteOrder, PowerPCByteOrder> : public SameOrder {};


    //-------------------------------------------------------------------------------
    /** 
        ReadableBlock is an interface class.
        It provides an interface for read access to raw byte data
    **/
    class WriteableBlock;

    template <class ARRANGER, class TN, unsigned SIZE>
    struct NumberReader;

    template <class TN>
    struct NumberReader<ReverseOrder,TN, 2> {
        static void swapBytes(TN & theDest) {
            theDest = SWAP_BYTE_ORDER_16(theDest);
        }
    };
    template <class TN>
    struct NumberReader<ReverseOrder,TN, 4> {
        static void swapBytes(TN & theDest) {
            *reinterpret_cast<Unsigned32*>(&theDest) = SWAP_BYTE_ORDER_32(*reinterpret_cast<Unsigned32*>(&theDest));
        }
    };
    template <class TN>
    struct NumberReader<ReverseOrder,TN, 8> {
        static void swapBytes(TN & theDest) {
            *reinterpret_cast<Unsigned64*>(&theDest) = SWAP_BYTE_ORDER_64(*reinterpret_cast<Unsigned64*>(&theDest));
        }
    };
    template <>
    struct NumberReader<ReverseOrder,Float32, 4> {
        static void swapBytes(Float32 & theDest) {
	    Number32 myTmp;
	    myTmp.asFloat = theDest;
	    myTmp.asUnsigned = SWAP_BYTE_ORDER_32(myTmp.asUnsigned);
	    theDest = myTmp.asFloat; 
        }
    };
    template <>
    struct NumberReader<ReverseOrder,Float64, 8> {
        static void swapBytes(Float64 & theDest) {
	    Number64 myTmp;
	    myTmp.asFloat = theDest;
	    myTmp.asUnsigned = SWAP_BYTE_ORDER_64(myTmp.asUnsigned);
	    theDest = myTmp.asFloat; 
        }
    };

    template <class TN>
    struct NumberReader<SameOrder,TN, 2> {
        static void swapBytes(TN & theDest) {}
    };
    template <class TN>
    struct NumberReader<SameOrder,TN, 4> {
        static void swapBytes(TN & theDest) {}
    };
    template <class TN>
    struct NumberReader<SameOrder,TN, 8> {
        static void swapBytes(TN & theDest) {}
    };

    template <class EXTERNAL_BYTE_ORDER, class SIZE_TYPE=AC_SIZE_TYPE, class OFFSET_TYPE=AC_OFFSET_TYPE>
    class ReadableArrangedStream {
    public:
        typedef SIZE_TYPE size_type;
        typedef OFFSET_TYPE offset_type;
        typedef BinaryArranger<AC_HOST_BYTE_ORDER, EXTERNAL_BYTE_ORDER> Arranger;
        typedef BinaryArranger<AC_HOST_BYTE_ORDER, BigEndianByteOrder> CountArranger;

        /// returns the total number of bytes in the object
        virtual size_type size() const = 0;

        /// copies sizeof(TX) bytes to theDest starting at theOffset
        virtual size_type readBytes(void * theDest, size_type theSize, size_type theReadOffset) const = 0;
        
        /// read specialized for asl::Bitsets. Arch independent data size 
        template <class T>
        size_type readData(asl::Bitset<T> & theDest, size_type theReadOffset) const {
            typename asl::Bitset<T>::int_type myValue;
            size_type myResult = readBytes(&myValue, sizeof(myValue), theReadOffset);
            NumberReader<typename Arranger::self, typename asl::Bitset<T>::int_type, 
                            sizeof(typename asl::Bitset<T>::int_type)>::swapBytes(myValue);
            theDest = myValue;
            return myResult;
        }

        /// copies sizeof(TX) bytes to theDest starting at theOffset
        template <class TX>
        size_type readData(TX & theDest, size_type theReadOffset, size_type theSize=0) const {
            return readBytes(&theDest, theSize ? theSize : sizeof(theDest), theReadOffset);
        }

        template <class TN>
        size_type readNumber(TN & theDest, size_type theReadOffset) const {
            size_type myResult = readBytes(&theDest, sizeof(theDest), theReadOffset);
            NumberReader<typename Arranger::self, TN, sizeof(TN)>::swapBytes(theDest);
            return myResult;
        }

        /// reads a theNumberSize-length big endian integer from stream
        template <class TN>
        size_type readPackedNumber(TN & theDest, size_type theReadOffset, size_type theNumberSize) const {
            if (theNumberSize <= sizeof(theDest)) {
                theDest = 0;
                size_type myResult = readBytes((char*)(&theDest) + sizeof(theDest) - theNumberSize, theNumberSize, theReadOffset);
                NumberReader<typename CountArranger::self, TN, sizeof(TN)>::swapBytes(theDest);
                return myResult;
            }
            throw StreamReadNumberTooLargeForVariable(JUST_FILE_LINE);
        }

        /// reads a theNumberSize-length big endian integer from stream
        template <class TN>
        size_type readPackedSignedNumber(TN & theDest, size_type theReadOffset, size_type theNumberSize) const {
            if (theNumberSize <= sizeof(theDest)) {
                theDest = 0;
                char * myStartDestPos = (char*)(&theDest) + sizeof(theDest) - theNumberSize;
                size_type myResult = readBytes(myStartDestPos, theNumberSize, theReadOffset);
                if (*myStartDestPos & 0x80) {
                    // extend negative bits
                    std::fill((char *)(&theDest), myStartDestPos, 0xff);
                }
                NumberReader<typename CountArranger::self, TN, sizeof(TN)>::swapBytes(theDest);
                return myResult;
            }
            throw StreamReadNumberTooLargeForVariable(JUST_FILE_LINE);
        }

        virtual size_type readBlock(WriteableBlock & theDest, size_type theReadOffset) const;

        size_type readSize(size_type & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }

        size_type readUnsigned(Unsigned64 & theDest, size_type theReadOffset) const {
            Unsigned8 mySmallCount;
            theReadOffset = readUnsigned8(mySmallCount, theReadOffset);
            if (mySmallCount >= 0xF0) {
                Unsigned8 myLen = mySmallCount - 0xEF;
                theReadOffset = readPackedNumber(theDest, theReadOffset, myLen);
            } else {
                theDest = mySmallCount;
            }
            return theReadOffset;
        }
       size_type readUnsigned(Unsigned32 & theDest, size_type theReadOffset) const {
            Unsigned8 mySmallCount;
            theReadOffset = readUnsigned8(mySmallCount, theReadOffset);
            if (mySmallCount >= 0xF0) {
                Unsigned8 myLen = mySmallCount - 0xEF;
                if (myLen <=4) {
                    theReadOffset = readPackedNumber(theDest, theReadOffset, myLen);
                } else {
                    throw StreamReadNumberTooLargeForVariable(JUST_FILE_LINE);
                }
            } else {
                theDest = mySmallCount;
            }
            return theReadOffset;
        }
        size_type readSigned(Signed64 & theDest, size_type theReadOffset) const {
            Signed8 mySmallOffset;
            theReadOffset = readSigned8(mySmallOffset, theReadOffset);
            if (mySmallOffset >= 120 || mySmallOffset <= -121) {
                Unsigned8 myLen = (Unsigned8)mySmallOffset - 119;
                theReadOffset = readPackedSignedNumber(theDest, theReadOffset, myLen);
            } else {
                theDest = mySmallOffset;
            }
            return theReadOffset;
        }
        size_type readSigned(Signed32 & theDest, size_type theReadOffset) const {
            Signed8 mySmallOffset;
            theReadOffset = readSigned8(mySmallOffset, theReadOffset);
            if (mySmallOffset >= 120 || mySmallOffset <= -121) {
                Unsigned8 myLen = (Unsigned8)mySmallOffset - 119;
                if (myLen <=4) {
                    theReadOffset = readPackedSignedNumber(theDest, theReadOffset, myLen);
                } else {
                    throw StreamReadNumberTooLargeForVariable(JUST_FILE_LINE);
                }
            } else {
                theDest = mySmallOffset;
            }
            return theReadOffset;
        }

        size_type readSigned64(Signed64 & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readUnsigned64(Unsigned64 & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readFloat64(Float64 & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readSigned32(Signed32 & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readUnsigned32(Unsigned32 & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readFloat32(Float32 & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readUnsigned16(Unsigned16 & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readSigned16(Signed16 & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readUnsigned8(Unsigned8 & theDest, size_type theReadOffset) const {
            return readData(theDest, theReadOffset);	
        }
        size_type readSigned8(Signed8 & theDest, size_type theReadOffset) const {
            return readData(theDest, theReadOffset);	
        }
        size_type readString(std::string & theDest, size_type theStringSize, size_type theReadOffset) const {
            theDest.resize(theStringSize);
            return readBytes(&theDest[0], theStringSize, theReadOffset);	
        }
        size_type readCountedString(std::string & theDest, size_type theReadOffset) const {
            size_type myStringLength;
            theReadOffset = readUnsigned(myStringLength, theReadOffset);
            return readString(theDest, myStringLength, theReadOffset);	
        }
        virtual operator const void*() const = 0;
        virtual ~ReadableArrangedStream() {};
    };
    typedef ReadableArrangedStream<AC_DEFAULT_BYTE_ORDER> ReadableStream;

    /**
    This adapter allows to change the expected byte-order of a given IN_BYTE_ORDER-Stream to OUT_BYTE_ORDER;
    you should need this only in cases when the byte-order of a stream is unknown at the time of creation;
    by using this adapter you can change the byte order at any time by using a different adapter.
    @Note: the IN_BYTE_ORDER must match the real byteorder of the stream, otherwise string with length field will
    not get treated correctly.
    @Note: It is the formatting functions of OUT_BYTE_ORDER that get used, not those of the supplied stream.
    */
    template <class OUT_BYTE_ORDER, class IN_BYTE_ORDER, class SIZE_TYPE=AC_SIZE_TYPE>
    class ReadableStreamAdapter : public ReadableArrangedStream<OUT_BYTE_ORDER, SIZE_TYPE> {
        typedef ReadableArrangedStream<OUT_BYTE_ORDER, SIZE_TYPE> Base;
    public:        
        ReadableStreamAdapter(ReadableArrangedStream<IN_BYTE_ORDER> & theStream) : _myStream(theStream) {}
        virtual typename Base::size_type size() const {
            return _myStream.size();
        }
        /// copies sizeof(TX) bytes to theDest starting at theOffset
        virtual typename Base::size_type readBytes(void * theDest, typename Base::size_type theSize, typename Base::size_type theReadOffset) const {
            return _myStream.readBytes(theDest, theSize, theReadOffset);
        }
        operator const void*() const {
            return _myStream.operator const void*();
        }
    private:
        ReadableArrangedStream<IN_BYTE_ORDER> & _myStream;
   };

    template <class EXTERNAL_BYTE_ORDER>
    class ReadableArrangedFile : public ReadableArrangedStream<EXTERNAL_BYTE_ORDER> {
        typedef ReadableArrangedStream<EXTERNAL_BYTE_ORDER> Base;
    public:
        ReadableArrangedFile(const std::string & theFileName)
            : _myFileName(theFileName, UTF8),
             _mySize(getFileSize(theFileName)),
            _myPosition(0)
            
        {
            _myInFile.open(_myFileName.toLocale().c_str() ,std::ios::binary);
        }

        typename Base::size_type readBytes(void * theDest, typename Base::size_type theSize, typename Base::size_type theReadOffset) const {
            if (theReadOffset != _myPosition) {
                _myInFile.seekg(theReadOffset);
                _myPosition = _myInFile.tellg();
                if (_myPosition != theReadOffset) {
                    throw StreamPositionMismatch(
                        std::string("ReadableFile::readBytes(size=")+as_string(theSize)+")",
                        std::string("Filename='")+ _myFileName.toLocale() + "', desired pos= " + as_string(theReadOffset) 
                        + ", actual pos = "+ as_string(_myPosition) );
                }
            }
            _myInFile.read((char*)theDest, theSize);
            if ( static_cast<std::size_t>(_myInFile.gcount()) != theSize) {
                throw StreamReadFailed(
                    std::string("ReadableFile::readBytes(size=")+as_string(theSize)+")",
                    std::string("Filename='")+ _myFileName.toLocale() + "', pos = " + as_string(theReadOffset) 
                    + ", actual size read = "+ as_string(_myPosition) );
            }
            _myPosition += _myInFile.gcount();
            return _myPosition;
        }

        virtual typename Base::size_type size() const {
            return _mySize;
        }
        operator const void*() const {
            return _myInFile;
        }
        const std::string & getName() const {
            return _myFileName.toUTF8();
        }
    private:
        // _myFileName must be initialized before the stream
        Path _myFileName;
        typename Base::size_type _mySize;
        mutable std::ifstream _myInFile;
        mutable typename Base::size_type _myPosition;
    };
    typedef ReadableArrangedFile<AC_DEFAULT_BYTE_ORDER> ReadableFile;

    struct ReadableStreamHandle {
        virtual ReadableStream & getStream() = 0;
        virtual const std::string & getName() const = 0;
        virtual ~ReadableStreamHandle() {}
    };

    struct AlwaysOpenReadableFileHandle : public ReadableStreamHandle {
        AlwaysOpenReadableFileHandle(const std::string & theFileName) {
            _myFile = asl::Ptr<ReadableFile>(new ReadableFile(theFileName));
            _myFileName = theFileName;
        } 
        ReadableStream & getStream() {
            return *_myFile;
        }
        const std::string & getName() const {
            return _myFileName;
        }
    private:
        AlwaysOpenReadableFileHandle() {}
        asl::Ptr<ReadableFile> _myFile;
        std::string _myFileName;
    };

    class ReadableBlock;

    template <class EXTERNAL_BYTE_ORDER, class SIZE_TYPE=AC_SIZE_TYPE, class OFFSET_TYPE=AC_OFFSET_TYPE>
    class WriteableArrangedStream {
    public:
        typedef SIZE_TYPE size_type;
        typedef OFFSET_TYPE offset_type;
        typedef BinaryArranger<AC_HOST_BYTE_ORDER, EXTERNAL_BYTE_ORDER> Arranger;
        typedef WriteableArrangedStream<EXTERNAL_BYTE_ORDER, SIZE_TYPE, OFFSET_TYPE> WriteableStream;
        typedef BinaryArranger<AC_HOST_BYTE_ORDER, BigEndianByteOrder> CountArranger;

        WriteableArrangedStream() : _myByteCounter(0) {}

        /// append theSize bytes from theMemory to the WriteableStream
        virtual WriteableStream & append(const void * theMemory, size_type theSize) = 0;

        /// appends all the bytes from theSource at the end of the WriteableStream
        virtual WriteableStream & append(const ReadableBlock & theSource);
        
        template <class T>
        WriteableStream & appendData(const T & thePlainOldData) {
            return append(&thePlainOldData, sizeof(thePlainOldData));
        }
        template <class T>
        WriteableStream & appendData(const asl::Bitset<T> & theBitset) {
            typename asl::Bitset<T>::int_type myValue = theBitset;
            return append(&myValue, sizeof(myValue));
        }
        template <class TN>
        WriteableStream & appendPackedNumber(TN theNumber, size_type theNumberSize) {
            if (theNumberSize <= sizeof(theNumber)) {
                NumberReader<typename CountArranger::self, TN, sizeof(TN)>::swapBytes(theNumber);
                return append((char*)(&theNumber) + sizeof(theNumber) - theNumberSize, theNumberSize);
            } else {
                throw StreamWriteNumberTooLargeForVariable(JUST_FILE_LINE);
            }
        }
       WriteableStream & appendUnsigned(Unsigned64 theCount) {
            if (theCount < 0xF0ULL) {
                return appendUnsigned8((Unsigned8)theCount);
            }
            if (theCount < 0x100ULL) {
                appendUnsigned8(0xF0);
                return appendUnsigned8((Unsigned8)theCount);
            }
            if (theCount < 0x10000ULL) {
                appendUnsigned8(0xF1);
                return appendPackedNumber(theCount, 2);
            }
            if (theCount < 0x1000000ULL) {
                appendUnsigned8(0xF2);
                return appendPackedNumber(theCount, 3);
            }
            if (theCount < 0x100000000ULL) {
                appendUnsigned8(0xF3);
                return appendPackedNumber(theCount, 4);
            }
            if (theCount < 0x10000000000ULL) {
                appendUnsigned8(0xF4);
                return appendPackedNumber(theCount, 5);
            }
            if (theCount < 0x1000000000000ULL) {
                appendUnsigned8(0xF5);
                return appendPackedNumber(theCount, 6);
            }
            if (theCount < 0x100000000000000ULL) {
                appendUnsigned8(0xF6);
                return appendPackedNumber(theCount, 7);
            }
            appendUnsigned8(0xF7);
            return appendPackedNumber(theCount, 8);
        };
       WriteableStream & appendSigned(Signed64 theCount) {
            if ((theCount < Signed64(120LL)) && (theCount > Signed64(-121LL))) {
                return appendSigned8((Signed8)theCount);
            }
            // Note: relying on Signed64(0xFFFFFFFFFFFFFF80LL) to wrap-around to a negative number
            //       is probably not a good idea. (MS)
            if ((theCount < Signed64(0x100LL)) && (theCount >= Signed64(0xFFFFFFFFFFFFFF80LL))) {  
                appendSigned8(120);
                return appendSigned8((Signed8)theCount);
            }
            if ((theCount < 0x10000LL) && (theCount >= Signed64(0xFFFFFFFFFFFF8000LL))) {
                appendSigned8(121);
                return appendPackedNumber(theCount, 2);
            }
            if ((theCount < 0x1000000LL) && (theCount >= Signed64(0xFFFFFFFFFF800000LL))) {
                appendSigned8(122);
                return appendPackedNumber(theCount, 3);
            }
            if ((theCount < 0x100000000LL) && (theCount >= Signed64(0xFFFFFFFF80000000LL))) {
                appendSigned8(123);
                return appendPackedNumber(theCount, 4);
            }
            if ((theCount < 0x10000000000LL) && (theCount >= Signed64(0xFFFFFF8000000000LL))) {
                appendSigned8(124);
                return appendPackedNumber(theCount, 5);
            }
            if ((theCount < 0x1000000000000LL) && (theCount >= Signed64(0xFFFF800000000000LL))) {
                appendSigned8(125);
                return appendPackedNumber(theCount, 6);
            }
            if ((theCount < 0x100000000000000LL) && (theCount >= Signed64(0xFF80000000000000LL))) {
                appendSigned8(126);
                return appendPackedNumber(theCount, 7);
            }
            appendSigned8(127);
            return appendPackedNumber(theCount, 8);
        };

        WriteableStream & appendSigned64(Signed64 theCount) {
            return appendData(Arranger::get(theCount));
        }	
        WriteableStream & appendUnsigned64(Signed64 theCount) {
            return appendData(Arranger::get(theCount));
        }	
        WriteableStream & appendFloat64(Float64 theNumber) {
            return appendData(Arranger::getFloatOut(theNumber));
        }
        WriteableStream & appendUnsigned32(Unsigned32 theCount) {
            return appendData(Arranger::get(theCount));
        }	
        WriteableStream & appendSigned32(Signed32 theCount) {
            return appendData(Arranger::get(theCount));
        }	
        WriteableStream & appendFloat32(Float32 theNumber) {
            return appendData(Arranger::getFloatOut(theNumber));
        }
        WriteableStream & appendSigned16(Signed16 theWord) {
            return appendData(Arranger::get(theWord));
        }
        WriteableStream & appendUnsigned16(Unsigned16 theWord) {
            return appendData(Arranger::get(theWord));
        }
        WriteableStream & appendUnsigned8(Unsigned8 theByte) {
            return append(&theByte, sizeof(theByte));
        }
        WriteableStream & appendSigned8(Signed8 theByte) {
            return append(&theByte, sizeof(theByte));
        }
        WriteableStream & appendString(const std::string & theString) {
            return append( begin_ptr(theString), theString.size() );
        }
        WriteableStream & appendCountedString(const std::string & theString) {
            appendUnsigned(SIZE_TYPE(theString.size()));
            return appendString(theString);
        }
        virtual operator const void*() const = 0;
        Unsigned64 getByteCounter()  {
            return _myByteCounter;
        };
        void setByteCounter(Unsigned64 theCount) {
            _myByteCounter = theCount;
        };
        void countBytes(Unsigned64 theCount) {
            _myByteCounter += theCount;
        }
		virtual ~WriteableArrangedStream() {}
    private:
        Unsigned64 _myByteCounter;
    };

    typedef WriteableArrangedStream<AC_DEFAULT_BYTE_ORDER> WriteableStream;

    template <class EXTERNAL_BYTE_ORDER>
    class WriteableArrangedFile : public WriteableArrangedStream<EXTERNAL_BYTE_ORDER> {
        typedef WriteableArrangedStream<EXTERNAL_BYTE_ORDER> Base;
    public:
        WriteableArrangedFile(const std::string & theFileName, bool appendToFile = false) : 
            _myFileName(theFileName, UTF8)
        {
            if (appendToFile) {
                _myOutFile.open(_myFileName.toLocale().c_str(),std::ios::binary|std::ios::ate);
            } else {
                _myOutFile.open(_myFileName.toLocale().c_str(),std::ios::binary|std::ios::trunc);
            }
        }

        typename Base::WriteableStream & append(const void * theMemory, typename Base::size_type theSize) {
            _myOutFile.write((char*)theMemory, theSize);
            if (!_myOutFile) {
                throw StreamWriteFailed(
                    std::string("WriteableFile::append(size=")+as_string(theSize)+")",
                    std::string("Filename='")+ _myFileName.toLocale() + "' failed." );
            }
            countBytes(theSize);
            return *this;
        }
        operator const void*() const {
            return _myOutFile;
        }
    private:
        // _myFileName must be initialized before the stream
        Path _myFileName;
        mutable std::ofstream _myOutFile;
    };
    typedef WriteableArrangedFile<AC_DEFAULT_BYTE_ORDER> WriteableFile;
    /* @} */

} // end of namespace asl

#endif
