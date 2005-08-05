/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2003, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
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
#include <asl/MemoryPool.h>
#include <asl/Exception.h>

#include <string>
#include <vector>
#include <map>


namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */


    struct Entry : public asl::MemoryPool<Entry> {
        Entry(const std::string & theName, unsigned long theIndex) 
            :  _myName(theName), _myIndex(theIndex) {}
        DOMString _myName;    
        unsigned long _myIndex;
    };

    typedef asl::Ptr<Entry,ThreadingModel> EntryPtr;

    typedef std::map<DOMString,EntryPtr> EntryMap;
    typedef std::vector<EntryPtr> EntryList;

    class Dictionary {
    public:
        DEFINE_NESTED_EXCEPTION(Dictionary,IndexOutOfRange,asl::Exception);

        Dictionary() {}
        bool enterName(const DOMString & theName, unsigned long & theIndex) {
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
        const std::string & lookupName(unsigned long theIndex) {
            if (theIndex>_myList.size()) {
                throw IndexOutOfRange(JUST_FILE_LINE);
            }
            return _myList[theIndex]->_myName;
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
        Dictionary _myElementNames;
        Dictionary _myAttributeNames;
#ifdef PATCH_STATISTIC
        PatchStatistic _myPatchStat;
#endif
    };

    /* @} */
} //Namespace dom

#endif
