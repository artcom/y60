#ifndef __ATLCOMCLI_H__
#define __ATLCOMCLI_H__

#pragma once

namespace ATL {

template <class IU>
class CComPtr {
public:
    IU * p;
    
    CComPtr() : p(NULL) {
    }

    CComPtr(IU* q) : p(q) {
        if (p) {
            p->AddRef();
        }
    }

    CComPtr(const CComPtr<IU>& r) : p(r.p) {
        if (p) {
            p->AddRef();
        }
    }
    
    IU*
    operator -> () const {
        return p;
    }
    
    bool
    operator ! () const {
        return p==NULL;
    }

    IU**
    operator & () {
        return &p;
    }
    
    operator IU * () {
        return p;
    }
    
    void Release() {
        IU* t=p;
        if (t) {
            p=NULL;
            t->Release();
        }
    }
    
    IU* operator = (const IU* q) {
        Release();
        p=q;
        p->AddRef();
        return p;
    }
    
    CComPtr<IU> & operator = (const CComPtr<IU> & r) {
        Release();
        p=r.p;
        p->AddRef();
        return *this;
    }
    
    template <class QI>
    HRESULT
    QueryInterface(QI** pp) const {
        return p->QueryInterface(__uuidof(QI), (void**)pp);
    }
    
    HRESULT
    CoCreateInstance(const GUID & g, IUnknown* u=NULL,
                     unsigned long c=CLSCTX_ALL) {
        return ::CoCreateInstance(g, u, c, __uuidof(IU), (void**)&p);
    }

    ~CComPtr() {
        Release();
    }
};

template <class IU, const IID* g = &__uuidof(IU)>
class CComQIPtr : public CComPtr<IU>
{
public:
    CComQIPtr(IUnknown* q) {
        if (q != NULL) {
            q->QueryInterface(*g, (void **)&p);
        }
    }
};

}

#ifndef _ATL_NO_AUTOMATIC_NAMESPACE
using namespace ATL;
#endif // !_ATL_NO_AUTOMATIC_NAMESPACE

#endif // !__ATLCOMCLI_H__
