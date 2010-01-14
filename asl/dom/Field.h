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
//    $RCSfile: Value.h,v $
//
//   $Revision: 1.34 $
//
// Description: tiny fast XML-Parser and DOM
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#ifndef _xml_Field_h_
#define _xml_Field_h_

#include "asl_dom_settings.h"

#include "typedefs.h"
#include "Exceptions.h"
#include <vector>
#include <asl/base/Logger.h>


namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

    class CallBackBase {
        public:
            virtual void callback() = 0;
            virtual ~CallBackBase() {}
    };


    template <class IMPL>
    class CallBack : public CallBackBase {
        public:
            CallBack(asl::Ptr<IMPL, ThreadingModel> theCallBack, void (IMPL::*theCallBackFunction)())
                : _myCallBack(theCallBack),
                _myCallBackFunction(theCallBackFunction)
            {}
            void callback() {
                if (!_myCallBack.expired() && _myCallBackFunction) {
                    (*(_myCallBack.lock()).*_myCallBackFunction)();
                }
            }
        private:
            asl::WeakPtr<IMPL, ThreadingModel> _myCallBack;
            void (IMPL::*_myCallBackFunction)();
    };

    class Field;

    typedef asl::Ptr<Field, ThreadingModel> FieldPtr;
    typedef asl::WeakPtr<Field, ThreadingModel> FieldWeakPtr;
    typedef asl::Ptr<CallBackBase, ThreadingModel> CallBackPtr;

    class ValueFactory;
    class Connector;
    class Facade;

    class ASL_DOM_DECL Field {
        friend class ValueFactory;
        friend class Connector;
        friend class Facade;
    public:
        DEFINE_NESTED_EXCEPTION(Field, Exception, asl::Exception);
        DEFINE_NESTED_EXCEPTION(Field, InvalidDependent, Exception);
        DEFINE_NESTED_EXCEPTION(Field, InfiniteRecursion, Exception);
        DEFINE_NESTED_EXCEPTION(Field, InvalidNullPointerPassed, Exception);

        Field() : _isDirty(true), _hasOutdatedDependencies(true), _isRecalculating(false), _isReconnecting(false), _myImmediateCB() {}
        virtual ~Field();

        // This should only be called by ValueFactory and FacadeAttributePlug, could be private, as soon as we have
        // template friends.
        void setSelf(FieldPtr theSelf) {
            _mySelf = theSelf;
        }

        bool isDirty() const {
            return _isDirty;
        }

        // Only use this if you really know what you are doing...
        void setClean() {
            _isDirty = false;
        }

        // This function is only for experts in dependency networking!
        void setDirty() {
            if (!_isDirty) {
                _isDirty = true;
                setDependentsDirty();
            }
        }

        // Field Connection Protocol
        void registerPrecursor(FieldPtr theField);
        void unregisterPrecursor(FieldPtr theField);

        bool hasPrecursor(FieldPtr theField);
        bool hasDependend(FieldWeakPtr theField);

        void markPrecursorDependenciesOutdated() {
            clearPrecursorFields();
            _hasOutdatedDependencies = true;
            setDirty();
        }

        template <class CALCULATOR>
        void setCalculatorFunction(asl::Ptr<CALCULATOR, ThreadingModel> theCalculator, void (CALCULATOR::*theCalculateFunction)()) {
            if (!theCalculator) {
                AC_ERROR << "Field::setCalculatorFunction: theCalculator is null";
                throw InvalidNullPointerPassed(JUST_FILE_LINE);
            }
            if (!theCalculateFunction) {
                AC_ERROR << "Field::setCalculatorFunction: theCalculateFunction is null";
                throw InvalidNullPointerPassed(JUST_FILE_LINE);
            }
            _myCalculator = CallBackPtr(new CallBack<CALCULATOR>(theCalculator, theCalculateFunction));

        }
        void setCalculatorFunction(CallBackPtr theCalculator) {
            _myCalculator = theCalculator;
        }

        CallBackPtr getCalculatorFunction() const {
            return _myCalculator;
        }


		template <class OBJECT>
        void setImmediateCallBack(asl::Ptr<OBJECT, ThreadingModel> theObject, void (OBJECT::*theFunction)()) {
            if (!theObject) {
                AC_ERROR << "Field::setImmediateCallBack: theObject is null";
                throw InvalidNullPointerPassed(JUST_FILE_LINE);
            }
            if (!theFunction) {
                AC_ERROR << "Field::setImmediateCallBack: theFunction is null";
                throw InvalidNullPointerPassed(JUST_FILE_LINE);
            }
            _myImmediateCB = CallBackPtr(new CallBack<OBJECT>(theObject, theFunction));
        }
        void setImmediateCallBack(CallBackPtr theCallback) {
             _myImmediateCB = theCallback;
        }
        CallBackPtr getImmediateCallBack() const {
            return _myImmediateCB;
        }

        void setReconnectFunction(CallBackPtr theCallBack);
        CallBackPtr geReconnectFunction() const {
            return _myConnector;
        }

        void transferDependenciesFrom(Field & theField);
        void clearPrecursorFields();
        void clearDependendFields();

        void printDependendGraph(const std::string & myPrefix="") const;
        void printPrecursorGraph(const std::string & myPrefix="") const;

        bool hasOutdatedDependencies() const {
            return _hasOutdatedDependencies;
        }
        void ensureDependencies() const;
    protected:
        void onGetValue() const;
        void onSetValue();
    private:
        unsigned  findDependend(FieldWeakPtr theField) const;
        unsigned  findPrecursor(FieldPtr theField) const;

        void registerDependend(FieldPtr theField);
        void eraseDependend(FieldWeakPtr theField);
        void erasePrecursor(FieldPtr theField);

        void setDependentsDirty();

        std::vector<FieldPtr>     _myPrecursorFields;
        std::vector<FieldWeakPtr> _myDependendFields;
        mutable bool              _isDirty;
        mutable bool              _hasOutdatedDependencies;
        mutable bool              _isRecalculating;
        mutable bool              _isReconnecting;
        FieldWeakPtr              _mySelf;
        CallBackPtr               _myCalculator;
        CallBackPtr               _myConnector;
        CallBackPtr               _myImmediateCB;
    };
    /* @} */
} //Namespace dom



#endif
