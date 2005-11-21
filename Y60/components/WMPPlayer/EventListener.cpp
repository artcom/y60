#include "EventListener.h"

#include <asl/Logger.h>

HRESULT EventListener::Invoke(
                            DISPID  dispIdMember,      
                            REFIID  riid,              
                            LCID  lcid,                
                            WORD  wFlags,              
                            DISPPARAMS FAR*  pDispParams,  
                            VARIANT FAR*  pVarResult,  
                            EXCEPINFO FAR*  pExcepInfo,  
                            unsigned int FAR*  puArgErr )
{
    if (!pDispParams)
        return E_POINTER;

    if (pDispParams->cNamedArgs != 0)
        return DISP_E_NONAMEDARGS;

    HRESULT hr = DISP_E_MEMBERNOTFOUND;

    switch (dispIdMember)
    {
        case DISPID_WMPCOREEVENT_ENDOFSTREAM:
            EndOfStream(pDispParams->rgvarg[0].lVal /* Result */ );
            break;
        case DISPID_WMPCOREEVENT_CURRENTMEDIAITEMAVAILABLE:
            CurrentMediaItemAvailable(pDispParams->rgvarg[0].bstrVal /* bstrItemName */);
            break;
        case DISPID_WMPOCXEVENT_CLICK:
            Click(pDispParams->rgvarg[3].iVal /* nButton */, pDispParams->rgvarg[2].iVal /* nShiftState */,  pDispParams->rgvarg[1].lVal /* fX */,  pDispParams->rgvarg[0].lVal /* fY */);
            break;
    }

    return( hr );
}

void 
EventListener::init() {
    _myLoadedFlag = false;
    _myErrorFlag = false;
}
bool 
EventListener::isLoaded() {
    return _myLoadedFlag;
}	   
bool 
EventListener::gotError() {
    return _myErrorFlag;
}	   

// Sent when the control changes OpenState
void EventListener::OpenStateChange(long NewState)
{
    return;
}

// Sent when the control changes PlayState
void EventListener::PlayStateChange(long NewState)
{
    return;
}

// Sent when the audio language changes
void EventListener::AudioLanguageChange(long LangID)
{
    return;
}

// Sent when the status string changes
void EventListener::StatusChange()
{
    return;
}

// Sent when a synchronized command or URL is received
void EventListener::ScriptCommand(BSTR scType,BSTR Param)
{
    return;
}

// Sent when a new stream is encountered (obsolete)
void EventListener::NewStream()
{
    return;
}

// Sent when the control is disconnected from the server (obsolete)
void EventListener:: Disconnect(long Result )
{
    return;
}

// Sent when the control begins or ends buffering
void EventListener:: Buffering(VARIANT_BOOL Start)
{
    return;
}

// Sent when the control has an error condition
void EventListener::Error()
{
    _myErrorFlag = true;
    return;
}

// Sent when the control has an warning condition (obsolete)
void EventListener::Warning(long WarningType, long Param, BSTR Description)
{
    return;
}

// Sent when the media has reached end of stream
void EventListener::EndOfStream(long Result)
{
    return;
}

// Indicates that the current position of the movie has changed
void EventListener::PositionChange(double oldPosition,double newPosition)
{
    return;
}

// Sent when a marker is reached
void EventListener::MarkerHit(long MarkerNum )
{
    return;
}

// Indicates that the unit used to express duration and position has changed
void EventListener::DurationUnitChange(long NewDurationUnit)
{
    return;
}

// Indicates that the CD ROM media has changed
void EventListener::CdromMediaChange(long CdromNum)
{
    return;
}

// Sent when a playlist changes
void EventListener::PlaylistChange(IDispatch * Playlist,WMPPlaylistChangeEventType change)
{
    return;
}

// Sent when the current playlist changes
void EventListener::CurrentPlaylistChange(WMPPlaylistChangeEventType change )
{
    return;
}

// Sent when a current playlist item becomes available
void EventListener::CurrentPlaylistItemAvailable(BSTR bstrItemName)
{
    return;
}

// Sent when a media object changes
void EventListener::MediaChange(IDispatch * Item)
{
    _myLoadedFlag = true;
    return;
}

// Sent when a current media item becomes available
void EventListener::CurrentMediaItemAvailable(BSTR bstrItemName)
{
    return;
}

// Sent when the item selection on the current playlist changes
void EventListener::CurrentItemChange(IDispatch *pdispMedia)
{
    return;
}

// Sent when the media collection needs to be requeried
void EventListener::MediaCollectionChange()
{
    return;
}

// Sent when an attribute string is added in the media collection
void EventListener::MediaCollectionAttributeStringAdded(BSTR bstrAttribName, BSTR bstrAttribVal)
{
    return;
}

// Sent when an attribute string is removed from the media collection
void EventListener::MediaCollectionAttributeStringRemoved(BSTR bstrAttribName, BSTR bstrAttribVal)
{
    return;
}

// Sent when an attribute string is changed in the media collection
void EventListener::MediaCollectionAttributeStringChanged(BSTR bstrAttribName, BSTR bstrOldAttribVal, BSTR bstrNewAttribVal)
{
    return;
}

// Sent when playlist collection needs to be requeried
void EventListener::PlaylistCollectionChange()
{
    return;
}

// Sent when a playlist is added to the playlist collection
void EventListener::PlaylistCollectionPlaylistAdded(BSTR bstrPlaylistName)
{
   return;
}

// Sent when a playlist is removed from the playlist collection
void EventListener::PlaylistCollectionPlaylistRemoved(BSTR bstrPlaylistName)
{
    return;
}

// Sent when a playlist has been set or reset as deleted
void EventListener::PlaylistCollectionPlaylistSetAsDeleted(BSTR bstrPlaylistName, VARIANT_BOOL varfIsDeleted)
{
    return;
}

// Playlist playback mode has changed
void EventListener::ModeChange(BSTR ModeName, VARIANT_BOOL NewValue)
{
    return;
}

// Sent when the media object has an error condition
void EventListener::MediaError(IDispatch * pMediaObject)
{
    _myErrorFlag = true;
    return;
}

// Current playlist switch with no open state change
void EventListener::OpenPlaylistSwitch(IDispatch *pItem)
{
    return;
}

// Sent when the current DVD domain changes
void EventListener::DomainChange(BSTR strDomain)
{
    return;
}

// Sent when display switches to player application
void EventListener::SwitchedToPlayerApplication()
{
    return;
}

// Sent when display switches to control
void EventListener::SwitchedToControl()
{
    return;
}

// Sent when the player docks or undocks
void EventListener::PlayerDockedStateChange()
{
    return;
}

// Sent when the OCX reconnects to the player
void EventListener::PlayerReconnect()
{
    return;
}

// Occurs when a user clicks the mouse
void EventListener::Click( short nButton, short nShiftState, long fX, long fY )
{
    AC_INFO << "CLICK";
    return;
}

// Occurs when a user double-clicks the mouse
void EventListener::DoubleClick( short nButton, short nShiftState, long fX, long fY )
{
    return;
}

// Occurs when a key is pressed
void EventListener::KeyDown( short nKeyCode, short nShiftState )
{
    return;
}

// Occurs when a key is pressed and released
void EventListener::KeyPress( short nKeyAscii )
{
    return;
}

// Occurs when a key is released
void EventListener::KeyUp( short nKeyCode, short nShiftState )
{
    return;
}

// Occurs when a mouse button is pressed
void EventListener::MouseDown( short nButton, short nShiftState, long fX, long fY )
{
    return;
}

// Occurs when a mouse pointer is moved
void EventListener::MouseMove( short nButton, short nShiftState, long fX, long fY )
{
    return;
}

// Occurs when a mouse button is released
void EventListener::MouseUp( short nButton, short nShiftState, long fX, long fY )
{
    return;
}
