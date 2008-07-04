//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "Types.h"

#include <utility>

#include <dom/Nodes.h>

namespace {

    typedef std::vector<const dom::NodePtr>         ParentList;
    typedef ParentList::const_reverse_iterator      ParentListIter;
    typedef std::pair<ParentListIter,ParentListIter>DiffResult;

    template< typename FwdIter >
    bool compareFindResult( FwdIter itLhs, FwdIter endLhs
                          , FwdIter itRhs, FwdIter endRhs )
    {
        if( itRhs == endRhs ) {
            return false; // since Lhs != Rhs, Rhs is an ancestor of Lhs.
        }
        if (itLhs != endLhs) {
            // Lhs shares Lhs parent with Rhs.
            for(dom::NodePtr tmpNode=*itRhs; tmpNode; tmpNode=tmpNode->nextSibling()) {
                if (tmpNode == *itLhs) {
                    return false; // Rhs is before Lhs
                }
            }
        }
        // Lhs != Rhs, Rhs is not an ancestor of Lhs, Rhs is not before Lhs:
        // Rhs is descendant of Lhs OR Rhs is before Lhs.
        return true;
    }

    ParentList getParentList(dom::NodePtr n)
    {
        ParentList result;
        while(n) {
            result.push_back(n);
            n = n->parentNode()->self().lock();
        }
        return result;
    }

    template< typename FwdIter >
    DiffResult find_firstDiff( FwdIter beginLhs, FwdIter endLhs
                             , FwdIter beginRhs, FwdIter endRhs )
    {
        while( beginLhs!=endLhs && beginRhs!=endRhs && *beginLhs==*beginRhs ) {
            ++beginLhs;
            ++beginRhs;
        }
        return DiffResult(beginLhs,beginRhs);
    }

}

namespace xpath {

    //struct DocOrderLess : public std::binary_function<const dom::NodePtr, const dom::NodePtr, bool>
    //{
    bool DocOrderLess::operator()(const dom::NodePtr Lhs, const dom::NodePtr Rhs) const {
            if (Lhs == Rhs) {
                return false;
            }

            const ParentList& parentsLhs = getParentList(Lhs);
            const ParentList& parentsRhs = getParentList(Rhs);

            const DiffResult& firstDiff = find_firstDiff( parentsLhs.rbegin(), parentsLhs.rend()
                                                        , parentsRhs.rbegin(), parentsRhs.rend() );

            return compareFindResult( firstDiff.first , parentsLhs.rend()
                                    , firstDiff.second, parentsRhs.rend() );
        }

    //};

    std::string
    string_value_for(const dom::NodePtr n) {
        std::string retval;
        switch(n->nodeType()) {
            case dom::Node::DOCUMENT_NODE:
            case dom::Node::ELEMENT_NODE:
                for (int i = 0; i < n->childNodesLength(); i++) {
                    retval += string_value_for(n->childNode(i));
                }
                break;
            case dom::Node::ATTRIBUTE_NODE:
            case dom::Node::TEXT_NODE:
                retval = n->nodeValue();
                break;
            default:
                AC_ERROR << " could not determine string_value for " << n->nodeType();
                retval = "";
        }
        return retval;
    }

    double
    number_value_for(const std::string &s) {
        std::istringstream iss(s);
        double num;
        iss >> std::ws >> num >> std::ws;
        if ( !iss ) {
            return 0;
        }
        return num;
    }

}
