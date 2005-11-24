/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2003, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: Block.h,v $
//
//   $Revision: 1.26 $
//
// Description:  Memory Block handling functions; consider to use them whenever
//               you think about using memcpy; here you get typesafe conversion
//		 read-write control, convenient windows api interfaces and
//               also a copy-on-write (COW) implementation if you want.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_Block_
#define _included_asl_Block_

#include "Exception.h"
#include "string_functions.h"
#include "Stream.h"

#include <vector>
#include <algorithm>

namespace asl {


    /*! \addtogroup aslbase */
    /* @{ */

    DEFINE_EXCEPTION(BlockException,Exception);
    DEFINE_EXCEPTION(BlockTooSmall,BlockException);
    DEFINE_EXCEPTION(CorruptedRefCount,BlockException);
    DEFINE_EXCEPTION(BlockSizeMismatch,BlockException);
    DEFINE_EXCEPTION(NotEnoughCapacity,BlockException);
    DEFINE_EXCEPTION(AdressOverflow,BlockException);

    //-------------------------------------------------------------------------------
    /**
        ReadableBlock is an interface class.
        It provides an interface for read access to raw byte data
    **/
    class WriteableBlock;

    class ReadableBlock : public ReadableStream {
    public:
        typedef unsigned char Byte;

        typedef unsigned char value_type;
        typedef const unsigned char * const_iterator;

        /// returns a read-only pointer to first byte in the theBlock
        virtual const unsigned char * begin() const = 0;

        /// returns a read-only pointer past the last byte in the theBlock
        virtual const unsigned char * end() const = 0;

        /// returns the number of bytes in the theBlock
        virtual AC_SIZE_TYPE size() const {
            return end()-begin();
        }
        /// returns a read-only reference to the i th character in the theBlock
        virtual const unsigned char& operator[](AC_SIZE_TYPE i) const {
            return begin()[i];
        }
        /// copies sizeof(TX) bytes to theDest starting at theOffset
        AC_SIZE_TYPE readBytes(void * theDest, AC_SIZE_TYPE theSize, AC_SIZE_TYPE theReadOffset) const {
            AC_SIZE_TYPE myEndOffset = theReadOffset+theSize;
            if (myEndOffset < theReadOffset) {
                throw AdressOverflow("Illegal Offset Value, possibly a negative unsigned", "ReadableBlock::copySlice()");
            }
            if (size()<myEndOffset) {
                throw BlockTooSmall("theBlock too small to retrieve this amount of bytes at this position",
                                    "ReadableBlock::copySlice()");
            }
            std::copy(begin()+theReadOffset, begin()+myEndOffset,static_cast<unsigned char *>(theDest));
            return myEndOffset;
        }
#ifdef goodNewCompiler
        /// returns a read only-reference to the theBlock interpreted as template argument TX
        template <class TX>
        const TX& as() const {
            if (size()<sizeof(TX)) {
                throw BlockTooSmall("theBlock too small to pose as this type",
                                    "ReadableBlock::as()");
            }
            return *reinterpret_cast<const TX*>(begin());
        }
        /// returns a read only-reference to the theBlock interpreted as template argument T at thePos
        template <class TX>
        const TX& as(AC_SIZE_TYPE thePos) const {
            if (size() < thePos+sizeof(TX)) {
                throw BlockTooSmall("theBlock too small to to contain this type at this positions",
                                    "ReadableBlock::as()");
            }
            return *reinterpret_cast<const TX*>(begin()+thePos);
        }
#endif
        /// std::string compatibility versions of begin()
        virtual const char * strbegin() const {
            return reinterpret_cast<const char *>(begin());
        }
        /// std::string compatibility versions of end()
        virtual const char * strend() const {
            return reinterpret_cast<const char *>(end());
        }

        /// word-type begin()
        virtual const short * wbegin() const {
            return reinterpret_cast<const short *>(begin());
        }
        /// word-type end()
        virtual const short * wend() const {
            return reinterpret_cast<const short *>(end());
        }
        /// returns the number of words in the theBlock
        virtual AC_SIZE_TYPE wsize() const {
            return size()/2;
        }
        virtual ~ReadableBlock() {}
    };

    template <class EXTERNAL_BYTE_ORDER, class SIZE_TYPE, class OFFSET_TYPE>
    WriteableArrangedStream<EXTERNAL_BYTE_ORDER, SIZE_TYPE, OFFSET_TYPE> &
    WriteableArrangedStream<EXTERNAL_BYTE_ORDER, SIZE_TYPE, OFFSET_TYPE>
    ::append(const ReadableBlock & theSource) 
    {
       return append(theSource.begin(), theSource.size());
    }
    /* 
    template <class EXTERNAL_BYTE_ORDER, class SIZE_TYPE, class OFFSET_TYPE>
    WriteableArrangedStream<EXTERNAL_BYTE_ORDER, SIZE_TYPE, OFFSET_TYPE> &
    append(WriteableArrangedStream<EXTERNAL_BYTE_ORDER, SIZE_TYPE, OFFSET_TYPE> & theStream,
           const ReadableBlock & theSource)
    {
       return theStream.append(theSource.begin(), theSource.size());
    }
    */

    /**
        WriteableBlock is an interface class extending the ReadableBlock.
        It adds the interface for write access to raw byte data.
    **/
    /*
      Design Note: Writing through an iterator also enables reading,
      so it is just consequent to derive WriteableBlock from ReadableBlock.
      Consequentially we can not have Write-Only Blocks.
    */
    class WriteableBlock : public ReadableBlock {
    public:
        typedef unsigned char * iterator;

        virtual WriteableBlock & operator=(WriteableBlock & theSource) {
            assign(theSource);
            return *this;
        }
        virtual void assign(const ReadableBlock &) = 0;

        /// returns a pointer to first byte in the theBlock
        virtual unsigned char *  begin() = 0;

        /// returns a pointer past the last byte in the theBlock
        virtual unsigned char * end() = 0;

        /// returns a reference to the i th character in the theBlock
        virtual unsigned char & operator[](AC_SIZE_TYPE i) {
            return begin()[i];
        }
#ifdef goodNewCompiler
        /// returns a reference to the theBlock interpreted as template argument T
        template <class TX>
        TX & as() {
            if (size()<sizeof(TX)) {
                throw BlockTooSmall("theBlock too small to pose as this type",
                                    "ReadableBlock::as()");
            }
            return *reinterpret_cast<TX*>(begin());
        }
        /// returns a reference to the theBlock interpreted as template argument T at thePos
        template <class TX>
        TX & as(AC_SIZE_TYPE thePos) {
            if (size() < thePos+sizeof(TX)) {
                throw BlockTooSmall("theBlock too small to contain this type at this positions",
                                    "ReadableBlock::as()");
            }
            return *reinterpret_cast<TX *>(begin()+thePos);
        }
#endif
        // inherited from ReadableBlock:
        /// returns a read-only pointer to first byte in the theBlock
        virtual const unsigned char * begin() const = 0;
        /// returns a read-only pointer past the last byte in the theBlock
        virtual const unsigned char * end() const = 0;

        // std::string compatibility names

        /// std::string compatibility versions of begin()
        virtual char * strbegin() {
            return reinterpret_cast<char *>(begin());
        }
        /// std::string compatibility versions of end()
        virtual char * strend() {
            return reinterpret_cast<char *>(end());
        }
        /// std::string compatibility versions of begin()
        virtual const char * strbegin() const {
            return reinterpret_cast<const char *>(begin());
        }
        /// std::string compatibility versions of end()
        virtual const char * strend() const {
            return reinterpret_cast<const char *>(end());
        }

        /// returns a read-only reference to the i th character in the theBlock
        virtual const unsigned char & operator[](AC_SIZE_TYPE i) const {
            return begin()[i];
        }

        /// word versions of begin()
        virtual short * wbegin() {
            return reinterpret_cast<short *>(begin());
        }
        /// word versions of end()
        virtual short * wend() {
            return reinterpret_cast<short *>(end());
        }
        virtual ~WriteableBlock() {}
    };

// ReadableArrangedStream implementation
template <class EXTERNAL_BYTE_ORDER, class SIZE_TYPE, class OFFSET_TYPE>
SIZE_TYPE
ReadableArrangedStream<EXTERNAL_BYTE_ORDER, SIZE_TYPE, OFFSET_TYPE>::readBlock(WriteableBlock & theDest, SIZE_TYPE theReadOffset) const {
            return readBytes(theDest.begin(),theDest.size(),theReadOffset);
}
    

    /**
        ResizeableBlock is an interface class extending the WriteableBlock.
        It adds the interface for resizing the theBlock and appending to it.
    */
    /*
      Design Note: It does not make much sense as it is almost impossible to implement
      a resizeable Block that is not writeable because we might need to relocate
      the block, so it seems to be natural to derive ResizeableBlock from WriteableBlock.
    */
    class ResizeableBlock : public WriteableBlock, public WriteableStream {
    protected:
        ResizeableBlock() {};
    public:
        /// sets the the Size of the Block
        virtual void resize(AC_SIZE_TYPE theSize) = 0;

        /// sets the theSize of the theBlock
        virtual void resizeBy(AC_SIZE_TYPE theAdditionalSize) {
            resize(size() + theAdditionalSize);
        }
        /// sets the theSize of the theBlock
        virtual unsigned char * extendEnd(AC_SIZE_TYPE theAdditionalSize) {
            AC_SIZE_TYPE myOldSize = size();
            resize(size() + theAdditionalSize);
            return begin() + myOldSize;
        }

        /// extends the block's size by theSize and copies theSize bytes from
        /// theMemory location to the end of the block after
        virtual WriteableStream & append(const void * theMemory, AC_SIZE_TYPE theSize) {
            AC_SIZE_TYPE myOldSize = size();
            resize(myOldSize + theSize);
            std::copy(static_cast<const unsigned char *>(theMemory),
                      static_cast<const unsigned char *>(theMemory)+theSize,
                      begin() + myOldSize);
            return *this;
        }
        /// appends all the bytes from theSource at the end of the block.
        virtual WriteableStream & append(const ReadableBlock & theSource) {
            return append(theSource.begin(), theSource.size());
            return *this;
        }

        /// returns a read-only pointer to first byte in the theBlock
        virtual const unsigned char * begin() const = 0;
        /// returns a read-only pointer past the last byte in the theBlock
        virtual const unsigned char * end() const = 0;

        /// returns a pointer to first byte in the theBlock
        virtual unsigned char * begin() = 0;
        /// returns a pointer past the last byte in the theBlock
        virtual unsigned char * end() = 0;

        virtual ResizeableBlock & operator=(ResizeableBlock & theSource) {
            assign(theSource);
            return *this;
        }
        virtual void assign(const ReadableBlock & theSource) {
            resize(theSource.size());
            std::copy(theSource.begin(),theSource.end(), begin());
        }
        virtual ~ResizeableBlock() {}
    };

    /**
        FixedCapacityBlock is an interface class extending the ResizeableBlock.
        It adds the interface for querying the maxmimum theBlock capacity.
    **/
    /*
      Design Note: In practice at least some fixed-size Blocks often will be partially
      filled, so it seems to be a good decision to treat fixed size blocks as resizeable
      blocks with fixed capacity and thus derive them from ResizeableBlock.
      We can just use a WriteableBlock if we have a true fixed sized block.
    */
    class FixedCapacityBlock : public ResizeableBlock {
    public:
        virtual FixedCapacityBlock & operator=(FixedCapacityBlock & theSource) {
            assign(theSource);
            return *this;
        }
        /// returns the maximum number of bytes this theBlock can hold
        virtual AC_SIZE_TYPE capacity() const = 0;

        /// returns the maximum number of bytes this theBlock can hold
        virtual AC_SIZE_TYPE bytesfree() const {
            return capacity() - size();
        }

        /// returns a pointer past the last byte this theBlock can hold
        virtual unsigned char * limit() {
            return begin() + capacity();
        }
        /// returns a read only pointer past the last byte this theBlock can hold
        virtual const unsigned char * limit() const {
            return begin() + capacity();
        }
        virtual ~FixedCapacityBlock() {}
    };

    /**
        VariableCapacityBlock is an interface class extending the FixedCapacityBlock.
        It adds the interface for setting the maxmimum theBlock capacity.
    **/
    class VariableCapacityBlock : public FixedCapacityBlock {
    public:
        virtual VariableCapacityBlock & operator=(VariableCapacityBlock & theSource) {
            assign(theSource);
            return *this;
        }
        /// will set the maximum number of bytes this theBlock can hold
        /// to 'theSize' if possible; if theCapacity is smaller the size(),
        /// the capacity will be set to size()
        virtual void reserve(AC_SIZE_TYPE theCapacity) = 0;

        virtual ~VariableCapacityBlock() {}
    };

    class Chunk {
    public:
        Chunk() : _myMem(0), _mySize(0), _myCapacity(0) {}
        Chunk(const Chunk & theOther) : _myMem(0), _mySize(0), _myCapacity(0) {
            resize(theOther.size());
            std::copy(theOther.begin(),theOther.end(),_myMem);
        }
        Chunk & operator=(const Chunk & theOther) {
            resize(0);
            resize(theOther.size());
            std::copy(theOther.begin(),theOther.end(),_myMem);
            return *this;
        }
        Chunk(const unsigned char * theBegin, const unsigned char * theEnd)
            : _myMem(0), _mySize(0), _myCapacity(0)
        {
            resize(theEnd - theBegin);
            std::copy(theBegin,theEnd,_myMem);
        }
        Chunk(const AC_SIZE_TYPE theSize, const unsigned char theInitValue)
            : _myMem(0), _mySize(0), _myCapacity(0)
        {
            resize(theSize);
            std::fill(_myMem,_myMem + _mySize, theInitValue);
        }
        void reserve(AC_SIZE_TYPE theSize) {
            if (_mySize > theSize) {
                _mySize = theSize;
            }
            unsigned char * myNewMem = static_cast<unsigned char *>(malloc(theSize));
            if (myNewMem) {
                if (_myMem) {
                    std::copy(_myMem,_myMem+_mySize,myNewMem);
                    free(_myMem);
                    _myMem = myNewMem;
                    _mySize = theSize;
                }
                _myMem =  myNewMem;
            } else {
                throw asl::Exception("out of memory",std::string("Chunk::reserve(")+asl::as_string(theSize)+")");
            }
            _myCapacity = theSize;
        }

        void resize(AC_SIZE_TYPE theSize) {
            if (_myCapacity < theSize) {
                reserve(theSize + theSize / 2);
            }
            _mySize = theSize;
        }
        unsigned char * begin() {
            return _myMem;
        }
        const unsigned char * begin() const {
            return _myMem;
        }
        unsigned char * end() {
            return _myMem + _mySize;
        }
        const unsigned char * end() const {
            return _myMem + _mySize;
        }
        AC_SIZE_TYPE size() const {
            return _mySize;
        }
        AC_SIZE_TYPE capacity() const {
            return _myCapacity;
        }

        ~Chunk() {
            if (_myMem) {
                free(_myMem);
            }
        }
    private:
        unsigned char * _myMem;
        AC_SIZE_TYPE _mySize;
        AC_SIZE_TYPE _myCapacity;
    };

    //-------------------------------------------------------------------------------
    /**
        Block is a general purpose class for raw byte data storage;
        it has full copy semantics, can be used in any standard container
        and initialized with data from any ReadableBlock
    **/
    class Block : public VariableCapacityBlock {
    public:
        operator bool() const {
            return true;
        }
        virtual const unsigned char * begin() const {
            return &(*_myData.begin());
        }
        virtual const unsigned char * end() const {
            return &(*_myData.end());
        }
        virtual unsigned char * begin() {
            return &(*_myData.begin());
        }
        virtual unsigned char * end() {
            return &(*_myData.end());
        }
        virtual void resize(AC_SIZE_TYPE theSize) {
            _myData.resize(theSize);
        }
        virtual AC_SIZE_TYPE capacity() const {
            return _myData.capacity();
        }
        virtual void reserve(AC_SIZE_TYPE theSize) {
            _myData.reserve(theSize);
        }
        virtual AC_SIZE_TYPE size() const {
            return _myData.size();
        }
    public: /* additional utilities */
        // Block(const Block &); // use default copy constructor */
        // Block & operator=(const Block & theSource); // use default assignment operator */

        /// does nothing
        Block() {}

        /// copies data from the range [theBegin..theEnd[ into the theBlock
        Block(const unsigned char * theBegin, const unsigned char * theEnd)
            : _myData(theBegin, theEnd) {}

        /// creates a theBlock with theSize bytes all set to theInitializationValue
        explicit Block(AC_SIZE_TYPE theSize, unsigned char theInitializationValue= 0)
            : _myData(theSize, theInitializationValue) {}

        /// sets the theSize equal to theSource theSize and copies all the data from theSource
        Block(const Block & theSource)
            : VariableCapacityBlock() //useless call of base constructor to satisfy compiler
        {
            assign(theSource);
        }
        /// sets the theSize equal to theSource theSize and copies all the data from theSource
        Block(const ReadableBlock & theSource) {
            assign(theSource);
        }
        /// sets the theSize equal to theSource theSize and copies all the data from theSource
        Block & operator=(const ReadableBlock & theSource) {
            assign(theSource);
            return *this;
        }
        /// sets the theSize equal to theSource theSize and copies all the data from theSource
        Block & operator=(const Block & theSource) {
            assign(theSource);
            return *this;
        }
        /// does nothing, just there to enable deriving from theBlock
        virtual ~Block() {
        }
    protected:

        Chunk & getData() {
            return _myData;
        }
        Chunk & mutableData() const {
            return const_cast<Chunk&>(_myData);
        }
    private:
        Chunk _myData;
    };

    /**
        SizeBlock extends Block with a member getSizeField() which can
        be used to receive the number of Bytes in typical windows api
        calls where data is copied into user space. When the theBlock is
        accessed a lazy resize is performed on the theBlock.
    **/
    class SizeBlock : public Block {
    public: /* implement overloaded functions first */
        virtual const unsigned char * begin() const {
            adjustSize();
            return Block::begin();
        }
        virtual const unsigned char * end() const {
            adjustSize();
            return Block::end();
        }
        virtual unsigned char * begin() {
            adjustSize();
            return Block::begin();
        }
        virtual unsigned char * end() {
            adjustSize();
            return Block::end();
        }
        virtual void resize(AC_SIZE_TYPE theSize) {
            Block::resize(theSize);
            _myRequestedSize = Block::size();
        }
        virtual AC_SIZE_TYPE size() const {
            adjustSize();
            return Block::size();
        }
        /// returns reference to field where a desired theSize can be written to;
        /// a resize will be performed if necessary when any method of this class is called
        AC_SIZE_TYPE & getSizeField() {
            adjustSize();
            return _myRequestedSize;
        }
        /// returns pointer to field where a desired theSize can be written to;
        /// a resize will be performed if necessary when any method of this class is called
        AC_SIZE_TYPE * getSizeFieldPtr() {
            return &getSizeField();
        }
    public: /* additional utilities */
        // SizeBlock(const SizeBlock &); // use default copy constructor */
        // SizeBlock & operator=(const SizeBlock & theSource); // use default assignment operator */

        /// does nothing
        SizeBlock() : _myRequestedSize(0) {}

        /// copies data from the range [theBegin..]theEnd into the theBlock
        SizeBlock(const unsigned char * theBegin, const unsigned char * theEnd)
            : Block(theBegin, theEnd), _myRequestedSize(theEnd-theBegin)  {}

        /// creates a theBlock with theSize bytes all set to theInitializationValue
        explicit SizeBlock(AC_SIZE_TYPE theSize, unsigned char theInitializationValue = 0)
            : Block(theSize, theInitializationValue), _myRequestedSize(theSize) {}

        /// appends all the bytes from theSource at the end
        WriteableStream & append(const ReadableBlock & theSource) {
            adjustSize();
            WriteableStream::append(theSource);
            return *this;
        }
        /// extends the block's size by theSize and copies theSize bytes from
        /// theMemory location to the end of the block after
        virtual WriteableStream & append(const void * theMemory, AC_SIZE_TYPE theSize) {
            adjustSize();
            Block::append(theMemory,theSize);
            return *this;
        }

        /// sets the theSize equal to theSource theSize and copies all the data from theSource
        SizeBlock(const ReadableBlock & theSource) {
            assign(theSource);
        }
        /// sets the theSize equal to theSource theSize and copies all the data from theSource
        SizeBlock & operator=(const ReadableBlock & theSource) {
            assign(theSource);
            return *this;
        }
        /// does nothing, just there to enable deriving from theBlock
        virtual ~SizeBlock() {
        }
    private:
        void adjustSize() const {
            if (_myRequestedSize != Block::size()) {
                Block::mutableData().resize(_myRequestedSize);
            }
        }
    private:
        AC_SIZE_TYPE _myRequestedSize;
    };

    /**
        FixedBlock is a general purpose template class for raw byte data storage;
        it has full copy semantics, can be used in any standard container
        and initialized with data from any ReadableBlock;
        any POD type can be used as used as template argument; however, the most
        useful application is to use a struct as template argument to get formatted
        Block access.
    **/
    template <class T>
    class FixedBlock : public FixedCapacityBlock {
    public: /* implement ResizeableBlock first */
        operator bool() const {
            return true;
        }
        virtual const unsigned char * begin() const {
            return reinterpret_cast<const unsigned char *>(&_myData);
        }
        virtual const unsigned char * end() const {
            return begin()+size();
        }
        virtual unsigned char * begin() {
            return reinterpret_cast<unsigned char *>(&_myData);
        }
        virtual unsigned char * end() {
            return begin()+size();
        }
        virtual void resize(AC_SIZE_TYPE theSize) {
            if (theSize > capacity()) {
                throw NotEnoughCapacity("can not resize to requested value",
                                        "FixedBlock::resize()");
            }
            _mySize=theSize;
        }
        virtual AC_SIZE_TYPE size() const {
            return _mySize;
        }
    public:
        /* FixedBlock(const FixedBlock &); // use default copy constructor */
        /* FixedBlock & operator=(const FixedBlock & theSource); // use default assignment operator */
        /// does nothing
        FixedBlock() : _mySize(0) {}
        /// copies data from the range [theBegin..]theEnd into the theBlock, can throw OversizeException
        FixedBlock(const unsigned char * theBegin, const unsigned char * theEnd) {
            resize(theEnd - theBegin);
            std::copy(theBegin,theEnd,begin());
        }
        /// copies rhs into the theBlock and sets theSize to capacity
        explicit FixedBlock(const T & rhs): _myData(rhs), _mySize(sizeof(T)) {}

        FixedBlock & operator=(const T & rhs) {
            _mySize = sizeof(T);
            _myData = rhs;
            return *this;
        }
        /// sets the theSize equal to theSource theSize and copies all the data from theSource, can throw OversizeException
        FixedBlock(const ReadableBlock & theSource) {
            assign(theSource);
        }
        /// sets the theSize equal to theSource theSize and copies all the data from theSource, can throw OversizeException
        FixedBlock & operator=(const ReadableBlock & theSource) {
            assign(theSource);
            return *this;
        }
        virtual void assign(const ReadableBlock & theSource) {
            resize(theSource.size());
            std::copy(theSource.begin(),theSource.end(),begin());
        }
        virtual AC_SIZE_TYPE capacity() const {
            return sizeof(T);
        }
        /// allows direct read access to the template data member
        const T & getData() const {
            return _myData;
        }
        /// Allows direct write access to the template data member; sets theSize to capacity
        T & getData() {
            _mySize = capacity();
            return _myData;
        }
        /// does nothing, just there to enable deriving from theBlock
        virtual ~FixedBlock() {
        }
    private:
        T _myData;
        AC_SIZE_TYPE _mySize;
    };

    //-------------------------------------------------------------------------------
    /**
        ReadableBlockAdapter is a general purpose adapter that provides a ReadableBlock
        for any contiguous byte sequence; it does not allocate any memory,
        it just points to a given block. When a Block type is assigned to this class,
        no data is copied, just a reference is created.
        ReadableBlockAdapter does not assume ownership of any data passed to it.
        The copy() function or assignment to a copying Block class must be used to
        move actual data.
    **/
    class ReadableBlockAdapter : public ReadableBlock {
    protected:
    public:
        virtual const unsigned char * begin() const {
            return _myBegin;
        }
        virtual const unsigned char * end() const {
            return _myEnd;
        }
    public:
        operator bool() const {
            return _myBegin != 0;
        }
        ReadableBlockAdapter() : _myBegin(0), _myEnd(0) {}

        /// stores a readonly reference to the theSource theBlock.
        /// Note: the reference becomes invalid when the theSource is resized
        ReadableBlockAdapter(const ReadableBlock & theSource)
            : _myBegin(theSource.begin()), _myEnd(theSource.end()) {}
        /// stores a readonly reference to the theSource theBlock.
        /// Note: the reference becomes invalid when the theSource is resized
        ReadableBlockAdapter & operator=(const ReadableBlock & theSource) {
            reference(theSource);
            return *this;
        }
        virtual void reference(const ReadableBlock & theSource) {
            _myBegin = theSource.begin();
            _myEnd = theSource.end();
        }
        /// stores a reference to the theBlock denoted by [theBegin..]theEnd
        ReadableBlockAdapter(const unsigned char * theBegin, const unsigned char * theEnd) : _myBegin(theBegin), _myEnd(theEnd) {}

        /// stores a reference to the theBlock denoted by [theBegin..]theEnd
        ReadableBlockAdapter(const void * theBegin, const void * theEnd)
            : _myBegin(static_cast<const unsigned char *>(theBegin)),
              _myEnd(static_cast<const unsigned char *>(theEnd)) {}
        virtual ~ReadableBlockAdapter() {}
    private:
        const unsigned char * _myBegin;
        const unsigned char * _myEnd;
    };

    /**
        WriteableBlockAdapter is a general purpose adapter that provides a WriteableBlock
        for any contiguous byte sequence; it does not allocate any memory,
        it just points to a given block. When a Block type is assigned to this class,
        no data is copied, just a reference is created.
        ReadableBlockAdapter does not assume ownership of any data passed to it.
        The copy() function, assignment to a copying Block class or the assign member function
        can be used to move actual data.
    **/
    class WriteableBlockAdapter : public WriteableBlock {
    public:
        operator bool() const {
            return _myBegin != 0;
        }
        virtual const unsigned char * begin() const {
            return _myBegin;
        }
        virtual const unsigned char * end() const {
            return _myEnd;
        }
        virtual unsigned char * begin() {
            return _myBegin;
        }
        virtual unsigned char * end() {
            return _myEnd;
        }
    public:
        /// creates an empty adapter
        WriteableBlockAdapter() : _myBegin(0), _myEnd(0) {}

        /// stores a writeable reference to the theSource theBlock.
        /// Note: the reference becomes invalid when the theSource is resized
        WriteableBlockAdapter(WriteableBlock & theSource)
            : _myBegin(theSource.begin()), _myEnd(theSource.end()) {}

        /// copies all data from theSource to the location pointed to by the WriteableBlockAdapter
        /// source and destination size must match
        WriteableBlock & operator=(const ReadableBlock & theSource) {
            assign(theSource);
            return *this;
        }
        /// stores a writeable reference to the theSource theBlock.
        /// Note: the reference becomes invalid when the theSource is resized
        WriteableBlock & operator=(WriteableBlock & theSource) {
            reference(theSource);
            return *this;
        }
        virtual void reference(WriteableBlock & theSource) {
            _myBegin = theSource.begin();
            _myEnd = theSource.end();
        }
        /// copies all data from theSource to the location pointed to by the WriteableBlockAdapter
        /// source and destination size must match
        virtual void assign(const ReadableBlock & theSource) {
			if (theSource.size() != size()) {
				throw BlockSizeMismatch(std::string("Blocks have different size. Source-size: ") + as_string(theSource.size()) +
					" blocksize: " + as_string(size()), PLUS_FILE_LINE);
			}
			std::copy(theSource.begin(),theSource.end(),_myBegin);
        }
        /// stores a writeable reference to the theBlock denoted by [theBegin..theEnd[
        WriteableBlockAdapter(unsigned char * theBegin, unsigned char * theEnd)
            : _myBegin(theBegin), _myEnd(theEnd) {}
        virtual ~WriteableBlockAdapter() {}
    private:
        unsigned char * _myBegin;
        unsigned char * _myEnd;
    };
    /**
        FixedBlockAdapter is a general purpose adapter that provides a FixedCapacityBlock
        for any contiguous byte sequence; it does not allocate any memory,
        it just points to a given block. When a Block type is assigned to this class,
        no data is copied, just a reference is created.
        ReadableBlockAdapter does not assume ownership of any data passed to it.
        The copy() function, assignment to a copying Block class or the assign member function
        can be used to move actual data.
    **/
    class FixedBlockAdapter : public FixedCapacityBlock {
    public:
        operator bool() const {
            return _myBegin != 0;
        }
        virtual const unsigned char * begin() const {
            return _myBegin;
        }
        virtual const unsigned char * end() const {
            return _myEnd;
        }
        virtual unsigned char * begin() {
            return _myBegin;
        }
        virtual unsigned char * end() {
            return _myEnd;
        }
        virtual void resize(AC_SIZE_TYPE theSize) {
            if (theSize > capacity()) {
                throw NotEnoughCapacity("can not resize to requested value",
                                        "FixedBlockAdapter::resize()");
            }
            _myEnd=_myBegin+theSize;
        }
        virtual AC_SIZE_TYPE capacity() const {
            return _myCapacity;
        }
    public:
        /// creates an empty adapter
        FixedBlockAdapter() : _myBegin(0), _myEnd(0), _myCapacity(0) {}

        /// stores a writeable reference to the theSource theBlock.
        /// Note: the reference becomes invalid when the theSource is resized
        FixedBlockAdapter(FixedCapacityBlock & theSource)
            : _myBegin(theSource.begin()), _myEnd(theSource.end()),
              _myCapacity(theSource.capacity())
        {
            //AC_TRACE << "FixedCapacityBlock(FixedCapacityBlock)"  << std::endl;
        }
        /*
       /// copies all data from theSource to the location pointed to by the FixedBlockAdapter
       /// source and destination size must match
       FixedCapacityBlock & operator=(const ReadableBlock & theSource) {
       AC_TRACE << "FixedCapacityBlock = ReadableBlock"  << std::endl;
       assign(theSource);
       return *this;
       }
       /// stores a writeable reference to the theSource theBlock.
       /// Note: the reference becomes invalid when the theSource is resized
       */
        FixedCapacityBlock & operator=(FixedCapacityBlock & theSource) {
            //AC_TRACE << "FixedCapacityBlock = FixedCapacityBlock"  << std::endl;
            reference(theSource);
            return *this;
        }
        virtual void reference(FixedCapacityBlock & theSource) {
            _myBegin = theSource.begin();
            _myEnd = theSource.end();
            _myCapacity = theSource.capacity();
        }
        virtual void reference(unsigned char * theBegin, unsigned char * theEnd, AC_SIZE_TYPE theCapacity) {
            _myBegin = theBegin;
            _myEnd = theEnd;
            _myCapacity = theCapacity;
        }
        /// copies all data from theSource to the location pointed to by the FixedBlockAdapter
        /// source and destination size must match
        virtual void assign(const ReadableBlock & theSource) {
            resize(theSource.size());
            std::copy(theSource.begin(),theSource.end(),_myBegin);
        }
        /// stores a writeable reference to the theBlock denoted by [theBegin..theEnd[
        FixedBlockAdapter(unsigned char * theBegin, unsigned char * theEnd, AC_SIZE_TYPE theCapacity)
            : _myBegin(theBegin), _myEnd(theEnd), _myCapacity(theCapacity)
        {
            //AC_TRACE << "FixedBlockAdapter(theBegin,theEnd,theCapacity)"  << std::endl;
        }
        virtual ~FixedBlockAdapter() {}
    private:
        unsigned char * _myBegin;
        unsigned char * _myEnd;
        AC_SIZE_TYPE _myCapacity;
    };
    /**
        CowBlock is a general purpose class for raw byte data storage
        with a reference counting shared storage area with copy-on-write
        semantics. It can be used instead of Block to avoid excessive
        copying. CowBlock can be used in any standard container
        and initialized with data from any ReadableBlock
    **/
    class CowBlock : public ResizeableBlock {
    private:
        typedef Block T;
        struct Representation {
            Representation() : _myRefCount(1) {}
            Representation(AC_SIZE_TYPE theSize, unsigned char theInitializationValue = 0) : theBlock(theSize, theInitializationValue), _myRefCount(1) {}
            Representation(const T& _Block) : theBlock(_Block), _myRefCount(1) {}
            Representation(const unsigned char * theBegin, const unsigned char * theEnd)
                : theBlock(theBegin,theEnd), _myRefCount(1) {}
            T theBlock;
            unsigned long _myRefCount;
        };
    public:
        operator bool() const {
            return true;
        }
        virtual const unsigned char * begin() const {
            if (!_myRep) return 0;
            return _myRep->theBlock.begin();
        }
        virtual const unsigned char * end() const {
            if (!_myRep) return 0;
            return _myRep->theBlock.end();
        }
        virtual unsigned char * begin() {
            if (!_myRep) return 0;
            makeWriteable();
            return _myRep->theBlock.begin();
        }
        virtual unsigned char * end() {
            if (!_myRep) return 0;
            makeWriteable();
            return _myRep->theBlock.end();
        }
        virtual void resize(AC_SIZE_TYPE theSize) ;
        virtual AC_SIZE_TYPE size() const {
            if (!_myRep) return 0;
            return _myRep->theBlock.size();
        }
    public: /* additional utilities */
        /// creates a new Block sharing the same underlying memory
        CowBlock(const CowBlock & theSource)
            : ResizeableBlock() //useless call of base constructor to satisfy compiler
        {
            _myRep = theSource._myRep;
            if (_myRep) {
                ++_myRep->_myRefCount;
            }
        }
        /// assign a CowBlock sharing the same underlying memory, eventually releasing the old representation
        CowBlock & operator=(const CowBlock & theSource);
        /// does nothing
        CowBlock() : _myRep(0) {}

        /// copies data from the range [theBegin..]theEnd into the theBlock
        CowBlock(const unsigned char * theBegin, const unsigned char * theEnd)
            : _myRep(new Representation(theBegin,theEnd))  {}

        /// creates a theBlock with theSize bytes all set to theInitializationValue
        explicit CowBlock(AC_SIZE_TYPE theSize, unsigned char theInitializationValue= 0)
            : _myRep(new Representation(theSize,theInitializationValue))  {}

        /// appends all the bytes from theSource at the end
        WriteableStream & append(const ReadableBlock & theSource);

        /// extends the block's size by theSize and copies theSize bytes from
        /// theMemory location to the end of the block after
        virtual WriteableStream & append(const void * theMemory, AC_SIZE_TYPE theSize);

        /// copies all data from theSource and resizes accordingly
        virtual void assign(const ReadableBlock & theSource);

        /// polymorpically deteremines the real type of theSource and does
        /// sharing if theSource is a CowBlock; otherwise sets the theSize
        /// equal to theSource theSize and copies all the data from theSource
        CowBlock(const ReadableBlock & theSource);

        /// polymorpically deteremines the real type of theSource and does
        /// sharing if theSource is a CowBlock; otherwise sets the theSize
        /// equal to theSource theSize and copies all the data from theSource
        CowBlock & operator=(const ReadableBlock & theSource) {
            assign(theSource);
            return *this;
        }

        /// decrements _myRefCount and eventually releases the underlying theBlock
        virtual ~CowBlock() {
            if (_myRep) {
                --_myRep->_myRefCount;
                if (_myRep->_myRefCount == 0) {
                    delete _myRep;
                }
            }
        }
    private:
        void makeWriteable();
    private:
        Representation* _myRep;
    };

    /// copies theSource to theDest, throws SizeMismatchException() when theSource and theDest are not equal
    void copy(const ReadableBlock & theSource, WriteableBlock & theDest);

    /// copies theSource to theDest and resizes theDest if necessary
    void copy(const ReadableBlock & theSource, ResizeableBlock & theDest);

    /// writes a theBlock as hex sequence to an ostream
    std::ostream& operator<<(std::ostream& os, const ReadableBlock & theBlock);

    /// convert a ascii hex theDigit to its binary value. Examples: '2' -> 2, 'A' -> 10,
    bool ascii_to_bin(char & theDigit);

    /// reads a fixed sized Block as hex sequence from a istream
    std::istream& operator>>(std::istream& is, WriteableBlock & theBlock);

    /// reads a variable sized Block as hex sequence from a istream
    std::istream& operator>>(std::istream& is, ResizeableBlock & theBlock);

    /// returns true if both Blocks contain the same data
    bool operator==(const ReadableBlock & lhs, const ReadableBlock & rhs);

    /// returns true if both Blocks do not contain the same data
    inline bool operator!=(const ReadableBlock & lhs, const ReadableBlock & rhs) {
        return !(lhs==rhs);
    }
    /// returns true if the content of lhs is lexicographically less than the content of rhs
    bool operator<(const ReadableBlock & lhs, const ReadableBlock & rhs);

    /// returns true if the content of lhs is lexicographically greater than the content of rhs
    inline bool operator>(const ReadableBlock & lhs, const ReadableBlock & rhs) {
        return rhs < lhs;
    }
    /// returns true if the content of lhs is lexicographically less than or equal to the content of rhs
    inline bool operator<=(const ReadableBlock & lhs, const ReadableBlock & rhs) {
        return lhs < rhs || lhs == rhs;
    }
    /// returns true if the content of lhs is lexicographically greater than or equal to the content of rhs
    inline bool operator>=(const ReadableBlock & lhs, const ReadableBlock & rhs) {
        return lhs > rhs || lhs == rhs;
    }

    /* @} */


    inline unsigned
    base64ToBin(const std::string & theSource, WriteableBlock & theBlock) {
        return base64ToBin(theSource, theBlock.begin(), theBlock.size());
    }

    inline void
    binToBase64(const asl::ReadableBlock & theBlock, std::string & theDest, const char * theCodeTable = cb66) {
        binToBase64(theBlock.begin(), theBlock.size(), theDest);
    }


} // end of namespace asl

#endif
