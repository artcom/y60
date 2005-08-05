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
 *	Name: Register.cpp
 *
 * 	Purpose: Definitions of Register class interface(s) and methods for a
 *           generic Xtra.
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
 *     Register        <this file name>
 *     CDirectorLink       <name of the class you defined>
 *
 *  3. Add and modify source code by looking for '--> insert -->' comment
 *  4. Create a Xtra.cpp file that contains the Xtra definition.
 ****************************************************************************/

/*****************************************************************************
 *  INCLUDE FILE(S)
 *  ---------------
 *	This .cpp file should automatically include all the support files needed
 *	for this particular class. In addition, this file may include other .h
 *	files defining additional callback interfaces for use by a third party.
 ****************************************************************************/
#ifndef INITGUID        /* INITGUID causes the actual GUIDs to be declared, */
	#define INITGUID      /* not just references to them. Must be declared in */
#endif                  /* only one file. */

#include "Register.h"


using namespace std;  // automatically added!



/*****************************************************************************
 *  INTERNAL PROTOTYPES(S)
 *  ----------------------
 *  Declarations for functions used specifically in this file.
 ****************************************************************************/

 // --> insert any static or global function prototype(s) here -->


/*****************************************************************************
 *  INTERNAL ROUTINE(S)
 *  -------------------
 *  Definition of file-specific functions
 ****************************************************************************/

// --> insert any static or global function implementation(s) here -->

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
BEGIN_DEFINE_CLASS_INTERFACE(CDirectorLink, IMoaRegister)
END_DEFINE_CLASS_INTERFACE

/*
 * --> insert additional interface(s) -->
 */

/*****************************************************************************
 *  Data needed for Registering
 *	---------------------------
 *	Specific code needed to register different types of Xtras.  The skeleton
 *	source files include minimal implementations appropriate for each Xtra
 *	type.  Current necessary actions:
 *
 *	Scripting Xtra:				Add the Scripting Xtra Message Table
 *	Sprite Asset Xtra:			Nothing
 *	Tool Xtra:					Nothing
 *	Transition Asset Xtra		Nothing
 *
 *  ****optional: Register as Safe for Shockwave!
 *****************************************************************************/

/*******************************************************************************
 * SCRIPTING XTRA MESSAGE TABLE DESCRIPTION.
 *
 * The general format is:
 * xtra <nameOfXtra>
 * new object me [ args ... ]
 * <otherHandlerDefintions>
 * --
 * The first line must give the name of the Scripting xtra.
 * The remaining lines give the names of the handlers that this xtra implements
 * along with the required types of the arguments for each handler.
 *
 * -- Pre-defined handler new
 * The new handler will be called when a child object is created,
 * the first argument is always the child object and you defined any remaining arguments.
 * The new handler is the place to initialize any memory required by the child object.
 *
 * -- Simple Handler Definitions
 * Each handler definition line is format like this:
 * <handlerName> <argType1> <argName1>, <argType2> <argName2> ...
 * The first word is the handler name. Following this are types description for
 * the argument to the handler, each separated by a comma.
 * The argument name <argName>, may be omited.
 * Permited argument types are:
 * 	integer
 * 	float
 * 	string
 * 	symbol
 * 	object
 * 	any
 * 	*
 * For integer, float, string, symbol, and object, the type of the argument must
 * match. The type any means allow any type. The asterisk (*) means any number and
 * any type of arguments.
 *
 * The first argument is the child object and is always declared to be of type object.
 *
 * -- Global Handlers
 * An asterisk (*) preceeding the handler name signifies a global handler.
 * This handler is at the global scope level and can be called from any
 * movie.
 *
 * -- Xtra level handlers
 * A plus (+) preceeding the handler name signifies an Xtra level handler.
 * This kind of handler can be called directly from the Xtra reference,
 * without creating a child object.
 *
 * The enumerated list in cscript.h must correspond directly with the msgTable
 * (i.e. they must be in the same order).
 *
 *******************************************************************************/

 /* MODIFY:  */

static char msgTable[] = {
	"xtra DirectorLink -- estabslishing a CORBA-based connection between Director and Y50\n"
	"* setDirectorLink string iorFilename, integer debugLevel -- set plugin behaviour and force plugin init\n"
	"* callDirectorLink string message -- send message to remote server\n"
	};

//	"-- Template handlers --\n"
//	"new object me\n"  /* standard first handler entry in all message tables */
//	"* globalHandler -- prints global handler message\n"
//	"+ parentHandler  object xtraRef -- prints parent handler message\n"
//	"childHandler object me -- prints child handler message\n"

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

/* ---------------------------------------------------- MoaCreate_CDirectorLink */
STDMETHODIMP MoaCreate_CDirectorLink(CDirectorLink FAR * This)
{
	UNUSED(This);

	MoaError		err = kMoaErr_NoErr;

	/*
	 * --> insert additional code -->
	 */

	return(err);
}

/* --------------------------------------------------- MoaDestroy_CDirectorLink */
STDMETHODIMP_(void) MoaDestroy_CDirectorLink(CDirectorLink FAR * This)
{
	UNUSED(This);

	/*
	 * --> insert additional code -->
	 */

	return;
}

/* ---------------------------------- CDirectorLink_IMoaRegister Create/Destroy */
CDirectorLink_IMoaRegister::CDirectorLink_IMoaRegister(MoaError FAR * pErr)
	{ *pErr = (kMoaErr_NoErr); }
CDirectorLink_IMoaRegister::~CDirectorLink_IMoaRegister() {}

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
 *****************************************************************************/

/* ------------------------------------------- CDirectorLink_IMoaRegister::Register */
STDMETHODIMP_(MoaError) CDirectorLink_IMoaRegister::Register(PIMoaCache pCache,
	PIMoaDict pXtraDict)
{

	/* variable declaration */
	MoaError 	err = kMoaErr_NoErr;
	PIMoaDict 	pRegDict;

	#ifdef USING_LOAD_CONTROL
	MoaLong		loadOptions = (kMoaXlc_PreloadAtLaunch | kMoaXlc_NeverUnload);
	err = pCache->AddRegistryEntry(pXtraDict, &CLSID_CInterrogate, &IID_IMoaXtraLoadControl, &pRegDict);
	err = pRegDict->Put(kMoaRegType_XtraLoadControlOptions, &loadOptions, 0, kMoaRegKey_XtraLoadControlOptions);
	#endif

	/* Register the scripting xtra */
	err = pCache->AddRegistryEntry(pXtraDict, &CLSID_CDirectorLinkScript, &IID_IMoaMmXScript, &pRegDict);

	if ( err == kMoaErr_NoErr)
	{
		/* Register the method table */
		err = pRegDict->Put(kMoaMmDictType_MessageTable, msgTable, 0, kMoaMmDictKey_MessageTable);

	}

	char buffer [1024];
	if ( err == kMoaErr_NoErr)
	{
		/* Popup Info box for debugging
		sprintf( buffer, "DirectorLink::Register:\nxtra registered\n");
		MessageBox( 0, buffer, "Xtra DirectorLink - Info", MB_ICONINFORMATION );
		*/
	} else {
		sprintf( buffer, "DirectorLink::Register:\nerror %x\n", err );
		MessageBox( 0, buffer, "Xtra DirectorLink - Error", MB_ICONERROR );
	}
	/*
	 * --> insert additional code -->
	 */

	return(err);
}

