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

#include "Nodes.h"

namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

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
