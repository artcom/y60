/*****************************************************************************
 * Copyright © 1994-1999 Macromedia, Inc.  All Rights Reserved
 *
 * You may utilize this source file to create and compile object code for use
 * within products you create.  THIS CODE IS PROVIDED "AS IS", WITHOUT
 * WARRANTY OF ANY KIND, AND MACROMEDIA DISCLAIMS ALL IMPLIED WARRANTIES
 * INCLUDING, BUT NOT LIMITED TO, MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT.  IN NO EVENT WILL MACROMEDIA BE LIABLE TO YOU
 * FOR ANY CONSEQUENTIAL, INDIRECT OR INCIDENTAL DAMAGES ARISING OUT OF YOUR
 * USE OR INABILITY TO USE THIS CODE.
 *
 *	Name: Script.cpp
 *
 * 	Purpose: Definitions of scripting class interface(s) and methods for the
 *           Scripting Xtra skeleton project.
 *
 *
 *  HOW TO CUSTOMIZE THIS FILE
 *  --------------------------
 *  1. Save this file under a different file name.
 *  2. Use a search and replace utility (case sensitive) to replace the
 *     following:
 *
 *     Replace         With
 *     -------         ----
 *     Script      <this file name>
 *     CDirectorLinkScript      <name of the class you defined>
 *
 *  3. Add and modify source code by looking for '--> insert -->' comment
 ****************************************************************************/

/*****************************************************************************
 *  INCLUDE FILE(S)
 *  ---------------
 *	This .cpp file should automatically include all the support files needed
 *	for this particular class. In addition, this file may include other .h
 *	files defining additional callback interfaces for use by a third party.
 ****************************************************************************/

#include "Script.h"

#ifdef MACINTOSH
	#include <windows.h>
	#include <string.h>
#endif
#include <mmiutil.h>

#include "Corba.h"


using namespace std;  // automatically added!



/*****************************************************************************
 *  Private Methods
 *  -------------------
 *  Implementation of Private Class Methods
 ****************************************************************************/

/* ----------------------------------------------------- XScrpGlobalHandler */

// MoaError CDirectorLinkScript_IMoaMmXScript::XScrpGlobalHandler(PMoaDrCallInfo callPtr)
// {
//	UNUSED(callPtr);

	/* variable declarations */
//	MoaError err = kMoaErr_NoErr;

	/*
	 *  --> insert additional code -->
	 */

//	return(err);
// }


/* ----------------------------------------------------- XScrpParentHandler */
// MoaError CDirectorLinkScript_IMoaMmXScript::XScrpParentHandler(PMoaDrCallInfo callPtr)
// {
//	UNUSED(callPtr);

	/* variable declarations */
//	MoaError err = kMoaErr_NoErr;

	/*
	 *  --> insert additional code -->
	 */

//	return(err);
// }

/* ------------------------------------------------------ XScrpChildHandler */
// MoaError CDirectorLinkScript_IMoaMmXScript::XScrpChildHandler(PMoaDrCallInfo callPtr)
// {
//	UNUSED(callPtr);

	/* variable declarations */
//	MoaError err = kMoaErr_NoErr;

	/*
	 *  --> insert additional code -->
	 */

//	return(err);
// }


// ---------------------------------------------------------------------------

static int initModule = 0;	// module already initialized?


/* ------------------------------------------------------ XScrpSetDirectorLink */

MoaError CDirectorLinkScript_IMoaMmXScript::XScrpSetDirectorLink (PMoaDrCallInfo callPtr)
{

	// variable declarations
	MoaError err = kMoaErr_NoErr;	// XXX what error could occur? 	e.g. kMoaErr_BadParam

	MoaMmValue	moaValue;

	// access iorFilename
	// AccessArgByIndex(1, &moaValue);
	pciGetArgByIndex (callPtr, 1, &moaValue);
	pObj->pMmValue->ValueToString( &moaValue, iorFilename, MAX_FILENAME_SIZE);

	// access debugLevel
	pciGetArgByIndex (callPtr, 2, &moaValue);
	pObj->pMmValue->ValueToInteger ( &moaValue, &debugLevel);

	if (debugLevel >= 1) {
		pObj->pConsole->PrintMessage1 ("DirectorLink::initDirectorLink - iorFilename: %s\n", (MoaLong) iorFilename);
		pObj->pConsole->PrintMessage1 ("DirectorLink::initDirectorLink - debugLevel: %d\n", (MoaLong) debugLevel);
	}


	// force new initialization
	initModule = 0;

	return (err);
}

/* ------------------------------------------------------ XScrpCallDirectorLink */

MoaError CDirectorLinkScript_IMoaMmXScript::XScrpCallDirectorLink (PMoaDrCallInfo callPtr)
{

	// variable declarations
	MoaError err = kMoaErr_NoErr;		// XXX: kMoaErr_BadParam?

	MoaMmValue	moaValue;

	#define MAX_MSG_SIZE 10000
	MoaChar	errorMessage [MAX_MSG_SIZE];
	MoaChar	inMessage [MAX_MSG_SIZE];
	MoaChar	outMessage [MAX_MSG_SIZE];

	// AccessArgByIndex(1, &moaValue);
	pciGetArgByIndex (callPtr, 1, &moaValue);
	pObj->pMmValue->ValueToString( &moaValue, inMessage, MAX_MSG_SIZE);

    if (debugLevel >= 2)
		pObj->pConsole->PrintMessage1("DirectorLink::CallDirectorLink - debug inMessage: %s\n", (MoaLong) inMessage);

	if ( !initModule ) {
		if ( initDirectorLinkCORBA (errorMessage)) {
			if (debugLevel >= 1)
				pObj->pConsole->PrintMessage("DirectorLink::CallDirectorLink - initDirectorLinkCORBA succeeded\n");
			initModule = 1;
		} else {
			if (debugLevel >= 1)
				pObj->pConsole->PrintMessage1("DirectorLink::CallDirectorLink - initDirectorLinkCORBA failed: %s\n", (MoaLong) errorMessage);
			pObj->pMmValue->StringToValue( errorMessage, &callPtr->resultValue);
		}
	} 
	
	if (initModule) {
		if (! callDirectorLinkCORBA( inMessage, outMessage)) {
			initModule = 0; // force new initialization
		}
		pObj->pMmValue->StringToValue( outMessage, &callPtr->resultValue);
	}

    if (debugLevel >= 2)
		pObj->pConsole->PrintMessage1("DirectorLink::CallDirectorLink - debug outMessage: %s\n", (MoaLong) outMessage);

	return (err);
}



/*****************************************************************************
 *  CLASS INTERFACE(S)
 *  ------------------
 *  The interface(s) implemented by your MOA class are specified here.  Note
 *	that at least one class in your Xtra should implement the IMoaRegister
 *	interface.
 *  NOTE: Because C++ does not use a lpVtbl to reference an interface's
 *	methods, the actual method declaration is done in the .h file.
 *
 *  Syntax:
 *  BEGIN_DEFINE_CLASS_INTERFACE(<class-name>, <interface-name>)
 ****************************************************************************/
BEGIN_DEFINE_CLASS_INTERFACE(CDirectorLinkScript, IMoaMmXScript)
END_DEFINE_CLASS_INTERFACE

#ifdef USING_INIT_FROM_DICT
BEGIN_DEFINE_CLASS_INTERFACE(CDirectorLinkScript, IMoaInitFromDict)
END_DEFINE_CLASS_INTERFACE
#endif

#ifdef USING_NOTIFICATION_CLIENT
BEGIN_DEFINE_CLASS_INTERFACE(CDirectorLinkScript, IMoaNotificationClient)
END_DEFINE_CLASS_INTERFACE
#endif

/*
 * --> insert additional method(s) -->
 */

/*****************************************************************************
 *  CREATE AND DESTROY METHODS
 *  --------------------------
 *  Every interface and class has an associated 'Create' and 'Destroy' pair.
 *  'Create' methods are typically used to acquire interface(s), allocate
 *  memory, and intialize variables. 'Destroy' methods are typically used to
 *  release interfaces and free memory.
 *
 * NOTE:  In C++, the local variable 'This' is provided implicitly within
 * a method implementation.  Thus, there is no need explicitly declare 'This'
 * as a function parameter. However, this implementation detail doesn’t apply
 * to the MOA class creator and destructor functions, which are standard C
 * functions, coded exactly as in like they are in C examples.
 *
 * Class Syntax:
 * STDMETHODIMP MoaCreate_<class-name>(<class-name> FAR * This)
 * STDMETHODIMP MoaDestroy_<class-name>(<class-name> FAR * This)
 *
 * Interface Syntax:
 * <class_name>_<if_name>::<class_name>_<if_name>(MoaError FAR * pErr)
 * <class_name>_<if_name>::~<class_name>_<if_name>()
 ****************************************************************************/

/* ------------------------------------------------------ MoaCreate_CDirectorLinkScript */
STDMETHODIMP MoaCreate_CDirectorLinkScript(CDirectorLinkScript FAR * This)
{
	// UNUSED(This);

	/* variable declarations */
	MoaError err = kMoaErr_NoErr;

	// X_ENTER

	/* variable declarations */
	err = kMoaErr_NoErr;
	
	This->pCallback->QueryInterface(&IID_IMoaMmValue, (PPMoaVoid)&This->pMmValue);

	This->pCallback->QueryInterface(&IID_IMoaMmUtils2, (PPMoaVoid)&This->pConsole);

    if (debugLevel >= 1)
		This->pConsole->PrintMessage("DirectorLinkScript:: create called\n");
	/*
	 * --> insert additional code -->
	 */

	// X_STD_RETURN(err);	
	// X_EXIT
	/*
	 * --> insert additional code -->
	 */

	return(err);
}

/* ----------------------------------------------------- MoaDestroy_CDirectorLinkScript */
STDMETHODIMP_(void) MoaDestroy_CDirectorLinkScript(CDirectorLinkScript FAR * This)
{
	UNUSED(This);

	X_ENTER
	
	if (This->pMmValue) This->pMmValue->Release();
	/*
	 * --> insert additional code -->
	 */
	
	X_RETURN_VOID;
	X_EXIT
	/*
	 * --> insert additional code -->
	 */

	return;
}

/* ----------------------------------- CDirectorLinkScript_IMoaMmXScript Create/Destroy */
CDirectorLinkScript_IMoaMmXScript::CDirectorLinkScript_IMoaMmXScript(MoaError FAR * pErr)
	{ *pErr = (kMoaErr_NoErr); }
CDirectorLinkScript_IMoaMmXScript::~CDirectorLinkScript_IMoaMmXScript() {}

#ifdef USING_INIT_FROM_DICT
/* -------------------------------- CDirectorLinkScript_IMoaInitFromDict Create/Destroy */
CDirectorLinkScript_IMoaInitFromDict::CDirectorLinkScript_IMoaInitFromDict(MoaError FAR * pErr)
	{ *pErr = (kMoaErr_NoErr); }
CDirectorLinkScript_IMoaInitFromDict::~CDirectorLinkScript_IMoaInitFromDict() {}
#endif

#ifdef USING_NOTIFICATION_CLIENT
/* -------------------------- CDirectorLinkScript_IMoaNotificationClient Create/Destroy */
CDirectorLinkScript_IMoaNotificationClient::CDirectorLinkScript_IMoaNotificationClient(MoaError FAR * pErr)
	{ *pErr = (kMoaErr_NoErr); }
CDirectorLinkScript_IMoaNotificationClient::~CDirectorLinkScript_IMoaNotificationClient() {}
#endif

/*
 * --> insert additional create/destroy method(s) -->
 */

/*****************************************************************************
 *  METHOD IMPLEMENTATION(S)
 *  ------------------------
 *  This is where the methods to be defined by your MOA class are implemented.
 *  The bulk of the work in implementing Xtras is done here.  NOTE: 'This' is
 *  implemented implicitly in C++, therefore it isn't used in the argument-
 *	list.
 *
 *  Syntax:
 *  STDMETHODIMP <class-name>_<interface-name>::<method-name>(<argument-list>)
 ****************************************************************************/

/* -------------------------------------------- CDirectorLinkScript_IMoaMmXScript::Call */
STDMETHODIMP CDirectorLinkScript_IMoaMmXScript::Call(PMoaMmCallInfo callPtr)
{

	/* variable declarations */
	MoaError err = kMoaErr_NoErr;

	switch	( callPtr->methodSelector )
	{
		case m_setDirectorLink:
			err = XScrpSetDirectorLink(callPtr);
			break;
	
		case m_callDirectorLink:
			//MessageBox( 0, "m_callDirectorLink", "Xtra", MB_ICONSTOP );
			err = XScrpCallDirectorLink(callPtr);
			break;

//		case m_new:
//			/*
//			 * --> insert additional code -->
//		 	 */
//			break;

//		case m_globalHandler:
//			err = XScrpGlobalHandler(callPtr);
//			break;

//		case m_parentHandler:
//			err = XScrpParentHandler(callPtr);
//			break;

//		case m_childHandler:
//			err = XScrpChildHandler(callPtr);
//			break;
//		break;

		/*
		 * --> insert additional methodSelector cases -->
		 */
	}

	return(err);
}

#ifdef USING_INIT_FROM_DICT
/* --------------------------------- CDirectorLinkScript_IMoaInitFromDict::InitFromDict */
STDMETHODIMP CDirectorLinkScript_IMoaInitFromDict::InitFromDict(PIMoaRegistryEntryDict pRegistryDict)
{
	UNUSED(pRegistryDict);

	/* variable declarations */
	MoaError err = kMoaErr_NoErr;

	/*
	 *  --> insert additional code -->
	 */

	return(err);
}
#endif

#ifdef USING_NOTIFICATION_CLIENT
/* --------------------------------- CDirectorLinkScript_IMoaNotificationClient::Notify */
STDMETHODIMP CDirectorLinkScript_IMoaNotificationClient::Notify(ConstPMoaNotifyID nid, PMoaVoid pNData, PMoaVoid pRefCon)
{
	UNUSED(nid);
	UNUSED(pNData);
	UNUSED(pRefCon);

	/* variable declarations */
	MoaError err = kMoaErr_NoErr;

	/*
	 *  --> insert additional code -->
	 */

	return(err);
}
#endif

