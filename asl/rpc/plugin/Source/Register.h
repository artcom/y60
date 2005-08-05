#ifndef _H_Register
#define _H_Register

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
 *	Name: Register.h
 *
 * 	Purpose: Declarations of the CLSID, class instance variables, and
 *           class interface(s) for a generic Xtra Register class.
 *
 *
 *  HOW TO CUSTOMIZE THIS FILE
 *  --------------------------
 *  1. Save this file under a different file name.
 *  2. Use a search and replace utility (case sensitive) to replace the
 *     following:
 *
 *     Replace         With
 *     -------		   ----
 *     Register		   <this file name>
 *     CRegister       <name of the class you defined>
 *
 *  3. Add and modify source code by looking for '--> insert -->' comment
 *  4. Create a Xtra.cpp file that contains the Xtra definition.
 **********************************************************************************/

/*****************************************************************************
 *  XTRA SPECIFIC DEFINE(S)
 *  -----------------------
 *	Any #define's specific to this Xtra.
 *****************************************************************************/

 // ---> insert definitions here --->

/*****************************************************************************
 *  INCLUDE FILE(S)
 *  ---------------
 *	This .h file should automatically include all the support files needed for
 *  this particular class. In addition, this file may include other .h files
 *  that define additional callback interfaces for use by a third party.
 *****************************************************************************/
#ifndef _H_moaxtra
	#include "moaxtra.h"
#endif

#include "Script.h"

// ---> insert additional includes here --->

/*****************************************************************************
 *  CONFIGURATION DEFINE(S)
 *  -----------------------
 *	Uncomment the line below to activate debugging or load control code.
 ****************************************************************************/
//#define MOA_DEBUG
//#define USING_LOAD_CONTROL

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

// {FD7D1D25-16CA-4503-A2C7-8EA7B9BF440D}
DEFINE_GUID(CLSID(CDirectorLink),
0xfd7d1d25, 0x16ca, 0x4503, 0xa2, 0xc7, 0x8e, 0xa7, 0xb9, 0xbf, 0x44, 0xd);

/*****************************************************************************
 *  CLASS INSTANCE VARIABLES
 *  ------------------------
 *  Class instance variable are variables whose scope is exclusive to the
 *  methods implemented by your MOA class.  Variables necessary for the
 *  implementation of your MOA class should be places here.
 *****************************************************************************/
EXTERN_BEGIN_DEFINE_CLASS_INSTANCE_VARS(CDirectorLink)
	/*
	 * ---> insert additional variable(s) -->
	 */
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
EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(CDirectorLink, IMoaRegister)
	EXTERN_DEFINE_METHOD(MoaError, Register, (PIMoaCache, PIMoaDict))
EXTERN_END_DEFINE_CLASS_INTERFACE

/*
 * --> insert additional interface(s) -->
 */

#endif /* _H_Register */
