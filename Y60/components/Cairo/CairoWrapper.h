//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_CAIRO_REFCOUNTWRAPPER_INCLUDED_
#define _Y60_CAIRO_REFCOUNTWRAPPER_INCLUDED_

#include <map>

#include <asl/base/Ptr.h>

namespace jslib {

template <typename WRAPPED>
class CairoWrapper {
public:

    typedef CairoWrapper<WRAPPED> WRAPPER;
    typedef asl::WeakPtr<WRAPPER> WEAKPTR;
    typedef asl::Ptr<WRAPPER>     STRONGPTR;

    static WRAPPER* get(WRAPPED* theWrapped) {
        WRAPPER* myWrapper = _myWrappers[theWrapped];
        if(!myWrapper) {
            myWrapper = new WRAPPER(theWrapped);
            _myWrappers[theWrapped] = myWrapper;
        }
        return myWrapper;
    }

    CairoWrapper(WRAPPED *theWrapped) :
        _myWrapped(theWrapped), _myWeakReference() {
        reference();
    }

    ~CairoWrapper() {
        _myWrappers.erase(_myWrapped);
        unreference();
    }

    WRAPPED* getWrapped() {
        return _myWrapped;
    }

    STRONGPTR getWrappedPtr() {
        return _myWeakReference.lock();
    }

    void setSelfPtr(WEAKPTR thePtr) {
        _myWeakReference = thePtr;
    }

protected:

    static std::map<WRAPPED *, WRAPPER *> _myWrappers;

    WRAPPED               *_myWrapped;
    WEAKPTR                _myWeakReference;

    // dummy implementation
    virtual void reference();

    // dummy implementation
    virtual void unreference();
};

}

#endif
