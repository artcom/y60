//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: DSoundMessages.h,v $
//
//     $Author: valentin $
//
//   $Revision: 1.2 $
//
//
// Description: Windows DirectSound Message Texts
//
//
//=============================================================================
#ifndef INCL_DSMESSAGES
#define INCL_DSMESSAGES

#include "AudioException.h"
#include <windows.h>
#include <dsound.h>

#include <string>

namespace asl {

    class DSoundMessages {
    public:

        static const std::string WindowsError(DWORD lastError) {
            LPVOID lpMsgBuf;
            if (!FormatMessage( 
                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM | 
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPTSTR) &lpMsgBuf,
                0,
                NULL ))
            {
                // Handle the error.
                return std::string("Unknown");
            }
            std::string result = std::string((LPCTSTR)lpMsgBuf);
            // Free the buffer.
            LocalFree( lpMsgBuf );
            return result;
        }
        
        static const std::string WindowsError() {
            return DSoundMessages::WindowsError(GetLastError());
        }

        static const std::string ErrorMessage(HRESULT hr) 
        {
            const char* msg;
            // From DirectX SDK
            // http://msdn.microsoft.com/archive/default.asp?url=/archive/en-us/directx9_c/directx/htm/3dsound.asp
            switch (hr) {
            case DS_OK:
                msg = "The method succeeded.";
                break;
            case DS_NO_VIRTUALIZATION:
                msg = "The buffer was created, but another 3-D algorithm was substituted.";
                break;
#ifdef DS_INCOMPLETE
            case DS_INCOMPLETE:
                msg = "The method succeeded, but not all the optional effects were obtained.";
                break;
#endif
            case DSERR_ACCESSDENIED:
                msg = "The request failed because access was denied.";
                break;
            case DSERR_ALLOCATED:
                msg = "The request failed because resources, such as a priority level, were already in use by another caller. ";
                break;
            case DSERR_ALREADYINITIALIZED:
                msg = "The object is already initialized. ";
                break;
            case DSERR_BADFORMAT:
                msg = "The specified wave format is not supported. ";
                break;
            case DSERR_BADSENDBUFFERGUID:
                msg = "The GUID specified in an audiopath file does not match a valid mix-in buffer. ";
                break;
            case DSERR_BUFFERLOST:
                msg = "The buffer memory has been lost and must be restored. ";
                break;
            case DSERR_BUFFERTOOSMALL:
                msg = "The buffer size is not great enough to enable effects processing.";
                break;
            case DSERR_CONTROLUNAVAIL:
                msg = "The buffer control (volume, pan, and so on) requested by the caller is not available. Controls must be specified when the buffer is created, using the dwFlags member of DSBUFFERDESC.";
                break;
            case DSERR_DS8_REQUIRED:
                msg = "A DirectSound object of class CLSID_DirectSound8 or later is required for the requested functionality. For more information, see IDirectSound8 Interface.";
                break;
            case DSERR_FXUNAVAILABLE:
                msg = "The effects requested could not be found on the system, or they are in the wrong order or in the wrong location; for example, an effect expected in hardware was found in software.";
                break;
            case DSERR_GENERIC:
                msg = "An undetermined error occurred inside the DirectSound subsystem. ";
                break;
            case DSERR_INVALIDCALL:
                msg = "This function is not valid for the current state of this object. ";
                break;
            case DSERR_INVALIDPARAM:
                msg = "An invalid parameter was passed to the returning function. ";
                break;
            case DSERR_NOAGGREGATION:
                msg = "The object does not support aggregation. ";
                break;
            case DSERR_NODRIVER:
                msg = "No sound driver is available for use, or the given GUID is not a valid DirectSound device ID.";
                break;
            case DSERR_NOINTERFACE:
                msg = "The requested COM interface is not available. ";
                break;
            case DSERR_OBJECTNOTFOUND:
                msg = "The requested object was not found. ";
                break;
            case DSERR_OTHERAPPHASPRIO:
                msg = "Another application has a higher priority level, preventing this call from succeeding. ";
                break;
            case DSERR_OUTOFMEMORY:
                msg = "The DirectSound subsystem could not allocate sufficient memory to complete the caller's request. ";
                break;
            case DSERR_PRIOLEVELNEEDED:
                msg = "A cooperative level of DSSCL_PRIORITY or higher is required. ";
                break;
            case DSERR_SENDLOOP:
                msg = "A circular loop of send effects was detected.";
                break;
            case DSERR_UNINITIALIZED:
                msg = "The IDirectSound8::Initialize method has not been called or has not been called successfully before other methods were called. ";
                break;
            case DSERR_UNSUPPORTED:
                msg = "The function called is not supported at this time.";
                break;
            default:
                msg = "#ERROR'";
            }
            return std::string(msg);
        }
    };

    class DSException : public AudioException {
    public:
        DSException(const std::string & what, const std::string & where) 
		    : AudioException(what, where, "DSException") {}
        DSException(HRESULT error, const std::string & where)
		    : AudioException(DSoundMessages::ErrorMessage(error), where, "DSException") {}
    protected:
        DSException(const std::string & what, const std::string & where, const char* name) 
			: AudioException(what, where, name) {}
    };

	inline void
	checkDSRetVal (HRESULT theRetVal, const std::string& theWhere) {
		if (FAILED(theRetVal)) {
			throw DSException(theRetVal, theWhere);
		}
	}


}













#endif