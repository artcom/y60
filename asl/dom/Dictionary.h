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
//    $RCSfile: Dictionary.h,v $
//
//   $Revision: 1.4 $
//
// Description: tiny fast XML-Parser and DOM
// 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#ifndef _asl_xml_Dictionary_included_
#define _asl_xml_Dictionary_included_

#include "typedefs.h"
#include "ThreadingModel.h"
#include "Exceptions.h"
#include <asl/base/MemoryPool.h>
#include <asl/base/Exception.h>
#include <asl/base/Stream.h>

#include <string>
#include <vector>
#include <map>


namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */


    struct Entry : public asl::MemoryPool<Entry> {
        Entry(const std::string & theName, unsigned int theIndex) 
            :  _myName(theName), _myIndex(theIndex) {}
        DOMString _myName;    
        unsigned int _myIndex;
    };

    typedef asl::Ptr<Entry,ThreadingModel> EntryPtr;

    typedef std::map<DOMString,EntryPtr> EntryMap;
    typedef std::vector<EntryPtr> EntryList;


    class Dictionary {
    public:
        DEFINE_NESTED_EXCEPTION(Dictionary,IndexOutOfRange,asl::Exception);
        
        enum { DictMagic = 0xb60d1cf0, DictsMagic = 0xb60a11d1 };

        Dictionary() {}
        bool enterName(const DOMString & theName, unsigned int & theIndex) {
            EntryMap::const_iterator it = _myMap.find(theName);
            if (it != _myMap.end()) {
                theIndex = it->second->_myIndex;
                return false;
            } else {
                theIndex = _myList.size();
               EntryPtr myEntry(new Entry(theName,theIndex));
               _myMap[theName]=myEntry;
               _myList.push_back(myEntry);
               return true;
            }
        }
        const std::string & lookupName(unsigned int theIndex) {
            if (theIndex>_myList.size()) {
                throw IndexOutOfRange(JUST_FILE_LINE);
            }
            return _myList[theIndex]->_myName;
        }
        void binarize(asl::WriteableStream & theDest) {
            theDest.appendUnsigned32(static_cast<asl::Unsigned32>(DictMagic));
            theDest.appendUnsigned(_myList.size());
            for (EntryList::size_type i = 0; i < _myList.size(); ++i) {
                theDest.appendCountedString(_myList[i]->_myName);
            }
        }
        asl::AC_SIZE_TYPE
        debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            _myMap = EntryMap();
            _myList.resize(0);

            asl::Unsigned32 myMagic = 0;
            asl::AC_SIZE_TYPE theOldPos = thePos;
            thePos = theSource.readUnsigned32(myMagic, thePos);
            if (myMagic != DictMagic) {
                std::string myError = "Bad magic reading Dictionary, thePos=";
                myError += asl::as_string(thePos);
                myError += ", myMagic=";
                myError += asl::as_string((void*)myMagic);
                throw FormatCorrupted(myError, PLUS_FILE_LINE, theOldPos);
            }
            asl::Unsigned64 myCount;
            thePos = theSource.readUnsigned(myCount, thePos);

            for (int i = 0; i < myCount; ++i) {
                DOMString myName;
                thePos = theSource.readCountedString(myName, thePos);
                if (myName.size() == 0) {
                    throw FormatCorrupted("empty dictionary key",PLUS_FILE_LINE);
                }
                unsigned int myIndex;
                enterName(myName, myIndex);
            }
            return thePos;
        }
     private:
        EntryMap _myMap;
        EntryList _myList;
    };

#define NO_PATCH_STATISTIC
#ifdef PATCH_STATISTIC
    struct PatchStatistic {
        PatchStatistic() : newNodes(0), newValues(0), newNames(0), deletedNodes(0), unmodifiedNodes(0)
        {}

        unsigned newNodes;
        unsigned newValues;
        unsigned newNames;
        unsigned deletedNodes;
        unsigned unmodifiedNodes;

        void print() {
            AC_PRINT 
                << "newNodes = "<<newNodes
                <<", newValues = "<<newValues
                <<", newNames = "<<newNames
                <<", deletedNodes = "<<deletedNodes
                <<", unmodifiedNodes =" <<unmodifiedNodes
                <<std::endl;
        }
        ~PatchStatistic() {
            if (newNodes || newValues || newNames || deletedNodes || unmodifiedNodes) {
                print();
            }
        };
    };
#endif

    struct Dictionaries {
        Dictionaries() : isComplete(false) {}
        void binarize(asl::WriteableStream & theDest) {
            _myElementNames.binarize(theDest);
            _myAttributeNames.binarize(theDest);
        }
        asl::AC_SIZE_TYPE
        debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            thePos = _myElementNames.debinarize(theSource, thePos);
            thePos = _myAttributeNames.debinarize(theSource, thePos);
            isComplete = true;
            return thePos;
        };
        Dictionary _myElementNames;
        Dictionary _myAttributeNames;
        bool isComplete;
#ifdef PATCH_STATISTIC
        PatchStatistic _myPatchStat;
#endif
    };

    /* @} */
} //Namespace dom

#endif
