// PLObject.h: interface for the CPLObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLOBJECT_H__E40881E3_C809_11D3_97BC_0050046F615E__INCLUDED_)
#define AFX_PLOBJECT_H__E40881E3_C809_11D3_97BC_0050046F615E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PLObject  
{
public:
	PLObject();
	virtual ~PLObject();

  virtual void AssertValid() const;
};

#endif // !defined(AFX_PLOBJECT_H__E40881E3_C809_11D3_97BC_0050046F615E__INCLUDED_)
