#ifndef _H_Script
#define _H_Script

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
 *	Name: Script.h
 *
 * 	Purpose: Declarations of the CLSID, class instance variables, and
 *           class interface(s) for an Xtra's scripting class.
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
 *     Script         <this file name>
 *     CDirectorLinkScript         <name of the class you defined>
 *
 *  3. Add and modify source code by looking for '--> insert -->' comment
 *  4. Create a Xtra.cpp file that contains the Xtra definition.
 ****************************************************************************/

/*****************************************************************************
 *  INCLUDE FILE(S)
 *  ---------------
 *	This .h file should automatically include all the support files needed for
 *	this particular class. In addition, this file may include other .h files
 *	defining additional callback interfaces for use by a third party.
 ****************************************************************************/
#ifndef _H_moaxtra
	#include "moaxtra.h"
#endif

#ifndef _H_mmiservc
	#include "mmiservc.h"
#endif

#ifndef _H_mmixscrp
	#include "mmixscrp.h"
#endif

#ifdef MACINTOSH
	#include <windows.h>
	#include <string.h>
#endif

/*****************************************************************************
 *  CONFIGURATION DEFINE(S)
 *  -----------------------
 *	Uncomment any appropriate lines below to implement the indicated Xtra
 *	interfaces within this class.
 ****************************************************************************/
//#define USING_INIT_FROM_DICT
//#define USING_NOTIFICATION_CLIENT

/*****************************************************************************
 *	CLASS SPECIFIC DEFINE(S)
 *	-----------------------
 *	Any #define's specific to this class
 ****************************************************************************/
#ifndef UNUSED
	#define UNUSED(x) x
#endif

/*****************************************************************************
 *	CLSID
 *	-----
 *	The CLSID is a GUID that unquely identifies your MOA class.  To generate a
 *	GUID, use the genUID.app utility (Mac) or the GUIDGEN.EXE utility (Win).
 *	The following line will produce a pre-compiler error if not replaced with
 *	a valid CLSID.
 ****************************************************************************/
// #error PLEASE DEFINE A NEW CLSID

// {9E4DBB58-E9D2-48e7-8B43-251014901FA8}
DEFINE_GUID(CLSID(CDirectorLinkScript),
0x9e4dbb58, 0xe9d2, 0x48e7, 0x8b, 0x43, 0x25, 0x10, 0x14, 0x90, 0x1f, 0xa8);

/*****************************************************************************
 *  CLASS INSTANCE VARIABLES
 *  ------------------------
 *  Class instance variable are variables whose scope is exclusive to the
 *  methods implemented by your MOA class.  Variables necessary for the
 *  implementation of your MOA class should be placed here.
 ****************************************************************************/
EXTERN_BEGIN_DEFINE_CLASS_INSTANCE_VARS(CDirectorLinkScript)
	/*
	 * ---> insert additional variable(s) -->
	 */
	PIMoaMmValue				pMmValue;
	PIMoaMmUtils2				pConsole;
EXTERN_END_DEFINE_CLASS_INSTANCE_VARS

/*****************************************************************************
 *  CLASS INTERFACE(S)
 *  ------------------
 *  The interface(s) implemented by your MOA class are specified here.  Note
 *	that at least one class in your Xtra should implement the IMoaRegister
 *	interface.
 *  NOTE: Because C++ does not use a lpVtbl to reference an interface's methods,
 *  the actual method declaration is done here.
 *
 *  Syntax:
 *  EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(<class-name>, <interface-name>)
 *		EXTERN_DEFINE_METHOD(<return-type>, <method-name>,(<argument-list>))
 *	EXTERN_END_DEFINE_CLASS_INTERFACE
 ****************************************************************************/
EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(CDirectorLinkScript, IMoaMmXScript)
	EXTERN_DEFINE_METHOD(MoaError, Call, (PMoaMmCallInfo))
   // definition of private member functions
   private:
   // PMoaDrCallInfo param is optional, include () if lingo call is to have no params...
	EXTERN_DEFINE_METHOD(MoaError, XScrpCallDirectorLink, (PMoaDrCallInfo))
	EXTERN_DEFINE_METHOD(MoaError, XScrpSetDirectorLink, (PMoaDrCallInfo))
	// EXTERN_DEFINE_METHOD(MoaError, XScrpGlobalHandler, (PMoaDrCallInfo))
	// EXTERN_DEFINE_METHOD(MoaError, XScrpParentHandler, (PMoaDrCallInfo))
	// EXTERN_DEFINE_METHOD(MoaError, XScrpChildHandler, (PMoaDrCallInfo))

EXTERN_END_DEFINE_CLASS_INTERFACE

#ifdef USING_INIT_FROM_DICT
EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(CDirectorLinkScript, IMoaInitFromDict)
	EXTERN_DEFINE_METHOD(MoaError, InitFromDict, (PIMoaRegistryEntryDict))
EXTERN_END_DEFINE_CLASS_INTERFACE
#endif

#ifdef USING_NOTIFICATION_CLIENT
EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(CDirectorLinkScript, IMoaNotificationClient)
	EXTERN_DEFINE_METHOD(MoaError, Notify, (ConstPMoaNotifyID, PMoaVoid, PMoaVoid))
EXTERN_END_DEFINE_CLASS_INTERFACE
#endif

/*
 * --> insert additional interface(s) -->
 */

/* 	MODIFY: This is the enumerated scripting method list. This list should
 *	directly correspond to the msgTable defined in an xxx.CPP file. It is used
 *	to dispatch method calls via the methodSelector. The 'm_XXXX' method must
 *	be last.
 */

enum
{
m_setDirectorLink,
m_callDirectorLink,

// m_new = 0,						/* standard */
// m_globalHandler,
// m_parentHandler,
// m_childHandler,

m_XXXX
};

#endif /* _H_Script */
