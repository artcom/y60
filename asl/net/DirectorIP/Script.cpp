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
 *     CIPClientScript      <name of the class you defined>
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
#include <asl/net/TCPClientSocket.h>
#include <asl/net/UDPConnection.h>
#include <asl/net/UDPSocket.h>

#include "Script.h"

#include <asl/net/address.h>
#include <asl/net/net.h>
#include <asl/base/os_functions.h>

#ifdef MACINTOSH
	#include <windows.h>
	#include <string.h>
#endif
#include <mmiutil.h>

#include <crtdbg.h>

using namespace inet;
using namespace std;

/*****************************************************************************
 *  Private Methods
 ****************************************************************************/

inet::Socket * CIPClientScript::_mySocket = 0;
std::string CIPClientScript::_myReceiveBuffer;
int CIPClientScript::_myReceiveRC;
bool CIPClientScript::_myIsTCP;
inet::UDPSocket * CIPClientScript::_myUDPSocket = 0;
u_long CIPClientScript::_myLastUDPRemoteHost = 0;

static int initModule = 0;	// module already initialized?

const unsigned CHUNK_SIZE = 10240;

void CIPClientScript_IMoaMmXScript::bork (const SocketException& ex,
                                               PMoaDrCallInfo callPtr)
{
    pObj->_myReceiveRC = ex.getErrorCode();
    pObj->pMmValue->StringToValue("", &callPtr->resultValue);
    pObj->pConsole->PrintMessage((ex.what()+"\n").c_str());
}


MoaError CIPClientScript_IMoaMmXScript::tcpconnect (PMoaDrCallInfo callPtr)
{
	MoaError err = kMoaErr_NoErr;

	MoaMmValue	moaValue;

	// access serverName
    char myServerName[256];
	pciGetArgByIndex (callPtr, 1, &moaValue);
	pObj->pMmValue->ValueToString(&moaValue, myServerName, 255);

	// access serverPort
	long myServerPort;
	pciGetArgByIndex(callPtr, 2, &moaValue);
	pObj->pMmValue->ValueToInteger(&moaValue, &myServerPort);

    try {
        TCPClientSocket * myTCPSocket;
        if (!pObj->_mySocket) {
            myTCPSocket = new TCPClientSocket;
            pObj->_myIsTCP = true;
            pObj->_mySocket = myTCPSocket;
        } else {
            myTCPSocket = dynamic_cast<TCPClientSocket*>(pObj->_mySocket);
        }
        pObj->_mySocket->setRemoteAddr(getHostAddress(myServerName), (u_short)myServerPort);
        myTCPSocket->connect();
        pObj->_mySocket->setBlockingMode(false);
	    pObj->pConsole->PrintMessage ("Connect successful.\n");
        pObj->pMmValue->IntegerToValue(0, &callPtr->resultValue);
    } catch (SocketException& ex) {
        delete pObj->_mySocket;
        pObj->_mySocket = 0;
        bork (ex, callPtr);
    }

	// force new initialization
	initModule = 0;
    pObj->_myReceiveRC = 0;

	return (err);
}

MoaError CIPClientScript_IMoaMmXScript::udpconnect (PMoaDrCallInfo callPtr)
{
	MoaError err = kMoaErr_NoErr;

	MoaMmValue	moaValue;

	// access serverName
    char myRemoteName[256];
	pciGetArgByIndex (callPtr, 1, &moaValue);
	pObj->pMmValue->ValueToString(&moaValue, myRemoteName, 255);

	// access serverPort
	long myRemotePort;
	pciGetArgByIndex(callPtr, 2, &moaValue);
	pObj->pMmValue->ValueToInteger(&moaValue, &myRemotePort);

    char myLocalName[256];
	pciGetArgByIndex (callPtr, 3, &moaValue);
	pObj->pMmValue->ValueToString(&moaValue, myLocalName, 255);

	long myLocalPort;
	pciGetArgByIndex(callPtr, 4, &moaValue);
	pObj->pMmValue->ValueToInteger(&moaValue, &myLocalPort);

    try {
        UDPConnection * pUDPConnection;
        if (!pObj->_mySocket) {
            unsigned long myLocalHostAddress = asl::hostaddress(myLocalName);
            pObj->_myIsTCP = false;
            pUDPConnection = new inet::UDPConnection(myLocalHostAddress, (u_short)myLocalPort);
            pObj->_mySocket = pUDPConnection;
        } else {
            pUDPConnection = dynamic_cast<UDPConnection*>(pObj->_mySocket);
        }
        pUDPConnection->connect(getHostAddress(myRemoteName), (u_short)myRemotePort);
        pObj->_mySocket->setRemoteAddr(getHostAddress(myRemoteName), (u_short)myRemotePort);
        pObj->_mySocket->setBlockingMode(false);
//	    pObj->pConsole->PrintMessage ("Connect successful.\n");
        pObj->pMmValue->IntegerToValue(0, &callPtr->resultValue);
    } catch (SocketException& ex) {
        delete pObj->_mySocket;
        pObj->_mySocket = 0;
        bork (ex, callPtr);
    }

	// force new initialization
	initModule = 0;
    pObj->_myReceiveRC = 0;

	return (err);
}

MoaError CIPClientScript_IMoaMmXScript::receiveMsg (PMoaDrCallInfo callPtr) {
    if (!pObj->_mySocket) {
        pObj->pConsole->PrintMessage("receiveMsg called without connection.\n");
        pObj->_myReceiveRC = -1;
        return 0;
    }
    // Get data from the network.
    unsigned myCharsReceived = 1;
    string & myBuf = pObj->_myReceiveBuffer;
    try {
        char myChunk[CHUNK_SIZE];
        if (pObj->_myIsTCP) {
            do {
                myCharsReceived = pObj->_mySocket->receive(myChunk, CHUNK_SIZE-1);
                if (myCharsReceived) {
                    myChunk[myCharsReceived] = 0;
                    myBuf += myChunk;
                    pObj->pConsole->PrintMessage((string(myChunk)+"\n").c_str());
                }
            } while (myCharsReceived == CHUNK_SIZE);

            // See if there's a complete message in the buffer and return it if so.
            string::size_type myEOMsg = myBuf.find('\n');
            if (myEOMsg != myBuf.npos) {
                string myMsg = myBuf.substr(0, myEOMsg);
                myBuf = myBuf.substr(myEOMsg+1);
    //    	    pObj->pConsole->PrintMessage((string("Received: ")+myMsg+"\n").c_str());
    	        pObj->pMmValue->StringToValue(myMsg.c_str(), &callPtr->resultValue);
            } else {
                pObj->pMmValue->StringToValue("", &callPtr->resultValue);
            }
        } else {
            myCharsReceived = pObj->_mySocket->receive(myChunk, CHUNK_SIZE-1);
            if (myCharsReceived) {
                myChunk[myCharsReceived] = 0;
            }
            if (myCharsReceived > 0) {
    	        pObj->pMmValue->StringToValue(myChunk, &callPtr->resultValue);
    	    } else {
                pObj->pMmValue->StringToValue("", &callPtr->resultValue);
    	    }
        }
        pObj->_myReceiveRC = 0;
    } catch (SocketException& ex) {
        pObj->pConsole->PrintMessage((ex.what()+"\n").c_str());
        pObj->_myReceiveRC = ex.getErrorCode();
        pObj->pMmValue->StringToValue("", &callPtr->resultValue);
    }

    return kMoaErr_NoErr;
}

MoaError CIPClientScript_IMoaMmXScript::getStatus (PMoaDrCallInfo callPtr) {
    pObj->pMmValue->IntegerToValue(pObj->_myReceiveRC, &callPtr->resultValue);
    pObj->_myReceiveRC = 0;
    return kMoaErr_NoErr;
}

MoaError CIPClientScript_IMoaMmXScript::sendMsg (PMoaDrCallInfo callPtr) {
    if (!pObj->_mySocket) {
        pObj->pConsole->PrintMessage("sendMsg called without connection.\n");
        pObj->_myReceiveRC = -1;
        return 0;
    }    
	MoaMmValue	moaValue;

	// access Message.
    char myMsg[16384];
	pciGetArgByIndex (callPtr, 1, &moaValue);
	pObj->pMmValue->ValueToString(&moaValue, myMsg, 16382);
	strcat(myMsg, "\n");

    try {
        pObj->_mySocket->send(myMsg, strlen(myMsg));
        pObj->pMmValue->IntegerToValue(0, &callPtr->resultValue);
    } catch (SocketException& ex) {
        bork(ex, callPtr);
    }

    return kMoaErr_NoErr;
}

MoaError CIPClientScript_IMoaMmXScript::disconnect(PMoaDrCallInfo callPtr) {
    if (pObj->_mySocket) {
        pObj->_mySocket->close();
        delete pObj->_mySocket;
    }
    pObj->_mySocket = 0;
    pObj->pMmValue->IntegerToValue(0, &callPtr->resultValue);

    return kMoaErr_NoErr;
}

MoaError CIPClientScript_IMoaMmXScript::getErrString (PMoaDrCallInfo callPtr) {
	MoaMmValue	moaValue;
	// access error code
	long myErrCode;
	pciGetArgByIndex(callPtr, 1, &moaValue);
	pObj->pMmValue->ValueToInteger(&moaValue, &myErrCode);

    pObj->pMmValue->StringToValue(SocketException::getErrorMessage(myErrCode).c_str(), &callPtr->resultValue);
    return kMoaErr_NoErr;
}

MoaError CIPClientScript_IMoaMmXScript::udpOpen (PMoaDrCallInfo callPtr) {
    try {
	    MoaMmValue moaValue;

	    char myLocalName[256];
	    pciGetArgByIndex (callPtr, 1, &moaValue);
	    pObj->pMmValue->ValueToString(&moaValue, myLocalName, 255);

	    // access serverPort
	    long myLocalPort;
	    pciGetArgByIndex(callPtr, 2, &moaValue);
	    pObj->pMmValue->ValueToInteger(&moaValue, &myLocalPort);
    	
	    if (pObj->_myUDPSocket) {
	        delete pObj->_myUDPSocket;
	        pObj->_myUDPSocket = 0;
	    }
    	
    	unsigned long myLocalHostAddress;
    	if (!strcmp(myLocalName, "INADDR_ANY")) {
    	    myLocalHostAddress = INADDR_ANY;
    	} else {
            myLocalHostAddress = asl::hostaddress(myLocalName);
        }
	    pObj->_myUDPSocket = new UDPSocket(myLocalHostAddress, (short)myLocalPort);
	    pObj->_myUDPSocket->close();
	    pObj->_myUDPSocket->open();
        pObj->_myUDPSocket->setBlockingMode(false);
    } catch (SocketException& ex) {
        delete pObj->_myUDPSocket;
        pObj->_myUDPSocket = 0;
        
        bork(ex, callPtr);
    }

    return kMoaErr_NoErr;
}


MoaError CIPClientScript_IMoaMmXScript::receiveUDPMsg (PMoaDrCallInfo callPtr) {
    try {
        if (!pObj->_myUDPSocket) {
            pObj->pConsole->PrintMessage("receiveUDPMsg called without udpOpen.\n");
            pObj->_myReceiveRC = -1;
            return 0;
        }
        char myChunk[CHUNK_SIZE];
        u_short remotePort;
        int myCharsReceived = pObj->_myUDPSocket->receiveFrom(&pObj->_myLastUDPRemoteHost, 
                &remotePort, myChunk, CHUNK_SIZE-1);
        if (myCharsReceived) {
            myChunk[myCharsReceived] = 0;
        }
        if (myCharsReceived > 0) {
    	    pObj->pMmValue->StringToValue(myChunk, &callPtr->resultValue);
    	} else {
            pObj->pMmValue->StringToValue("", &callPtr->resultValue);
    	}        
    } catch (SocketException& ex) {
        pObj->pMmValue->StringToValue("", &callPtr->resultValue);
        bork(ex, callPtr);
    }

    return kMoaErr_NoErr;        
}

MoaError CIPClientScript_IMoaMmXScript::getUDPMsgOrigin (PMoaDrCallInfo callPtr) {

    try {
        if (!pObj->_myUDPSocket) {
            pObj->pConsole->PrintMessage("getUDPMsgOrigin called without udpOpen.\n");
            pObj->_myReceiveRC = -1;
            return 0;
        }
        string s = asl::hostname(pObj->_myLastUDPRemoteHost);
        pObj->pMmValue->StringToValue(s.c_str(), &callPtr->resultValue);
    } catch (SocketException& ex) {
        bork(ex, callPtr);
    }

    return kMoaErr_NoErr;        
}

MoaError CIPClientScript_IMoaMmXScript::sendUDPMsg (PMoaDrCallInfo callPtr) {
    try {
        if (!pObj->_myUDPSocket) {
            pObj->pConsole->PrintMessage("getUDPMsgOrigin called without udpOpen.\n");
            pObj->_myReceiveRC = -1;
            return 0;
        }
	    MoaMmValue moaValue;

	    char myRemoteName[256];
	    pciGetArgByIndex (callPtr, 1, &moaValue);
	    pObj->pMmValue->ValueToString(&moaValue, myRemoteName, 255);

	    long myRemotePort;
	    pciGetArgByIndex(callPtr, 2, &moaValue);
	    pObj->pMmValue->ValueToInteger(&moaValue, &myRemotePort);
    	
	    char myMessage[10240];
	    pciGetArgByIndex (callPtr, 3, &moaValue);
	    pObj->pMmValue->ValueToString(&moaValue, myMessage, 10239);
	    
	    unsigned long myRemoteHostAddress = asl::hostaddress(myRemoteName);
	    pObj->_myUDPSocket->sendTo(myRemoteHostAddress, myRemotePort, myMessage, strlen(myMessage));
    } catch (SocketException& ex) {
        bork(ex, callPtr);
    }
    return kMoaErr_NoErr;        
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
BEGIN_DEFINE_CLASS_INTERFACE(CIPClientScript, IMoaMmXScript)
END_DEFINE_CLASS_INTERFACE

#ifdef USING_INIT_FROM_DICT
BEGIN_DEFINE_CLASS_INTERFACE(CIPClientScript, IMoaInitFromDict)
END_DEFINE_CLASS_INTERFACE
#endif

#ifdef USING_NOTIFICATION_CLIENT
BEGIN_DEFINE_CLASS_INTERFACE(CIPClientScript, IMoaNotificationClient)
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

/* ------------------------------------------------------ MoaCreate_CIPClientScript */
STDMETHODIMP MoaCreate_CIPClientScript(CIPClientScript FAR * This)
{
	// UNUSED(This);

	/* variable declarations */
	MoaError err = kMoaErr_NoErr;

	// X_ENTER
    initSockets();
	/* variable declarations */
	err = kMoaErr_NoErr;

	This->pCallback->QueryInterface(&IID_IMoaMmValue, (PPMoaVoid)&This->pMmValue);

	This->pCallback->QueryInterface(&IID_IMoaMmUtils2, (PPMoaVoid)&This->pConsole);

//    if (debugLevel >= 1)
		This->pConsole->PrintMessage("DirectorIP initialized.\n");
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

/* ----------------------------------------------------- MoaDestroy_CIPClientScript */
STDMETHODIMP_(void) MoaDestroy_CIPClientScript(CIPClientScript FAR * This)
{
	UNUSED(This);

	X_ENTER

	if (This->pMmValue) This->pMmValue->Release();

    terminateSockets();

	X_RETURN_VOID;
	X_EXIT

	return;
}

/* ----------------------------------- CIPClientScript_IMoaMmXScript Create/Destroy */
CIPClientScript_IMoaMmXScript::CIPClientScript_IMoaMmXScript(MoaError FAR * pErr)
	{ *pErr = (kMoaErr_NoErr); }
CIPClientScript_IMoaMmXScript::~CIPClientScript_IMoaMmXScript() {}

#ifdef USING_INIT_FROM_DICT
/* -------------------------------- CIPClientScript_IMoaInitFromDict Create/Destroy */
CIPClientScript_IMoaInitFromDict::CIPClientScript_IMoaInitFromDict(MoaError FAR * pErr)
	{ *pErr = (kMoaErr_NoErr); }
CIPClientScript_IMoaInitFromDict::~CIPClientScript_IMoaInitFromDict() {}
#endif

#ifdef USING_NOTIFICATION_CLIENT
/* -------------------------- CIPClientScript_IMoaNotificationClient Create/Destroy */
CIPClientScript_IMoaNotificationClient::CIPClientScript_IMoaNotificationClient(MoaError FAR * pErr)
	{ *pErr = (kMoaErr_NoErr); }
CIPClientScript_IMoaNotificationClient::~CIPClientScript_IMoaNotificationClient() {}
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

/* -------------------------------------------- CIPClientScript_IMoaMmXScript::Call */
STDMETHODIMP CIPClientScript_IMoaMmXScript::Call(PMoaMmCallInfo callPtr)
{

	/* variable declarations */
	MoaError err = kMoaErr_NoErr;

	switch	( callPtr->methodSelector )
	{
        case m_IPClientTCPConnect:
            err = tcpconnect(callPtr);
            break;
        case m_IPClientUDPConnect:
            err = udpconnect(callPtr);
            break;
        case m_IPClientReceiveMsg:
            err = receiveMsg(callPtr);
            break;
        case m_IPClientGetStatus:
            err = getStatus(callPtr);
            break;
        case m_IPClientSendMsg:
            err = sendMsg(callPtr);
            break;
        case m_IPClientDisconnect:
            err = disconnect(callPtr);
            break;
        case m_IPClientGetErrString:
            err = getErrString(callPtr);
            break;
        case m_IPClientUDPOpen:
            err = udpOpen(callPtr);
            break;
        case m_IPClientReceiveUDPMsg:
            err = receiveUDPMsg(callPtr);
            break;
        case m_IPClientGetUDPMsgOrigin:
            err = getUDPMsgOrigin(callPtr);
            break;
        case m_IPClientSendUDPMsg:
            err = sendUDPMsg(callPtr);
            break;
        default:
            MessageBox (0, "Call: Unknown method", "DirectorIP", MB_ICONSTOP);
            break;
	}

	return(err);
}

#ifdef USING_INIT_FROM_DICT
/* --------------------------------- CIPClientScript_IMoaInitFromDict::InitFromDict */
STDMETHODIMP CIPClientScript_IMoaInitFromDict::InitFromDict(PIMoaRegistryEntryDict pRegistryDict)
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
/* --------------------------------- CIPClientScript_IMoaNotificationClient::Notify */
STDMETHODIMP CIPClientScript_IMoaNotificationClient::Notify(ConstPMoaNotifyID nid, PMoaVoid pNData, PMoaVoid pRefCon)
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

