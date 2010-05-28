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
//   $RCSfile: AttributePlug.h,v $
//
//   $Revision: 1.15 $
//
//   Description:
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _xml_PlugHelper_h_
#define _xml_PlugHelper_h_

#include "asl_dom_settings.h"

#include "Nodes.h"

namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

    class PlugBase {
    public:
        virtual ~PlugBase() {}
        virtual const ValuePtr getDefaultValue(const Node & theNode) const = 0;
        virtual const ValuePtr getValuePtr() const = 0;
    };

    template <class VALUE, class WRAPPER>
    struct ValueHelper {
       typedef asl::Ptr<WRAPPER,ThreadingModel> WrapperPtr;

       static const VALUE & getValue(const ValuePtr theValueHolder) {
           const WrapperPtr myTypedValuePtr = static_cast_Ptr<WRAPPER>(theValueHolder);
           return myTypedValuePtr->getValue();
       }
       static const VALUE & setValue(ValuePtr theValueHolder, const VALUE & theValue) {
           WrapperPtr myTypedValuePtr = static_cast_Ptr<WRAPPER>(theValueHolder);
           myTypedValuePtr->setValue(theValue);
           return myTypedValuePtr->getValue();
       }
       static bool checkValueType(const ValuePtr theValueHolder) {
           const WrapperPtr myTypedValuePtr = dynamic_cast_Ptr<WRAPPER>(theValueHolder);
           return myTypedValuePtr != 0;
       }
    };

    template <>
    struct ValueHelper <std::string,StringValue> {
        typedef asl::Ptr<StringValue,ThreadingModel> WrapperPtr;

        static const std::string & getValue(const ValuePtr theValueHolder) {
            const WrapperPtr myTypedValuePtr = static_cast_Ptr<StringValue>(theValueHolder);
            return myTypedValuePtr->getString();
        }
        static const std::string & setValue(ValuePtr theValueHolder, const std::string & theValue) {
            WrapperPtr myTypedValuePtr = static_cast_Ptr<StringValue>(theValueHolder);
            myTypedValuePtr->setString(theValue);
            return myTypedValuePtr->getString();
        }
        static bool checkValueType(const ValuePtr theValueHolder) {
           const WrapperPtr myTypedValuePtr = dynamic_cast_Ptr<StringValue>(theValueHolder);
           return myTypedValuePtr != 0;
        }
    };

    /* @} */
}

#endif
