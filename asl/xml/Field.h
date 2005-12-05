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

#include "typedefs.h"
#include "Exceptions.h"
#include <vector>
#include <asl/Logger.h>


namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

    class Field;

    class CallBackBase {
    public:
        virtual void callback() = 0;
    };


    template <class IMPL>
    class CallBack : public CallBackBase {
    public:
        CallBack(asl::Ptr<IMPL, ThreadingModel> theCallBack, void (IMPL::*theCallBackFunction)()) 
            : _myCallBack(theCallBack),
              _myCallBackFunction(theCallBackFunction)
        {}
        void callback() {
           if (_myCallBack && _myCallBackFunction) {
                (*(_myCallBack.lock()).*_myCallBackFunction)();
            }
        }
    private:
        asl::WeakPtr<IMPL, ThreadingModel> _myCallBack; 
        void (IMPL::*_myCallBackFunction)();
    };

    class Field;
    typedef asl::Ptr<Field,ThreadingModel> FieldPtr;
    typedef asl::WeakPtr<Field,ThreadingModel> FieldWeakPtr;

    class ValueFactory;
    class Connector;
    class Facade;

    class Field {
        friend class ValueFactory;
        friend class Connector;
        friend class Facade;
    public:
        DEFINE_NESTED_EXCEPTION(Field, Exception, asl::Exception);
        DEFINE_NESTED_EXCEPTION(Field, InvalidDependent, Exception);
        DEFINE_NESTED_EXCEPTION(Field, InvalidNullPointerPassed, Exception);

        Field() : _isDirty(true), _isRecalculating(false), _hasOutdatedDependencies(true) {}        
        virtual ~Field();
        
        // This should only be called by ValueFactory and FacadeAttributePlug, could be private, as soon as we have
        // template friends.
        void setSelf(FieldPtr theSelf) {
            _mySelf = theSelf;
        }
        
        bool isDirty() const {
            return _isDirty;
        }

        // Field Connection Protocol
        void registerPrecursor(FieldPtr theField);
        void unregisterPrecursor(FieldPtr theField);

        bool hasPrecursor(FieldPtr theField);
        bool hasDependend(FieldWeakPtr theField);

        void markPrecursorDependenciesOutdated() {
            clearPrecursorFields();
            _hasOutdatedDependencies = true;
            _isDirty = true;
        };
        void markPrecursorDependenciesUpToDate() {
            _hasOutdatedDependencies = false;
        };

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
            _myCalculator = asl::Ptr<CallBackBase, ThreadingModel>(new CallBack<CALCULATOR>(theCalculator, theCalculateFunction));
            markPrecursorDependenciesUpToDate();
        }

        void setReconnectFunction(asl::Ptr<CallBackBase, ThreadingModel> theCallBack);

        void transferDependenciesFrom(Field & theField);
        void clearPrecursorFields();
        void clearDependendFields();

        void printDependendGraph(const std::string & myPrefix="") const;
        void printPrecursorGraph(const std::string & myPrefix="") const;

        bool hasOutdatedDependencies() const {
            return _hasOutdatedDependencies;
        };
        void ensureDependencies() const;
    protected:
        void onGetValue() const;
        void onSetValue();
    private:
        unsigned  findDependend(FieldWeakPtr theField) const;
        unsigned  findPrecursor(FieldPtr theField) const;
        void recalculate() {
            if (_myCalculator) {
                _myCalculator->callback();
            }
        }
        void reconnect();
        void setDirty();
        void registerDependend(FieldPtr theField);
        void unregisterDependend(FieldWeakPtr theField);
        void eraseDependend(FieldWeakPtr theField);
        void erasePrecursor(FieldPtr theField);

        void setDependentsDirty();

        std::vector<FieldPtr>     _myPrecursorFields;
        std::vector<FieldWeakPtr> _myDependendFields;
        mutable bool              _isDirty;
        mutable bool              _hasOutdatedDependencies;
        mutable bool              _isRecalculating;
        FieldWeakPtr              _mySelf;
        asl::Ptr<CallBackBase, ThreadingModel>  _myCalculator;
        asl::Ptr<CallBackBase, ThreadingModel>  _myConnector;
    };
    /* @} */
} //Namespace dom



#endif
