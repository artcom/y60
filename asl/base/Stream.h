/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: Stream.h,v $
//
//   $Revision: 1.8 $
//
// Description:  Memory Block handling functions; consider to use them whenever
//               you think about using memcpy; here you get typesafe conversion,
//				 read-write control, convenient windows api interfaces and
//               also a copy-on-write (COW) implementation if you want.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_Stream_
#define _included_asl_Stream_

#include "settings.h"
#include "Exception.h"
#include "string_functions.h"

#include "Ptr.h"
#include "Path.h"
#include "Enum.h"

#include <fstream>
#include <vector>
#include <algorithm>

namespace asl {

    off_t getFileSize(const std::string& theFileName);

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

    class SameOrder {
    public:
        typedef SameOrder self;
        template <class T>
        static T get(T theNumber) {
            return theNumber;
        }
    };

#define SWAP_BYTE_ORDER_16(x) (((x)&0xff)<<8 | ((x)&0xff00)>>8)
#define SWAP_BYTE_ORDER_32(x) (((x)&0xff)<<24 | ((x)&0xff00)<<8 | ((x)&0xff0000)>>8 | ((x)&0xff000000)>>24)
#define SWAP_BYTE_ORDER_64(x) (((x)&0xffULL)<<56 | ((x)&0xff00ULL)<<40 | ((x)&0xff0000ULL)<<24 | ((x)&0xff000000ULL)<<8 |\
       ((x)&0xff00000000ULL)>>8 | ((x)&0xff0000000000ULL)>>24 | ((x)&0xff000000000000ULL)>>40 | ((x)&0xff00000000000000ULL)>>56)

    class ReverseOrder {
    public:
        typedef ReverseOrder self;
#if 0
        template <class T>
        static T get(T theNumber) {
            return theNumber;
        }
#endif
        static float get(float theNumber) {
            unsigned int myResult = SWAP_BYTE_ORDER_32(*reinterpret_cast<unsigned int*>(&theNumber));
            return *reinterpret_cast<float*>(&myResult);
        }
        static double get(double theNumber) {
            unsigned long long myResult = SWAP_BYTE_ORDER_64(*reinterpret_cast<unsigned long long*>(&theNumber));
            return *reinterpret_cast<double*>(&myResult);
        }
        static short get(short theNumber) {
            return short(SWAP_BYTE_ORDER_16(theNumber));
        }
        static unsigned short get(unsigned short theNumber) {
            return (unsigned short)(SWAP_BYTE_ORDER_16(theNumber));
        }
        static int get(int theNumber) {
            return int(SWAP_BYTE_ORDER_32(theNumber));
        }
        static unsigned int get(unsigned int theNumber) {
            return (unsigned int)(SWAP_BYTE_ORDER_32(theNumber));
        }
        static long get(long theNumber) {
            return long(SWAP_BYTE_ORDER_32(theNumber));
        }
        static unsigned long get(unsigned long theNumber) {
            return (unsigned long)(SWAP_BYTE_ORDER_32(theNumber));
        }
        static long long get(long long theNumber) {
            return (long long)(SWAP_BYTE_ORDER_64(theNumber));
        }
        static unsigned long long get(unsigned long long theNumber) {
            return (unsigned long long)(SWAP_BYTE_ORDER_64(theNumber));
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

    template <class ARRANGER, class TN, int SIZE>
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
            *reinterpret_cast<unsigned int*>(&theDest) = SWAP_BYTE_ORDER_32(*reinterpret_cast<unsigned int*>(&theDest));
            //theDest = (TN)SWAP_BYTE_ORDER_32((unsigned long)theDest);
        }
    };
    template <class TN>
    struct NumberReader<ReverseOrder,TN, 8> {
        static void swapBytes(TN & theDest) {
            *reinterpret_cast<unsigned long long*>(&theDest) = SWAP_BYTE_ORDER_64(*reinterpret_cast<unsigned long long*>(&theDest));
            //theDest = (TN)SWAP_BYTE_ORDER_64((unsigned long long)theDest);
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
#if OLD_NUM
        size_type readUnsigned(size_type & theDest, size_type theReadOffset) const {
            unsigned char mySmallCount;
            theReadOffset = readUnsigned8(mySmallCount, theReadOffset);
            switch (mySmallCount) {
                case 0xff: 
                    unsigned short myWord;
                    theReadOffset = readUnsigned16(myWord, theReadOffset);
                    theDest = myWord; 
                    return theReadOffset;
                case 0xfe:
                    Unsigned32 myLong;
                    theReadOffset = readUnsigned32(myLong, theReadOffset);
                    theDest = myLong; 
                    return theReadOffset;
                case 0xfd: 
                    Unsigned64 myLongLong;
                    theReadOffset = readUnsigned64(myLongLong, theReadOffset);
                    theDest = size_type(myLongLong); 
                    return theReadOffset;
            }
            theDest=mySmallCount;
            return theReadOffset;
        }
       size_type readSigned(offset_type & theDest, size_type theReadOffset) const {
            signed char mySmallCount;
            theReadOffset = readSigned8(mySmallCount, theReadOffset);
            if (mySmallCount == -128) {
                return readSigned32(theDest, theReadOffset);
            }
            theDest=mySmallCount;
            return theReadOffset;
        }
#else
        size_type readUnsigned(Unsigned64 & theDest, size_type theReadOffset) const {
            unsigned char mySmallCount;
            theReadOffset = readUnsigned8(mySmallCount, theReadOffset);
            if (mySmallCount >= 0xF0) {
                unsigned char myLen = mySmallCount - 0xEF;
                theReadOffset = readPackedNumber(theDest, theReadOffset, myLen);
            } else {
                theDest = mySmallCount;
            }
            return theReadOffset;
        }
       size_type readUnsigned(Unsigned32 & theDest, size_type theReadOffset) const {
            unsigned char mySmallCount;
            theReadOffset = readUnsigned8(mySmallCount, theReadOffset);
            if (mySmallCount >= 0xF0) {
                unsigned char myLen = mySmallCount - 0xEF;
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
            signed char mySmallOffset;
            theReadOffset = readSigned8(mySmallOffset, theReadOffset);
            if (mySmallOffset >= 120 || mySmallOffset <= -121) {
                unsigned char myLen = (unsigned char)mySmallOffset - 119;
                theReadOffset = readPackedSignedNumber(theDest, theReadOffset, myLen);
            } else {
                theDest = mySmallOffset;
            }
            return theReadOffset;
        }
        size_type readSigned(Signed32 & theDest, size_type theReadOffset) const {
            signed char mySmallOffset;
            theReadOffset = readSigned8(mySmallOffset, theReadOffset);
            if (mySmallOffset >= 120 || mySmallOffset <= -121) {
                unsigned char myLen = (unsigned char)mySmallOffset - 119;
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
#endif
 

        size_type readSigned64(long long & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readUnsigned64(unsigned long long & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readFloat64(double & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readSigned32(int & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readUnsigned32(unsigned int & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readFloat32(float & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readUnsigned16(unsigned short & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readSigned16(short & theDest, size_type theReadOffset) const {
            return readNumber(theDest, theReadOffset);	
        }
        size_type readUnsigned8(unsigned char & theDest, size_type theReadOffset) const {
            return readData(theDest, theReadOffset);	
        }
        size_type readSigned8(signed char & theDest, size_type theReadOffset) const {
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
        virtual operator bool() const = 0;
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
        operator bool() const {
            return _myStream.operator bool();
        }
    private:
        ReadableArrangedStream<IN_BYTE_ORDER> & _myStream;
   };

    template <class EXTERNAL_BYTE_ORDER>
    class ReadableArrangedFile : public ReadableArrangedStream<EXTERNAL_BYTE_ORDER> {
        typedef ReadableArrangedStream<EXTERNAL_BYTE_ORDER> Base;
    public:
        ReadableArrangedFile(const std::string & theFileName)
            : _mySize(getFileSize(theFileName)),
            _myPosition(0),
            _myFileName(theFileName, UTF8)
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
            if (_myInFile.gcount() != theSize) {
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
        operator bool() const {
            return _myInFile;
        }
        const std::string & getName() const {
            return _myFileName.toUTF8();
        }
    private:
        mutable typename Base::size_type _myPosition;
        // _myFileName must be initialized before the stream
        Path _myFileName;
        mutable std::ifstream _myInFile;
        typename Base::size_type _mySize;
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
#ifdef OLD_NUM
         WriteableStream & appendUnsigned(size_type theCount) {
            if (theCount < 0xfd) {
                return appendSigned8((unsigned char)theCount);
            }
            if (theCount <= 0xffff) {
                appendUnsigned8(0xff);
                return appendUnsigned16((unsigned short)theCount);
            }
            if (theCount <= 0xffffffff) {
                appendUnsigned8(0xfe);
                return appendUnsigned32((Unsigned32)theCount);
            }
            appendUnsigned8(0xfd);
            return appendUnsigned64(theCount);
        }
        WriteableStream & appendSigned(offset_type theCount) {
            if (theCount < 127 && theCount > -127) {
                return appendUnsigned8((signed char)theCount);
            }
            appendUnsigned8(0x8f);
            return appendSigned32((Signed32)theCount);
        }
#else
       WriteableStream & appendUnsigned(Unsigned64 theCount) {
            if (theCount < 0xF0ULL) {
                return appendUnsigned8((unsigned char)theCount);
            }
            if (theCount < 0x100ULL) {
                appendUnsigned8(0xF0);
                return appendUnsigned8((unsigned char)theCount);
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
                return appendSigned8((signed char)theCount);
            }
            // Note: relying on Signed64(0xFFFFFFFFFFFFFF80LL) to wrap-around to a negative number
            //       is probably not a good idea. (MS)
            if ((theCount < Signed64(0x100LL)) && (theCount >= Signed64(0xFFFFFFFFFFFFFF80LL))) {  
                appendSigned8(120);
                return appendSigned8((signed char)theCount);
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

#endif

        WriteableStream & appendSigned64(long long theCount) {
            return appendData(Arranger::get(theCount));
        }	
        WriteableStream & appendUnsigned64(long long theCount) {
            return appendData(Arranger::get(theCount));
        }	
        WriteableStream & appendFloat64(double theNumber) {
            return appendData(Arranger::get(theNumber));
        }
        WriteableStream & appendUnsigned32(Unsigned32 theCount) {
            return appendData(Arranger::get(theCount));
        }	
        WriteableStream & appendSigned32(Signed32 theCount) {
            return appendData(Arranger::get(theCount));
        }	
        WriteableStream & appendFloat32(float theNumber) {
            return appendData(Arranger::get(theNumber));
        }
        WriteableStream & appendSigned16(short theWord) {
            return appendData(Arranger::get(theWord));
        }
        WriteableStream & appendUnsigned16(unsigned short theWord) {
            return appendData(Arranger::get(theWord));
        }
        WriteableStream & appendUnsigned8(unsigned char theByte) {
            return append(&theByte, sizeof(theByte));
        }
        WriteableStream & appendSigned8(signed char theByte) {
            return append(&theByte, sizeof(theByte));
        }
        WriteableStream & appendString(const std::string & theString) {
            return append(&(*theString.begin()), theString.size());
        }
        WriteableStream & appendCountedString(const std::string & theString) {
            appendUnsigned(SIZE_TYPE(theString.size()));
            return appendString(theString);
        }
        virtual operator bool() const = 0;
        Unsigned64 getByteCounter()  {
            return _myByteCounter;
        };
        void setByteCounter(Unsigned64 theCount) {
            _myByteCounter = theCount;
        };
        void countBytes(Unsigned64 theCount) {
            _myByteCounter += theCount;
        }
    private:
        Unsigned64 _myByteCounter;
    };

    typedef WriteableArrangedStream<AC_DEFAULT_BYTE_ORDER> WriteableStream;

    template <class EXTERNAL_BYTE_ORDER>
    class WriteableArrangedFile : public WriteableArrangedStream<EXTERNAL_BYTE_ORDER> {
        typedef WriteableArrangedStream<EXTERNAL_BYTE_ORDER> Base;
    public:
        WriteableArrangedFile(const std::string & theFileName) : 
            _myFileName(theFileName, UTF8)
        {
            _myOutFile.open(_myFileName.toLocale().c_str(),std::ios::binary);
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
        operator bool() const {
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
