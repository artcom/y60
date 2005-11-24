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
//    $RCSfile: Block.cpp,v $
//
//   $Revision: 1.7 $
//
// Description:  Memory Block handling functions; consider to use them whenever
//               you think about using memcpy; here you get typesafe conversion,
//				 read-write control, convenient windows api interfaces and
//               also a copy-on-write (COW) implementation if you want.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Block.h"
#include "Exception.h"

#include <vector>
#include <algorithm>
#include <typeinfo>

using namespace asl;

asl::CowBlock &
asl::CowBlock::operator=(const asl::CowBlock & theSource) {
    if (!_myRep) {
        _myRep = theSource._myRep;
        if (_myRep) ++_myRep->_myRefCount;
    } else if (_myRep != theSource._myRep) {
        --_myRep->_myRefCount;
        if (_myRep->_myRefCount == 0) delete _myRep;
        _myRep = theSource._myRep;
        if (_myRep) ++_myRep->_myRefCount;
    }
    return *this;
}

void
asl::CowBlock::assign(const asl::ReadableBlock & theSource) {
    if (typeid(theSource) == typeid(CowBlock)) {
        *this = dynamic_cast<const CowBlock &>(theSource);
    } else {
        if (_myRep == 0)
            _myRep = new Representation(theSource.begin(),theSource.end());
        else {
            makeWriteable();
            _myRep->theBlock = theSource;
        }
    }
}

void
asl::CowBlock::makeWriteable() {
    if (_myRep->_myRefCount == 1) return;
    if(_myRep->_myRefCount<1)
        throw CorruptedRefCount(
        "internal refcount has illegal value",
        "CowBlock::makeWriteable()");
    --_myRep->_myRefCount;
    _myRep = new Representation(_myRep->theBlock);
}

asl::CowBlock::CowBlock(const asl::ReadableBlock & theSource) {
    _myRep = 0;
    if (typeid(theSource) == typeid(CowBlock)) {
        *this = dynamic_cast<const CowBlock &>(theSource);
    } else {
        _myRep = new Representation(theSource.begin(),theSource.end());
    }
}

asl::WriteableStream &
asl::CowBlock::append(const void * theMemory, AC_SIZE_TYPE theSize) {
	if (_myRep == 0) {
        _myRep = new Representation(static_cast<const unsigned char *>(theMemory),
									static_cast<const unsigned char *>(theMemory)+theSize);
	} else {
        makeWriteable();
        _myRep->theBlock.append(theMemory, theSize);
    }
    return *this;
}

asl::WriteableStream &
asl::CowBlock::append(const asl::ReadableBlock & theSource) {
	return append(theSource.begin(), theSource.size());
}

void
asl::CowBlock::resize(AC_SIZE_TYPE theSize) {
    if (!_myRep)
        _myRep= new Representation(theSize);
    else {
        makeWriteable();
        _myRep->theBlock.resize(theSize);
    }
}


/// copies theSource to theDest,
/// throws SizeMismatchException() when theSource and theDest sizes are not equal
void
asl::copy(const ReadableBlock & theSource, WriteableBlock & theDest) {
    if (theSource.size()!=theDest.size())
    throw BlockSizeMismatch( "theSource and theDest block size differ",
							 "copy(const ReadableBlock &, WriteableBlock &");
    std::copy(theSource.begin(),theSource.end(),theDest.begin());
}

/// copies theSource to theDest and resizes theDest if necessary
void
asl::copy(const ReadableBlock & theSource, ResizeableBlock& theDest) {
	if (theSource.size()!=theDest.size()) {
        theDest.resize(theSource.size());
	}
    std::copy(theSource.begin(),theSource.end(),theDest.begin());
}

/// writes a theBlock as hex sequence to an ostream
std::ostream &
asl::operator<<(std::ostream & os, const ReadableBlock & theBlock) {
    const char * hex_digit = "0123456789ABCDEF";
    for (ReadableBlock::const_iterator it = theBlock.begin(); it != theBlock.end(); ++it) {
        os << hex_digit[*it >> 4] << hex_digit[*it & 0x0f];
        if (!os) {
            return os;
        }
    }
    return os;
}

/// convert a ascii hex theDigit to its binary value. Examples: '2' -> 2, 'A' -> 10,
bool
asl::ascii_to_bin(char & theDigit) {
    if (theDigit >= '0' && theDigit <= '9') {
        theDigit = theDigit - '0';
        return 1;
    } else if (theDigit >= 'A' && theDigit<='F') {
        theDigit = theDigit - 'A' + 10;
        return 1;
    } else if (theDigit >= 'a' && theDigit<='f') {
        theDigit = theDigit - 'a' + 10;
        return 1;
    }
    return 0;
}

/// reads a fixed sized theBlock as hex sequence from a istream
std::istream&
asl::operator>>(std::istream& is, WriteableBlock & theBlock) {
    const char * hex_digit = "0123456789ABCDEF";
    for (WriteableBlock::iterator it = theBlock.begin(); it != theBlock.end(); ++it) {
        char msd;
        is.get(msd);
        if (!ascii_to_bin(msd)) {
            is.clear(std::ios::failbit);
            return is;
        }
        char lsd;
        is.get(lsd);
        if (!ascii_to_bin(lsd)) {
            is.clear(std::ios::failbit);
            return is;
        }
        *it = msd << 4 | lsd;
    }
    return is;
}

/// reads a variable sized theBlock as hex sequence from a istream
std::istream&
asl::operator>>(std::istream& is, ResizeableBlock & theBlock) {
    const char * hex_digit = "0123456789ABCDEF";
    do {
        char msd;
        if (!(is.get(msd)) || msd == ' ') {
            is.clear();
            return is;
        }
        if (!ascii_to_bin(msd)) {
            is.clear(std::ios::failbit);
            return is;
        }
        char lsd;
        if (!(is.get(lsd)) || lsd == ' ') {
            is.clear(std::ios::failbit);
            return is;
        }

        if (!ascii_to_bin(lsd)) {
            is.clear(std::ios::failbit);
            return is;
        }
        unsigned char theByte = msd << 4 | lsd;
        theBlock.appendUnsigned8(theByte);
    } while (is);
    return is;
}

/// returns true if both Blocks contain the same data
bool
asl::operator==(const ReadableBlock & lhs, const ReadableBlock & rhs) {
    if (lhs.size() == rhs.size())
        return std::equal(lhs.begin(),lhs.end(),rhs.begin());
    return 0;
}
/// returns true if the content of lhs is lexicographically less than the content of rhs
bool
asl::operator<(const ReadableBlock & lhs, const ReadableBlock & rhs) {
    return std::lexicographical_compare(lhs.begin(),lhs.end(),
                                        rhs.begin(),rhs.end());
}

