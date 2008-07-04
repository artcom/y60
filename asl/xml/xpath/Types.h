//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef XPATH_TYPES_H
#define XPATH_TYPES_H

#include <string>
#include <vector>
#include <set>
#include <functional>

#include <asl/Ptr.h>

#include <dom/Nodes.h>

namespace xpath {

    typedef dom::ThreadingModel                     ThreadingModel;

    typedef std::set<dom::NodePtr>                  NodeSet;
    typedef asl::Ptr<NodeSet,ThreadingModel>        NodeSetPtr;

    typedef std::vector<dom::NodePtr>               NodeVector;
    typedef asl::Ptr<NodeVector,ThreadingModel>     NodeVectorPtr;

    struct DocOrderLess : public std::binary_function<const dom::NodePtr, const dom::NodePtr, bool>
    {
        bool operator()(const dom::NodePtr Lhs, const dom::NodePtr Rhs) const;
    };

    std::string string_value_for(const dom::NodePtr);
    double number_value_for(const std::string &);

    inline
    double number_value_for(const dom::NodePtr n) {
        return number_value_for(string_value_for(n));
    }

    struct StringLess : public std::binary_function<const dom::NodePtr, const dom::NodePtr, bool>
    {
        bool operator()(const dom::NodePtr Lhs, const dom::NodePtr Rhs) const {
            return string_value_for(Lhs) < string_value_for(Rhs);
        }
    };

    struct NumberLess : public std::binary_function<const dom::NodePtr, const dom::NodePtr, bool>
    {
        bool operator()(const dom::NodePtr Lhs, const dom::NodePtr Rhs) const {
            return number_value_for(Lhs) < number_value_for(Rhs);
        }
    };

    typedef std::set<dom::NodePtr, DocOrderLess>    OrderedNodeSet;
    typedef asl::Ptr<OrderedNodeSet,ThreadingModel> OrderedNodeSetPtr;
};
#endif
