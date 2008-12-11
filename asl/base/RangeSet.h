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
//
// Description:  A hybrid (reference counting & mark-sweep garbage collector
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : 
//    formatting             : 
//    documentation          : 
//    test coverage          : 
//    names                  : 
//    style guide conformance: 
//    technical soundness    : 
//    dead code              : 
//    readability            : 
//    understandabilty       : 
//    interfaces             : 
//    confidence             : 
//    integration            : 
//    dependencies           : 
//    cheesyness             : 
//
//    overall review status  :
//
//    recommendations:
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_RangeSet_h_
#define _included_asl_RangeSet_h_

#include "settings.h"
#include "Exception.h"
#include "string_functions.h"

#ifdef WIN32
#include <stddef.h>
#endif

#include <assert.h>
#include <pthread.h>
#ifdef OSX
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#include <vector>
#include <algorithm>
#include <new>


namespace asl {

typedef unsigned long Size;

/*! \addtogroup aslbase */
/* @{ */

    DEFINE_EXCEPTION(EmptyRange,asl::Exception);


    // This class manages a set of T using a vector of ranges. It is about ten times faster than std::set for
    // consecutive linear order insertion and deletion for ranges of any size, and as fast a std::set for up to
    // 10000 random insertions and deletion out of a set of 10000 elements. With 100000 random insertions it is ten
    // times slower. For sets smaller than 1000 elements insertion and deletion it is much faster for any usage pattern.
    // And in any case it requires less memory than std::set, in extreme cases it requires only four words
    // to store a range of arbitrary size.
    template <class T>
    class RangeSet {
        typedef std::vector<T> RangeVector;
    public:

        // returns true if none of the elements have been registered before, otherwise false is returned and nothing is changed 
        // theBegin must be > theEnd, theEnd is not inclusive
        bool insert(const T & theBegin, const T & theEnd )
        {
            if (theBegin >= theEnd) {
                throw EmptyRange("RangeSet::insert: can not add empty range", PLUS_FILE_LINE);
            }
            if (myValues.size() == 0) {
                push_back2(theBegin, theEnd);
                return true;
            }
            typename RangeVector::iterator lb = std::lower_bound(myValues.begin(),myValues.end(), theBegin);
            // check for end of container
            if (lb == myValues.end()) { 
                push_back2(theBegin, theEnd);
                return true;
            }
            return insert_remove<0>(lb, theBegin, theEnd);
        }
        bool insert(const T & theElement) {
            return insert(theElement, theElement+1);
        }

        // returns true if none of the elements have been registered before, otherwise false is returned and nothing is changed 
        // theBegin must be > theEnd, theEnd is not inclusive
        bool remove(const T & theBegin, const T & theEnd )
        {
            if (theBegin >= theEnd) {
                throw EmptyRange("RangeSet::remove: can not remove empty range", PLUS_FILE_LINE);
            }
            if (myValues.size() == 0) {
                return false;
            }
            if (myValues.size() == 2) {
                if (myValues[0] == theBegin && myValues[1] == theEnd) {
                    myValues.resize(0);
                    myValues.reserve(0);
                    return true;
                }
            }
             typename RangeVector::iterator lb = std::lower_bound(myValues.begin(),myValues.end(), theBegin);
            // check for end of container
            if (lb == myValues.end()) { 
                return false;
            }
            return insert_remove<1>(lb, theBegin, theEnd);
        }
        bool remove(const T & theElement) {
            return remove(theElement, theElement+1);
        }

        unsigned size() const {
            unsigned mySize = 0;
            for (typename RangeVector::size_type i = 0; i< myValues.size(); i+=2) {
                mySize+=myValues[i+1] - myValues[i];
            }
            return mySize;
        }


        // This wonderful algorithm uses a symmetry in the RangeVector data structure:
        // when looking at the list starting from index 0, it is a list of used ranges
        // when looking at the list starting from index 1, it is a list of free ranges
        // So inserting into the free range list is exactly the same like deleting from
        // the used list and vice versa.
        // remove: ALGO=0 insert, ALGO=1 remove
        template <int ALGO>
        bool insert_remove(typename RangeVector::iterator lb, const T & theBegin, const T & theEnd) {
            unsigned i = lb - myValues.begin();
            if ((i & 1) == ALGO) {
                // *lb aka myValues[i] is a stop value,
                // that meets the condition myValues[i] >= theBegin aka theBegin <= myValues[i]

                // check for adjacent range to an existing range
                if (theBegin < myValues[i]) {
                    if (theEnd == myValues[i]) { // check for end-adjacent fit
                        myValues[i] = theBegin; // just shrink top of range downward
                        return true;
                    }
                    if (theEnd < myValues[i]) { // new range ends before end of current range
                        insert2(lb, theBegin, theEnd); // (!) odd insert redefines splits range 
                        return true;
                    }
                    // new range overlaps with existing range
                }
            } else {
                // *lb aka myValues[i] is a start value,
                // that meets the condition myValues[i] >= theBegin aka theBegin <= myValues[i]

                // check for adjacent range to an existing range
                if (theBegin == myValues[i]) {
                    // new range starts at known end
                    if ((i+1 == myValues.size()-ALGO) || myValues[i+1] > theEnd) {
                        // new range fits before next range, just extend end of lb range
                        myValues[i]= theEnd;
                        return true;
                    }
                    if (myValues[i+1] == theEnd) {
                        // new range fits exactly before next range, merge two ranges
                        myValues[i]= myValues[i+2];
                        erase2(lb+1);
                        return true;
                    }
                    // overlap with next range
                }
            }
            return false;
        }

        bool contains(const T & theValue) const {
            if (myValues.size() == 0) {
                return false;
            }
            typename RangeVector::const_iterator lb = std::lower_bound(myValues.begin(),myValues.end(), theValue);
            unsigned i = lb - myValues.begin();
            if (i & 1) {
                // lb is a stop ptr
                if (theValue < myValues[i]) {
                    return true; // stop ptr includes our position
                }
                return false;    
            } else {
                // lb is a start ptr or end   
                if (theValue < myValues[i]) { // check for < begin
                    return false;
                }
                if (lb == myValues.end()) { // check for > end
                    return false;
                }
                return true;
             }
        }
        bool getFirst(T & theValue) const {
            if (myValues.size() == 0) {
                return false;
            }
            theValue = myValues[0];
            return true;
        }
        bool advance(T & theValue) const {
            if (myValues.size() == 0) {
                return false;
            }
            typename RangeVector::const_iterator lb = std::lower_bound(myValues.begin(),myValues.end(), theValue+1);
            if (lb == myValues.end()) {
                return false;
            }
            unsigned i = lb - myValues.begin();
            if (i & 1) {
                // lb is a stop ptr
                if (theValue+1 < myValues[i]) {
                    theValue+=1;
                    return true; // stop ptr includes our position
                }
                if (i == myValues.size() - 1) {
                    return false;    
                }
                // advance to next start
                theValue = myValues[i+1];
                return true; 
            } else {
                // lb is a start ptr or end   
                if (theValue+1 < myValues[i]) { // check for < begin
                    theValue = myValues[0];
                    return true;
                }
                if (lb == myValues.end()) { // check for > end
                    return false;
                }
                theValue+=1;
                return true;
             }
        }
         std::ostream & print(std::ostream & os) const {
            os << "RangeVector@" << (void*)this << " ";
            for (int i = 0; i < myValues.size(); i+=2) {
                os << "|"<<myValues[i]<<"-"<<myValues[i+1];
            }
            os << "|";
            return os;
        }
 private:
        void insert2(typename RangeVector::iterator thePos, const T & theFirst, const T & theSecond) {
            unsigned i = thePos - myValues.begin();
            myValues.insert(thePos, 2, theFirst);
            myValues[i+1] = theSecond;
        }
        void push_back2(const T & theFirst, const T & theSecond) {
            myValues.push_back(theFirst);
            myValues.push_back(theSecond);
        }
        void erase2(typename RangeVector::iterator thePos) {
            myValues.erase(thePos, thePos+2);
        }
         // every even index is a start ptr, every odd index a stop ptr
        RangeVector myValues;
    };
    template <class T>
    std::ostream & operator<<(std::ostream & os, const RangeSet<T> & theRange) {
        return theRange.print(os);
    }


} //namespace

#undef DB2
#undef DBP
#undef DBP2


/* @} */

#endif
