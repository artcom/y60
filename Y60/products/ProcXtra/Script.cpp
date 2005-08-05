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
*     Script          <this file name>
*     CProcScript     <name of the class you defined>
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

// This needs to be the first include to avoid duplicate definitions in
// winsock.h and winsock2.h.

#include "Script.h"

#ifdef MACINTOSH
	#include <windows.h>
	#include <string.h>
#endif


/*****************************************************************************
*  Private Methods
****************************************************************************/

static void PrintErrorMessage(const char* theFormat, const char* theArgument,
							  CProcScript* pObj)
{
	DWORD myError = GetLastError();
	if (myError == ERROR_SUCCESS) {
		return;
	}

	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		myError,
		0/*MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)*/,
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
	);

	char myMessage[2048];
	if (theArgument) {
        sprintf(myMessage, theFormat, theArgument);
	}
	else {
		strcpy(myMessage, theFormat);
	}
	strcat(myMessage, (LPCSTR) lpMsgBuf);
	pObj->pConsole->PrintMessage(myMessage);

	LocalFree(lpMsgBuf);
}

MoaError CProcScript_IMoaMmXScript::start (PMoaDrCallInfo callPtr)
{
	MoaError err = kMoaErr_NoErr;
	MoaMmValue	moaValue;

	// working directory
	char myDirectory[1024];
	pciGetArgByIndex (callPtr, 1, &moaValue);
	pObj->pMmValue->ValueToString(&moaValue, myDirectory, sizeof(myDirectory));

	LPCTSTR myDirectoryPtr = NULL;
	if (myDirectory[0] != '\0') {
		myDirectoryPtr = myDirectory;
	}

	// commandline
	char myCommandLine[1024];
	pciGetArgByIndex (callPtr, 2, &moaValue);
	pObj->pMmValue->ValueToString(&moaValue, myCommandLine, sizeof(myCommandLine));

	STARTUPINFO myStartupInfo = {
		sizeof(STARTUPINFO),
		NULL,					// lpReserved
		NULL,					// lpDesktop
		NULL,					// lpTitle
		0,0,					// dwX, dwY
		0,0,					// dwXSize, dwYSize
		0,0,					// dwXCountChars, dwYCountChars
		0,						// dwFillAttribute
		STARTF_USESHOWWINDOW,
		SW_SHOWMINNOACTIVE,		// wShowWindow
		0,						// cbReserved2
		NULL,					// lpReserved2
		NULL,					// hStdInput
		NULL,					// hStdOutput
		NULL					// hStdError
	};
	PROCESS_INFORMATION myProcessInfo;

	bool myResult = CreateProcess(NULL,				// No module name (use command line). 
								  myCommandLine,	// Command line.
								  NULL,				// Process handle not inheritable.
								  NULL,				// Thread handle not inheritable.
								  FALSE,			// Set handle inheritance to FALSE.
								  0,				// Creation flags.
								  NULL,				// Use parent's environment block.
								  myDirectoryPtr,	// Use parent's starting directory.
								  &myStartupInfo,
								  &myProcessInfo);
	if (!myResult) {
		PrintErrorMessage("ProcStart: Unable to start process: %s", myCommandLine, pObj);
		err = kMoaErr_BadParam;
	}
	else {
		err = kMoaErr_NoErr;
	}

	// returns PID
	long myPID = long(myProcessInfo.dwProcessId);
	pObj->pMmValue->IntegerToValue(myPID, &callPtr->resultValue);

	return err;
}

MoaError CProcScript_IMoaMmXScript::reparent (PMoaDrCallInfo callPtr)
{
	MoaError err = kMoaErr_NoErr;
	MoaMmValue	moaValue;

	// process ID
	long myPID;
	pciGetArgByIndex (callPtr, 1, &moaValue);
	pObj->pMmValue->ValueToInteger(&moaValue, &myPID);

	// viewer window name
	char myViewerName[1024];
	pciGetArgByIndex (callPtr, 2, &moaValue);
	pObj->pMmValue->ValueToString(&moaValue, myViewerName, sizeof(myViewerName));

	// director window name
	char myDirectorName[1024];
	pciGetArgByIndex (callPtr, 3, &moaValue);
	pObj->pMmValue->ValueToString(&moaValue, myDirectorName, sizeof(myDirectorName));

	// find window handle
	HWND myWindow = NULL;
	for (unsigned int i = 0; i < 50; ++i) {
		myWindow = FindWindow(NULL, myViewerName);
		if (myWindow) {
			break;
		}
		Sleep(500); // ms
	}
	if (myWindow == NULL)  {
		PrintErrorMessage("ProcReparent: Unable to find window: '%s'\n", myViewerName, pObj);
	}

	// find Director window
	HWND myDirectorWindow = FindWindow(NULL, myDirectorName);
	if (myDirectorWindow == NULL) {
		PrintErrorMessage("ProcReparent: Unable to find window: '%s'\n", myDirectorName, pObj);
	}

	// reparent window
	if (myWindow && myDirectorWindow && SetParent(myWindow, myDirectorWindow) == NULL) {
		PrintErrorMessage("ProcReparent: Unable to reparent window\n", NULL, pObj);
	}

	return err;
}

/*
 * Terminate an Application "Cleanly" in Win32
 * http://support.microsoft.com:80/support/kb/articles/Q178/8/93.ASP&NoWebContent=1
 */
static BOOL CALLBACK Enum_TerminateApp(HWND hwnd, LPARAM lParam) 
{
	DWORD dwID;
	GetWindowThreadProcessId(hwnd, &dwID);

	if (dwID == (DWORD) lParam) {
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	}

	return TRUE;
}

static bool TerminateProcess(DWORD dwPID, DWORD dwTimeout)
{
	bool result = false;

	// If we can't open the process with PROCESS_TERMINATE rights, then we give up immediately.
	HANDLE hProc = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE, dwPID);
	if (hProc == NULL)
		return result;

	// Enum_TerminateApp() posts WM_CLOSE to all windows whose PID matches your process's.
	EnumWindows((WNDENUMPROC) Enum_TerminateApp, (LPARAM) dwPID);

	// Wait on the handle. If it signals, great. If it times out, then you kill it.
	if (WaitForSingleObject(hProc, dwTimeout) != WAIT_OBJECT_0)
		result = (::TerminateProcess(hProc,0) == 0);
	else
		result = true;

	CloseHandle(hProc);

	return result;
}

MoaError CProcScript_IMoaMmXScript::terminate (PMoaDrCallInfo callPtr)
{
	MoaError err = kMoaErr_NoErr;
	MoaMmValue	moaValue;

	// process ID
	long myPID;
	pciGetArgByIndex (callPtr, 1, &moaValue);
	pObj->pMmValue->ValueToInteger(&moaValue, &myPID);

	// timeout
	long myTimeout;
	pciGetArgByIndex (callPtr, 2, &moaValue);
	pObj->pMmValue->ValueToInteger(&moaValue, &myTimeout);
	if (myTimeout == long(-1)) {
		myTimeout = INFINITE;
	}

	// return result (0=OK, 1=FAIL)
	long myResult = (long)(TerminateProcess(myPID, myTimeout) == true ? 0 : 1);
	pObj->pMmValue->IntegerToValue(myResult, &callPtr->resultValue);

	return err;
}

MoaError CProcScript_IMoaMmXScript::wait (PMoaDrCallInfo callPtr)
{
	MoaError err = kMoaErr_NoErr;
	MoaMmValue moaValue;

	// process ID
	long myPID;
	pciGetArgByIndex (callPtr, 1, &moaValue);
	pObj->pMmValue->ValueToInteger(&moaValue, &myPID);

	// timeout
	long myTimeout;
	pciGetArgByIndex (callPtr, 2, &moaValue);
	pObj->pMmValue->ValueToInteger(&moaValue, &myTimeout);
	if (myTimeout == long(-1)) {
		myTimeout = INFINITE;
	}

	// wait for process
	DWORD myResult = WAIT_FAILED;
	HANDLE hProc = OpenProcess(SYNCHRONIZE, FALSE, myPID);
	if (hProc) {
		myResult = WaitForSingleObject(hProc, myTimeout);
		CloseHandle(hProc);
	}
	if (myResult == WAIT_FAILED) {
		PrintErrorMessage("ProcWait:\n", NULL, pObj);
		err = kMoaErr_BadParam;
	}

	// return (0=OK, 1=FAIL)
	long myResultValue = (long)(myResult == WAIT_OBJECT_0 ? 0 : 1);
	pObj->pMmValue->IntegerToValue(myResultValue, &callPtr->resultValue);

	return err;
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
BEGIN_DEFINE_CLASS_INTERFACE(CProcScript, IMoaMmXScript)
END_DEFINE_CLASS_INTERFACE

#ifdef USING_INIT_FROM_DICT
BEGIN_DEFINE_CLASS_INTERFACE(CProcScript, IMoaInitFromDict)
END_DEFINE_CLASS_INTERFACE
#endif

#ifdef USING_NOTIFICATION_CLIENT
BEGIN_DEFINE_CLASS_INTERFACE(CProcScript, IMoaNotificationClient)
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

/* ------------------------------------------------------ MoaCreate_CProcScript */
STDMETHODIMP MoaCreate_CProcScript(CProcScript FAR * This)
{
	// UNUSED(This);

	/* variable declarations */
	MoaError err = kMoaErr_NoErr;

	// X_ENTER

	/* variable declarations */
	err = kMoaErr_NoErr;

	This->pCallback->QueryInterface(&IID_IMoaMmValue, (PPMoaVoid)&This->pMmValue);

	This->pCallback->QueryInterface(&IID_IMoaMmUtils2, (PPMoaVoid)&This->pConsole);

//    if (debugLevel >= 1)
		This->pConsole->PrintMessage("ProcXtra initialized.\n");
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

/* ----------------------------------------------------- MoaDestroy_CProcScript */
STDMETHODIMP_(void) MoaDestroy_CProcScript(CProcScript FAR * This)
{
	UNUSED(This);

	X_ENTER

	if (This->pMmValue) This->pMmValue->Release();

	X_RETURN_VOID;
	X_EXIT

	return;
}

/* ----------------------------------- CProcScript_IMoaMmXScript Create/Destroy */
CProcScript_IMoaMmXScript::CProcScript_IMoaMmXScript(MoaError FAR * pErr)
	{ *pErr = (kMoaErr_NoErr); }
CProcScript_IMoaMmXScript::~CProcScript_IMoaMmXScript() {}

#ifdef USING_INIT_FROM_DICT
/* -------------------------------- CProcScript_IMoaInitFromDict Create/Destroy */
CProcScript_IMoaInitFromDict::CProcScript_IMoaInitFromDict(MoaError FAR * pErr)
	{ *pErr = (kMoaErr_NoErr); }
CProcScript_IMoaInitFromDict::~CProcScript_IMoaInitFromDict() {}
#endif

#ifdef USING_NOTIFICATION_CLIENT
/* -------------------------- CProcScript_IMoaNotificationClient Create/Destroy */
CProcScript_IMoaNotificationClient::CProcScript_IMoaNotificationClient(MoaError FAR * pErr)
	{ *pErr = (kMoaErr_NoErr); }
CProcScript_IMoaNotificationClient::~CProcScript_IMoaNotificationClient() {}
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

/* -------------------------------------------- CProcScript_IMoaMmXScript::Call */
STDMETHODIMP CProcScript_IMoaMmXScript::Call(PMoaMmCallInfo callPtr)
{
	/* variable declarations */
	MoaError err = kMoaErr_NoErr;

	switch	( callPtr->methodSelector )
	{
		case m_ProcStart:
			err = start(callPtr);
			break;
		case m_ProcReparent:
			err = reparent(callPtr);
			break;
		case m_ProcTerminate:
			err = terminate(callPtr);
			break;
		case m_ProcWait:
			err = wait(callPtr);
			break;
		default:
			MessageBox (0, "Call: Unknown method", "ProcXtra", MB_ICONSTOP);
			break;
	}

	return err;
}

#ifdef USING_INIT_FROM_DICT
/* --------------------------------- CProcScript_IMoaInitFromDict::InitFromDict */
STDMETHODIMP CProcScript_IMoaInitFromDict::InitFromDict(PIMoaRegistryEntryDict pRegistryDict)
{
	UNUSED(pRegistryDict);

	/* variable declarations */
	MoaError err = kMoaErr_NoErr;

	/*
	*  --> insert additional code -->
	*/

	return err;
}
#endif

#ifdef USING_NOTIFICATION_CLIENT
/* --------------------------------- CProcScript_IMoaNotificationClient::Notify */
STDMETHODIMP CProcScript_IMoaNotificationClient::Notify(ConstPMoaNotifyID nid, PMoaVoid pNData, PMoaVoid pRefCon)
{
	UNUSED(nid);
	UNUSED(pNData);
	UNUSED(pRefCon);

	/* variable declarations */
	MoaError err = kMoaErr_NoErr;

	/*
	*  --> insert additional code -->
	*/

	return err;
}
#endif