/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Sep 08 14:05:58 2007
 */
/* Compiler settings for c:\BOB\ActiveDcam\ActiveDcam.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __ActiveCam_h__
#define __ActiveCam_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IAVTActiveCam_FWD_DEFINED__
#define __IAVTActiveCam_FWD_DEFINED__
typedef interface IAVTActiveCam IAVTActiveCam;
#endif 	/* __IAVTActiveCam_FWD_DEFINED__ */


#ifndef ___IAVTActiveCamEvents_FWD_DEFINED__
#define ___IAVTActiveCamEvents_FWD_DEFINED__
typedef interface _IAVTActiveCamEvents _IAVTActiveCamEvents;
#endif 	/* ___IAVTActiveCamEvents_FWD_DEFINED__ */


#ifndef __AVTActiveCam_FWD_DEFINED__
#define __AVTActiveCam_FWD_DEFINED__

#ifdef __cplusplus
typedef class AVTActiveCam AVTActiveCam;
#else
typedef struct AVTActiveCam AVTActiveCam;
#endif /* __cplusplus */

#endif 	/* __AVTActiveCam_FWD_DEFINED__ */


#ifndef __VideoSource_FWD_DEFINED__
#define __VideoSource_FWD_DEFINED__

#ifdef __cplusplus
typedef class VideoSource VideoSource;
#else
typedef struct VideoSource VideoSource;
#endif /* __cplusplus */

#endif 	/* __VideoSource_FWD_DEFINED__ */


#ifndef __VideoFormat_FWD_DEFINED__
#define __VideoFormat_FWD_DEFINED__

#ifdef __cplusplus
typedef class VideoFormat VideoFormat;
#else
typedef struct VideoFormat VideoFormat;
#endif /* __cplusplus */

#endif 	/* __VideoFormat_FWD_DEFINED__ */


#ifndef __VideoExposure_FWD_DEFINED__
#define __VideoExposure_FWD_DEFINED__

#ifdef __cplusplus
typedef class VideoExposure VideoExposure;
#else
typedef struct VideoExposure VideoExposure;
#endif /* __cplusplus */

#endif 	/* __VideoExposure_FWD_DEFINED__ */


#ifndef __VideoColor_FWD_DEFINED__
#define __VideoColor_FWD_DEFINED__

#ifdef __cplusplus
typedef class VideoColor VideoColor;
#else
typedef struct VideoColor VideoColor;
#endif /* __cplusplus */

#endif 	/* __VideoColor_FWD_DEFINED__ */


#ifndef __VideoDisplay_FWD_DEFINED__
#define __VideoDisplay_FWD_DEFINED__

#ifdef __cplusplus
typedef class VideoDisplay VideoDisplay;
#else
typedef struct VideoDisplay VideoDisplay;
#endif /* __cplusplus */

#endif 	/* __VideoDisplay_FWD_DEFINED__ */


#ifndef __VideoAdvanced_FWD_DEFINED__
#define __VideoAdvanced_FWD_DEFINED__

#ifdef __cplusplus
typedef class VideoAdvanced VideoAdvanced;
#else
typedef struct VideoAdvanced VideoAdvanced;
#endif /* __cplusplus */

#endif 	/* __VideoAdvanced_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
//#include "CategorizeProperties.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __AVTACTIVECAMLib_LIBRARY_DEFINED__
#define __AVTACTIVECAMLib_LIBRARY_DEFINED__

/* library AVTACTIVECAMLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_AVTACTIVECAMLib;

#ifndef __IAVTActiveCam_INTERFACE_DEFINED__
#define __IAVTActiveCam_INTERFACE_DEFINED__

/* interface IAVTActiveCam */
/* [unique][helpstring][dual][uuid][object] */ 

#define	DISPID_GRAB	( 1 )

#define	DISPID_CAMERA	( 2 )

#define	DISPID_MODE	( 3 )

#define	DISPID_RATE	( 4 )

#define	DISPID_SIZEX	( 5 )

#define	DISPID_SIZEY	( 6 )

#define	DISPID_STARTX	( 7 )

#define	DISPID_STARTY	( 8 )

#define	DISPID_TRIGGER	( 9 )

#define	DISPID_TRIGGER_MODE	( 10 )

#define	DISPID_TRIGGER_COUNTER	( 11 )

#define	DISPID_TRIGGER_POLARITY	( 12 )

#define	DISPID_TIMEOUT	( 13 )

#define	DISPID_ASYNCH	( 14 )

#define	DISPID_PACKET	( 15 )

#define	DISPID_SWAP	( 16 )

#define	DISPID_BAYER	( 17 )

#define	DISPID_BAYER_ORDER	( 18 )

#define	DISPID_DISPLAY	( 19 )

#define	DISPID_BRIGHTNESS	( 20 )

#define	DISPID_EXPOSURE	( 21 )

#define	DISPID_SHARPNESS	( 22 )

#define	DISPID_WHITEBALANCE_UB	( 23 )

#define	DISPID_WHITEBALANCE_VR	( 123 )

#define	DISPID_HUE	( 24 )

#define	DISPID_SATURATION	( 25 )

#define	DISPID_GAMMA	( 26 )

#define	DISPID_SHUTTER	( 27 )

#define	DISPID_GAIN	( 28 )

#define	DISPID_IRIS	( 29 )

#define	DISPID_FOCUS	( 30 )

#define	DISPID_FILTER	( 31 )

#define	DISPID_ZOOM	( 32 )

#define	DISPID_PAN	( 33 )

#define	DISPID_TILT	( 34 )

#define	DISPID_TEMPERATURE	( 135 )

#define	DISPID_DELAY	( 136 )

#define	DISPID_SHADING	( 137 )

#define	DISPID_TRIGGER_SOURCE	( 35 )

#define	DISPID_BITSHIFT	( 36 )

#define	DISPID_OVERLAY	( 37 )

#define	DISPID_OVERLAY_COLOR	( 38 )

#define	DISPID_OVERLAY_FONT	( 39 )

#define	DISPID_BRIGHTNESS_CONTROL	( 40 )

#define	DISPID_EXPOSURE_CONTROL	( 41 )

#define	DISPID_SHARPNESS_CONTROL	( 42 )

#define	DISPID_WHITEBALANCE_CONTROL	( 43 )

#define	DISPID_HUE_CONTROL	( 44 )

#define	DISPID_SATURATION_CONTROL	( 45 )

#define	DISPID_GAMMA_CONTROL	( 46 )

#define	DISPID_SHUTTER_CONTROL	( 47 )

#define	DISPID_GAIN_CONTROL	( 48 )

#define	DISPID_IRIS_CONTROL	( 49 )

#define	DISPID_FOCUS_CONTROL	( 50 )

#define	DISPID_FILTER_CONTROL	( 51 )

#define	DISPID_ZOOM_CONTROL	( 52 )

#define	DISPID_PAN_CONTROL	( 53 )

#define	DISPID_TILT_CONTROL	( 54 )

#define	DISPID_TEMPERATURE_CONTROL	( 55 )

#define	DISPID_DELAY_CONTROL	( 56 )

#define	DISPID_SHADING_CONTROL	( 57 )

#define	DISPID_BRIGHTNESS_MIN	( 60 )

#define	DISPID_BRIGHTNESS_MAX	( 61 )

#define	DISPID_EXPOSURE_MIN	( 62 )

#define	DISPID_EXPOSURE_MAX	( 63 )

#define	DISPID_SHARPNESS_MIN	( 64 )

#define	DISPID_SHARPNESS_MAX	( 65 )

#define	DISPID_WHITEBALANCE_MIN	( 66 )

#define	DISPID_WHITEBALANCE_MAX	( 67 )

#define	DISPID_HUE_MIN	( 68 )

#define	DISPID_HUE_MAX	( 69 )

#define	DISPID_SATURATION_MIN	( 70 )

#define	DISPID_SATURATION_MAX	( 71 )

#define	DISPID_GAMMA_MIN	( 72 )

#define	DISPID_GAMMA_MAX	( 73 )

#define	DISPID_SHUTTER_MIN	( 74 )

#define	DISPID_SHUTTER_MAX	( 75 )

#define	DISPID_GAIN_MIN	( 76 )

#define	DISPID_GAIN_MAX	( 77 )

#define	DISPID_IRIS_MIN	( 78 )

#define	DISPID_IRIS_MAX	( 79 )

#define	DISPID_FOCUS_MIN	( 80 )

#define	DISPID_FOCUS_MAX	( 81 )

#define	DISPID_FILTER_MIN	( 82 )

#define	DISPID_FILTER_MAX	( 83 )

#define	DISPID_ZOOM_MIN	( 84 )

#define	DISPID_ZOOM_MAX	( 85 )

#define	DISPID_PAN_MIN	( 86 )

#define	DISPID_PAN_MAX	( 87 )

#define	DISPID_TILT_MIN	( 88 )

#define	DISPID_TILT_MAX	( 89 )

#define	DISPID_FLIP	( 90 )

#define	DISPID_TEMPERATURE_MIN	( 91 )

#define	DISPID_TEMPERATURE_MAX	( 92 )

#define	DISPID_DELAY_MIN	( 93 )

#define	DISPID_DELAY_MAX	( 94 )

#define	DISPID_SHADING_MIN	( 95 )

#define	DISPID_SHADING_MAX	( 96 )

#define	DISPID_ACQUIRE	( 100 )

#define	DISPID_MAG	( 101 )

#define	DISPID_SCROLLBAR	( 102 )

#define	DISPID_SCROLLX	( 103 )

#define	DISPID_SCROLLY	( 104 )

#define	DISPID_PALETTE	( 105 )

#define	DISPID_3D	( 106 )

#define	DISPID_ANTITEARING	( 107 )

#define	DISPID_ROTATE	( 108 )

#define	DISPID_BKGCORRECT	( 109 )

#define	DISPID_BKGNAME	( 110 )

#define	DISPID_AVRG	( 150 )


EXTERN_C const IID IID_IAVTActiveCam;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F762827C-CF21-4350-996F-A4BC012B8D39")
    IAVTActiveCam : public IDispatch
    {
    public:
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_BackColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_BackColor( 
            /* [retval][out] */ OLE_COLOR __RPC_FAR *pclr) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL vbool) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbool) = 0;
        
        virtual /* [id][propputref] */ HRESULT STDMETHODCALLTYPE putref_Font( 
            /* [in] */ IFontDisp __RPC_FAR *Font) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Font( 
            /* [in] */ IFontDisp __RPC_FAR *Font) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Font( 
            /* [retval][out] */ IFontDisp __RPC_FAR *__RPC_FAR *pFont) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Grab( 
            /* [retval][out] */ SHORT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_SizeX( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_SizeX( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_SizeY( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_SizeY( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_StartX( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_StartX( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_StartY( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_StartY( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Acquire( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Acquire( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Magnification( 
            /* [retval][out] */ float __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Magnification( 
            /* [in] */ float newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_ScrollBars( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_ScrollBars( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_ScrollX( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_ScrollX( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_ScrollY( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_ScrollY( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Palette( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Palette( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Edge( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Edge( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Camera( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Camera( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Mode( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Mode( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Rate( 
            /* [retval][out] */ float __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Rate( 
            /* [in] */ float newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Trigger( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Trigger( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_TriggerMode( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_TriggerMode( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_TriggerCounter( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_TriggerCounter( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_TriggerPolarity( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_TriggerPolarity( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Asynch( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Asynch( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PacketSize( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PacketSize( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_SwapBytes( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_SwapBytes( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Bayer( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Bayer( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_BayerLayout( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_BayerLayout( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Display( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Display( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Flip( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Flip( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Brightness( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Brightness( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_BrightnessControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_BrightnessControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_AutoExposureRef( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_AutoExposureRef( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_AutoExposure( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_AutoExposure( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Sharpness( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Sharpness( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_SharpnessControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_SharpnessControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Gamma( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Gamma( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_GammaControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_GammaControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Shutter( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Shutter( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShutterControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_ShutterControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Gain( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Gain( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_GainControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_GainControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Iris( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Iris( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_IrisControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_IrisControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_WhiteBalanceUB( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_WhiteBalanceUB( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_WhiteBalanceVR( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_WhiteBalanceVR( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_WhiteBalanceControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_WhiteBalanceControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Hue( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Hue( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_HueControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_HueControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Saturation( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Saturation( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_SaturationControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_SaturationControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Focus( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Focus( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_FocusControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_FocusControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Zoom( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Zoom( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_ZoomControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_ZoomControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Pan( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Pan( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_PanControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_PanControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Tilt( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Tilt( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_TiltControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_TiltControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Temperature( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Temperature( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_TemperatureControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_TemperatureControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_OpticalFilter( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_OpticalFilter( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_OpticalFilterControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_OpticalFilterControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_TriggerDelay( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_TriggerDelay( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_WhiteShading( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_WhiteShading( 
            /* [retval][out] */ OLE_COLOR __RPC_FAR *pclr) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_WhiteShadingControl( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_WhiteShadingControl( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AboutBox( void) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_OverlayColor( 
            /* [in] */ OLE_COLOR clr) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_OverlayColor( 
            /* [retval][out] */ OLE_COLOR __RPC_FAR *pclr) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Overlay( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Overlay( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [id][propputref] */ HRESULT STDMETHODCALLTYPE putref_OverlayFont( 
            /* [in] */ IFontDisp __RPC_FAR *Font) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_OverlayFont( 
            /* [in] */ IFontDisp __RPC_FAR *Font) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_OverlayFont( 
            /* [retval][out] */ IFontDisp __RPC_FAR *__RPC_FAR *pFont) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BkgCorrect( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_BkgCorrect( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BkgName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_BkgName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetBrightnessMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetBrightnessMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetExposureMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetExposureMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSharpnessMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSharpnessMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWhiteBalanceMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWhiteBalanceMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetHueMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetHueMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSaturationMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSaturationMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetGammaMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetGammaMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetShutterMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetShutterMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetGainMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetGainMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetIrisMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetIrisMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFocusMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFocusMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetZoomMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetZoomMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPanMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPanMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTiltMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTiltMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOpticalFilterMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOpticalFilterMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTriggerDelayMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTriggerDelayMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWhiteShadingMin( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWhiteShadingMax( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TriggerSource( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TriggerSource( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_BitShift( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_BitShift( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Timeout( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Timeout( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_AntiTearing( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_AntiTearing( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE get_Rotate( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE put_Rotate( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadBlock( 
            /* [in] */ long offset,
            /* [in] */ long pData,
            /* [in] */ long nBytes) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteBlock( 
            /* [in] */ long offset,
            /* [in] */ long pData,
            long nBytes) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRawData( 
            /* [defaultvalue][optional][in] */ VARIANT_BOOL isPointer,
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetImageWindow( 
            /* [in] */ SHORT x,
            /* [in] */ SHORT y,
            /* [in] */ SHORT sx,
            /* [in] */ SHORT sy,
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPixel( 
            /* [in] */ SHORT x,
            /* [in] */ SHORT y,
            /* [retval][out] */ long __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRGBPixel( 
            /* [in] */ SHORT x,
            /* [in] */ SHORT y,
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetImageLine( 
            /* [in] */ SHORT y,
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetComponentLine( 
            /* [in] */ SHORT y,
            /* [in] */ SHORT component,
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetImageData( 
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetComponentData( 
            /* [in] */ SHORT component,
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetImagePointer( 
            /* [in] */ SHORT x,
            /* [in] */ SHORT y,
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveImage( 
            /* [in] */ BSTR filepath,
            /* [defaultvalue][optional][in] */ long compression = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SoftTrigger( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWidth( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetHeight( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetBytesPerPixel( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadRegister( 
            /* [in] */ long address,
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteRegister( 
            /* [in] */ long address,
            /* [in] */ long Val) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCameraList( 
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetModeList( 
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRateList( 
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTriggerList( 
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StartCapture( 
            /* [in] */ BSTR filepath,
            /* [defaultvalue][optional][in] */ float delay = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StopCapture( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadChannel( 
            /* [in] */ SHORT channel) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveChannel( 
            /* [in] */ SHORT channel) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMaxChannel( 
            /* [retval][out] */ short __RPC_FAR *pChannel) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetIsoSpeed( 
            /* [retval][out] */ short __RPC_FAR *pSpeed) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetIsoSpeed( 
            /* [in] */ short value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowProperties( 
            /* [defaultvalue][optional][in] */ VARIANT_BOOL CameraList = TRUE) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDIB( 
            /* [retval][out] */ long __RPC_FAR *hDIB) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetImageWindow( 
            /* [in] */ SHORT x,
            /* [in] */ SHORT y,
            /* [in] */ VARIANT value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTriggerInfo( 
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetF7Info( 
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPicture( 
            /* [retval][out] */ IPictureDisp __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Draw( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OverlayRectangle( 
            /* [in] */ short StartX,
            /* [in] */ short StartY,
            /* [in] */ short EndX,
            /* [in] */ short EndY,
            /* [defaultvalue][optional][in] */ short Width = 1) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OverlayEllipse( 
            /* [in] */ short StartX,
            /* [in] */ short StartY,
            /* [in] */ short EndX,
            /* [in] */ short EndY,
            /* [defaultvalue][optional][in] */ short Width = 1) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OverlayLine( 
            /* [in] */ short StartX,
            /* [in] */ short StartY,
            /* [in] */ short EndX,
            /* [in] */ short EndY,
            /* [defaultvalue][optional][in] */ short Width = 1) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OverlayText( 
            /* [in] */ short X,
            /* [in] */ short Y,
            /* [in] */ BSTR str) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OverlayClear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OverlayPixel( 
            /* [in] */ short X,
            /* [in] */ short Y) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetItemList( 
            /* [in] */ long item,
            /* [out] */ long __RPC_FAR *count,
            /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *list) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsControls( 
            /* [retval][out] */ long __RPC_FAR *list) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadPIO( 
            /* [retval][out] */ long __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WritePIO( 
            /* [in] */ long value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetSIO( 
            /* [defaultvalue][optional][in] */ BSTR settings = L"9600,N,8,1") = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadSIO( 
            /* [defaultvalue][optional][in] */ long count,
            long timeout,
            /* [retval][out] */ BSTR __RPC_FAR *str) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteSIO( 
            /* [in] */ BSTR str,
            /* [defaultvalue][optional][in] */ long timeout = 10000) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetStrobe( 
            /* [in] */ short index,
            /* [in] */ long duration,
            /* [in] */ long delay,
            /* [in] */ VARIANT_BOOL polarity) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadSettings( 
            /* [in] */ BSTR profile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveSettings( 
            /* [in] */ BSTR profile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DrawText( 
            /* [in] */ short X,
            /* [in] */ short Y,
            /* [in] */ BSTR str,
            /* [in] */ long red,
            /* [defaultvalue][optional][in] */ long green = -1,
            /* [defaultvalue][optional][in] */ long blue = -1,
            /* [defaultvalue][optional][in] */ SHORT opacity = 100) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DrawLine( 
            /* [in] */ short X1,
            /* [in] */ short Y1,
            /* [in] */ short X2,
            /* [in] */ short Y2,
            /* [in] */ short width,
            /* [in] */ long red,
            /* [defaultvalue][optional][in] */ long green = -1,
            /* [defaultvalue][optional][in] */ long blue = -1,
            /* [defaultvalue][optional][in] */ short opacity = 100) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DrawRectangle( 
            /* [in] */ short X1,
            /* [in] */ short Y1,
            /* [in] */ short X2,
            /* [in] */ short Y2,
            /* [in] */ short width,
            /* [in] */ long red,
            /* [defaultvalue][optional][in] */ long green = -1,
            /* [defaultvalue][optional][in] */ long blue = -1,
            /* [defaultvalue][optional][in] */ short opacity = 100) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DrawEllipse( 
            /* [in] */ short X1,
            /* [in] */ short Y1,
            /* [in] */ short X2,
            /* [in] */ short Y2,
            /* [in] */ short width,
            /* [in] */ long red,
            /* [defaultvalue][optional][in] */ long green = -1,
            /* [defaultvalue][optional][in] */ long blue = -1,
            /* [defaultvalue][optional][in] */ short opacity = 100) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DrawPixel( 
            /* [in] */ short X,
            /* [in] */ short Y,
            /* [in] */ long red,
            /* [defaultvalue][optional][in] */ long green = -1,
            /* [defaultvalue][optional][in] */ long blue = -1,
            /* [defaultvalue][optional][in] */ SHORT opacity = 100) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetHistogram( 
            /* [in] */ short component,
            /* [defaultvalue][optional][in] */ long bins,
            /* [defaultvalue][optional][in] */ short step,
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetImageStat( 
            /* [in] */ short component,
            /* [defaultvalue][optional][in] */ short step,
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetROI( 
            /* [in] */ long X1,
            /* [in] */ long Y1,
            /* [in] */ long X2,
            /* [in] */ long Y2,
            /* [defaultvalue][optional][in] */ long l1 = 0,
            /* [defaultvalue][optional][in] */ long l2 = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetROI( 
            /* [retval][out] */ VARIANT __RPC_FAR *value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveBkg( 
            /* [in] */ short mode,
            /* [defaultvalue][optional][in] */ short frames = 8) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAVTActiveCamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAVTActiveCam __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAVTActiveCam __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BackColor )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BackColor )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ OLE_COLOR __RPC_FAR *pclr);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Enabled )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vbool);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Enabled )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbool);
        
        /* [id][propputref] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_Font )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ IFontDisp __RPC_FAR *Font);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Font )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ IFontDisp __RPC_FAR *Font);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Font )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ IFontDisp __RPC_FAR *__RPC_FAR *pFont);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Grab )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ SHORT __RPC_FAR *value);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SizeX )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SizeX )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SizeY )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SizeY )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StartX )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StartX )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StartY )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StartY )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Acquire )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Acquire )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Magnification )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ float __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Magnification )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ float newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ScrollBars )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ScrollBars )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ScrollX )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ScrollX )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ScrollY )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ScrollY )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Palette )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Palette )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Edge )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Edge )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Camera )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Camera )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Mode )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Mode )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rate )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ float __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Rate )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ float newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Trigger )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Trigger )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TriggerMode )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TriggerMode )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TriggerCounter )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TriggerCounter )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TriggerPolarity )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TriggerPolarity )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Asynch )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Asynch )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PacketSize )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PacketSize )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SwapBytes )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SwapBytes )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bayer )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Bayer )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BayerLayout )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BayerLayout )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Display )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Display )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Flip )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Flip )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Brightness )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Brightness )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BrightnessControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BrightnessControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoExposureRef )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoExposureRef )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoExposure )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoExposure )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Sharpness )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Sharpness )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SharpnessControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SharpnessControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Gamma )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Gamma )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_GammaControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_GammaControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Shutter )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Shutter )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShutterControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShutterControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Gain )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Gain )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_GainControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_GainControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Iris )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Iris )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IrisControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IrisControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WhiteBalanceUB )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WhiteBalanceUB )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WhiteBalanceVR )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WhiteBalanceVR )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WhiteBalanceControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WhiteBalanceControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Hue )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Hue )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HueControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HueControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Saturation )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Saturation )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SaturationControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SaturationControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Focus )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Focus )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FocusControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FocusControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Zoom )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Zoom )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ZoomControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ZoomControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Pan )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Pan )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PanControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PanControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Tilt )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Tilt )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TiltControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TiltControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Temperature )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Temperature )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TemperatureControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TemperatureControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OpticalFilter )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OpticalFilter )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OpticalFilterControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OpticalFilterControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TriggerDelay )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TriggerDelay )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WhiteShading )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WhiteShading )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ OLE_COLOR __RPC_FAR *pclr);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WhiteShadingControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WhiteShadingControl )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AboutBox )( 
            IAVTActiveCam __RPC_FAR * This);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OverlayColor )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ OLE_COLOR clr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OverlayColor )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ OLE_COLOR __RPC_FAR *pclr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Overlay )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Overlay )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [id][propputref] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_OverlayFont )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ IFontDisp __RPC_FAR *Font);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OverlayFont )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ IFontDisp __RPC_FAR *Font);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OverlayFont )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ IFontDisp __RPC_FAR *__RPC_FAR *pFont);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BkgCorrect )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BkgCorrect )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BkgName )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BkgName )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetBrightnessMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetBrightnessMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetExposureMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetExposureMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSharpnessMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSharpnessMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetWhiteBalanceMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetWhiteBalanceMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHueMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHueMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSaturationMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSaturationMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetGammaMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetGammaMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetShutterMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetShutterMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetGainMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetGainMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIrisMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIrisMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFocusMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFocusMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetZoomMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetZoomMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPanMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPanMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTiltMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTiltMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetOpticalFilterMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetOpticalFilterMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTriggerDelayMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTriggerDelayMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetWhiteShadingMin )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetWhiteShadingMax )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TriggerSource )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TriggerSource )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BitShift )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BitShift )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Timeout )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Timeout )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AntiTearing )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AntiTearing )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][requestedit][bindable][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rotate )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][requestedit][bindable][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Rotate )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadBlock )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long offset,
            /* [in] */ long pData,
            /* [in] */ long nBytes);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteBlock )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long offset,
            /* [in] */ long pData,
            long nBytes);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRawData )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL isPointer,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetImageWindow )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ SHORT x,
            /* [in] */ SHORT y,
            /* [in] */ SHORT sx,
            /* [in] */ SHORT sy,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPixel )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ SHORT x,
            /* [in] */ SHORT y,
            /* [retval][out] */ long __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRGBPixel )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ SHORT x,
            /* [in] */ SHORT y,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetImageLine )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ SHORT y,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetComponentLine )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ SHORT y,
            /* [in] */ SHORT component,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetImageData )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetComponentData )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ SHORT component,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetImagePointer )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ SHORT x,
            /* [in] */ SHORT y,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveImage )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ BSTR filepath,
            /* [defaultvalue][optional][in] */ long compression);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SoftTrigger )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetWidth )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHeight )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetBytesPerPixel )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadRegister )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long address,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteRegister )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long address,
            /* [in] */ long Val);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCameraList )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetModeList )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRateList )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTriggerList )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartCapture )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ BSTR filepath,
            /* [defaultvalue][optional][in] */ float delay);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StopCapture )( 
            IAVTActiveCam __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadChannel )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ SHORT channel);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveChannel )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ SHORT channel);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMaxChannel )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pChannel);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIsoSpeed )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pSpeed);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetIsoSpeed )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowProperties )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL CameraList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDIB )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *hDIB);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetImageWindow )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ SHORT x,
            /* [in] */ SHORT y,
            /* [in] */ VARIANT value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTriggerInfo )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetF7Info )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPicture )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ IPictureDisp __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Draw )( 
            IAVTActiveCam __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OverlayRectangle )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short StartX,
            /* [in] */ short StartY,
            /* [in] */ short EndX,
            /* [in] */ short EndY,
            /* [defaultvalue][optional][in] */ short Width);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OverlayEllipse )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short StartX,
            /* [in] */ short StartY,
            /* [in] */ short EndX,
            /* [in] */ short EndY,
            /* [defaultvalue][optional][in] */ short Width);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OverlayLine )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short StartX,
            /* [in] */ short StartY,
            /* [in] */ short EndX,
            /* [in] */ short EndY,
            /* [defaultvalue][optional][in] */ short Width);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OverlayText )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short X,
            /* [in] */ short Y,
            /* [in] */ BSTR str);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OverlayClear )( 
            IAVTActiveCam __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OverlayPixel )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short X,
            /* [in] */ short Y);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetItemList )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long item,
            /* [out] */ long __RPC_FAR *count,
            /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *list);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsControls )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *list);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadPIO )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WritePIO )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetSIO )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [defaultvalue][optional][in] */ BSTR settings);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadSIO )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [defaultvalue][optional][in] */ long count,
            long timeout,
            /* [retval][out] */ BSTR __RPC_FAR *str);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteSIO )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ BSTR str,
            /* [defaultvalue][optional][in] */ long timeout);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetStrobe )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short index,
            /* [in] */ long duration,
            /* [in] */ long delay,
            /* [in] */ VARIANT_BOOL polarity);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadSettings )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ BSTR profile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveSettings )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ BSTR profile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DrawText )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short X,
            /* [in] */ short Y,
            /* [in] */ BSTR str,
            /* [in] */ long red,
            /* [defaultvalue][optional][in] */ long green,
            /* [defaultvalue][optional][in] */ long blue,
            /* [defaultvalue][optional][in] */ SHORT opacity);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DrawLine )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short X1,
            /* [in] */ short Y1,
            /* [in] */ short X2,
            /* [in] */ short Y2,
            /* [in] */ short width,
            /* [in] */ long red,
            /* [defaultvalue][optional][in] */ long green,
            /* [defaultvalue][optional][in] */ long blue,
            /* [defaultvalue][optional][in] */ short opacity);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DrawRectangle )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short X1,
            /* [in] */ short Y1,
            /* [in] */ short X2,
            /* [in] */ short Y2,
            /* [in] */ short width,
            /* [in] */ long red,
            /* [defaultvalue][optional][in] */ long green,
            /* [defaultvalue][optional][in] */ long blue,
            /* [defaultvalue][optional][in] */ short opacity);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DrawEllipse )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short X1,
            /* [in] */ short Y1,
            /* [in] */ short X2,
            /* [in] */ short Y2,
            /* [in] */ short width,
            /* [in] */ long red,
            /* [defaultvalue][optional][in] */ long green,
            /* [defaultvalue][optional][in] */ long blue,
            /* [defaultvalue][optional][in] */ short opacity);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DrawPixel )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short X,
            /* [in] */ short Y,
            /* [in] */ long red,
            /* [defaultvalue][optional][in] */ long green,
            /* [defaultvalue][optional][in] */ long blue,
            /* [defaultvalue][optional][in] */ SHORT opacity);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHistogram )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short component,
            /* [defaultvalue][optional][in] */ long bins,
            /* [defaultvalue][optional][in] */ short step,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetImageStat )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short component,
            /* [defaultvalue][optional][in] */ short step,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetROI )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ long X1,
            /* [in] */ long Y1,
            /* [in] */ long X2,
            /* [in] */ long Y2,
            /* [defaultvalue][optional][in] */ long l1,
            /* [defaultvalue][optional][in] */ long l2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetROI )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveBkg )( 
            IAVTActiveCam __RPC_FAR * This,
            /* [in] */ short mode,
            /* [defaultvalue][optional][in] */ short frames);
        
        END_INTERFACE
    } IAVTActiveCamVtbl;

    interface IAVTActiveCam
    {
        CONST_VTBL struct IAVTActiveCamVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAVTActiveCam_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAVTActiveCam_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAVTActiveCam_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAVTActiveCam_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAVTActiveCam_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAVTActiveCam_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAVTActiveCam_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAVTActiveCam_put_BackColor(This,clr)	\
    (This)->lpVtbl -> put_BackColor(This,clr)

#define IAVTActiveCam_get_BackColor(This,pclr)	\
    (This)->lpVtbl -> get_BackColor(This,pclr)

#define IAVTActiveCam_put_Enabled(This,vbool)	\
    (This)->lpVtbl -> put_Enabled(This,vbool)

#define IAVTActiveCam_get_Enabled(This,pbool)	\
    (This)->lpVtbl -> get_Enabled(This,pbool)

#define IAVTActiveCam_putref_Font(This,Font)	\
    (This)->lpVtbl -> putref_Font(This,Font)

#define IAVTActiveCam_put_Font(This,Font)	\
    (This)->lpVtbl -> put_Font(This,Font)

#define IAVTActiveCam_get_Font(This,pFont)	\
    (This)->lpVtbl -> get_Font(This,pFont)

#define IAVTActiveCam_Grab(This,value)	\
    (This)->lpVtbl -> Grab(This,value)

#define IAVTActiveCam_get_SizeX(This,pVal)	\
    (This)->lpVtbl -> get_SizeX(This,pVal)

#define IAVTActiveCam_put_SizeX(This,newVal)	\
    (This)->lpVtbl -> put_SizeX(This,newVal)

#define IAVTActiveCam_get_SizeY(This,pVal)	\
    (This)->lpVtbl -> get_SizeY(This,pVal)

#define IAVTActiveCam_put_SizeY(This,newVal)	\
    (This)->lpVtbl -> put_SizeY(This,newVal)

#define IAVTActiveCam_get_StartX(This,pVal)	\
    (This)->lpVtbl -> get_StartX(This,pVal)

#define IAVTActiveCam_put_StartX(This,newVal)	\
    (This)->lpVtbl -> put_StartX(This,newVal)

#define IAVTActiveCam_get_StartY(This,pVal)	\
    (This)->lpVtbl -> get_StartY(This,pVal)

#define IAVTActiveCam_put_StartY(This,newVal)	\
    (This)->lpVtbl -> put_StartY(This,newVal)

#define IAVTActiveCam_get_Acquire(This,pVal)	\
    (This)->lpVtbl -> get_Acquire(This,pVal)

#define IAVTActiveCam_put_Acquire(This,newVal)	\
    (This)->lpVtbl -> put_Acquire(This,newVal)

#define IAVTActiveCam_get_Magnification(This,pVal)	\
    (This)->lpVtbl -> get_Magnification(This,pVal)

#define IAVTActiveCam_put_Magnification(This,newVal)	\
    (This)->lpVtbl -> put_Magnification(This,newVal)

#define IAVTActiveCam_get_ScrollBars(This,pVal)	\
    (This)->lpVtbl -> get_ScrollBars(This,pVal)

#define IAVTActiveCam_put_ScrollBars(This,newVal)	\
    (This)->lpVtbl -> put_ScrollBars(This,newVal)

#define IAVTActiveCam_get_ScrollX(This,pVal)	\
    (This)->lpVtbl -> get_ScrollX(This,pVal)

#define IAVTActiveCam_put_ScrollX(This,newVal)	\
    (This)->lpVtbl -> put_ScrollX(This,newVal)

#define IAVTActiveCam_get_ScrollY(This,pVal)	\
    (This)->lpVtbl -> get_ScrollY(This,pVal)

#define IAVTActiveCam_put_ScrollY(This,newVal)	\
    (This)->lpVtbl -> put_ScrollY(This,newVal)

#define IAVTActiveCam_get_Palette(This,pVal)	\
    (This)->lpVtbl -> get_Palette(This,pVal)

#define IAVTActiveCam_put_Palette(This,newVal)	\
    (This)->lpVtbl -> put_Palette(This,newVal)

#define IAVTActiveCam_get_Edge(This,pVal)	\
    (This)->lpVtbl -> get_Edge(This,pVal)

#define IAVTActiveCam_put_Edge(This,newVal)	\
    (This)->lpVtbl -> put_Edge(This,newVal)

#define IAVTActiveCam_get_Camera(This,pVal)	\
    (This)->lpVtbl -> get_Camera(This,pVal)

#define IAVTActiveCam_put_Camera(This,newVal)	\
    (This)->lpVtbl -> put_Camera(This,newVal)

#define IAVTActiveCam_get_Mode(This,pVal)	\
    (This)->lpVtbl -> get_Mode(This,pVal)

#define IAVTActiveCam_put_Mode(This,newVal)	\
    (This)->lpVtbl -> put_Mode(This,newVal)

#define IAVTActiveCam_get_Rate(This,pVal)	\
    (This)->lpVtbl -> get_Rate(This,pVal)

#define IAVTActiveCam_put_Rate(This,newVal)	\
    (This)->lpVtbl -> put_Rate(This,newVal)

#define IAVTActiveCam_get_Trigger(This,pVal)	\
    (This)->lpVtbl -> get_Trigger(This,pVal)

#define IAVTActiveCam_put_Trigger(This,newVal)	\
    (This)->lpVtbl -> put_Trigger(This,newVal)

#define IAVTActiveCam_get_TriggerMode(This,pVal)	\
    (This)->lpVtbl -> get_TriggerMode(This,pVal)

#define IAVTActiveCam_put_TriggerMode(This,newVal)	\
    (This)->lpVtbl -> put_TriggerMode(This,newVal)

#define IAVTActiveCam_get_TriggerCounter(This,pVal)	\
    (This)->lpVtbl -> get_TriggerCounter(This,pVal)

#define IAVTActiveCam_put_TriggerCounter(This,newVal)	\
    (This)->lpVtbl -> put_TriggerCounter(This,newVal)

#define IAVTActiveCam_get_TriggerPolarity(This,pVal)	\
    (This)->lpVtbl -> get_TriggerPolarity(This,pVal)

#define IAVTActiveCam_put_TriggerPolarity(This,newVal)	\
    (This)->lpVtbl -> put_TriggerPolarity(This,newVal)

#define IAVTActiveCam_get_Asynch(This,pVal)	\
    (This)->lpVtbl -> get_Asynch(This,pVal)

#define IAVTActiveCam_put_Asynch(This,newVal)	\
    (This)->lpVtbl -> put_Asynch(This,newVal)

#define IAVTActiveCam_get_PacketSize(This,pVal)	\
    (This)->lpVtbl -> get_PacketSize(This,pVal)

#define IAVTActiveCam_put_PacketSize(This,newVal)	\
    (This)->lpVtbl -> put_PacketSize(This,newVal)

#define IAVTActiveCam_get_SwapBytes(This,pVal)	\
    (This)->lpVtbl -> get_SwapBytes(This,pVal)

#define IAVTActiveCam_put_SwapBytes(This,newVal)	\
    (This)->lpVtbl -> put_SwapBytes(This,newVal)

#define IAVTActiveCam_get_Bayer(This,pVal)	\
    (This)->lpVtbl -> get_Bayer(This,pVal)

#define IAVTActiveCam_put_Bayer(This,newVal)	\
    (This)->lpVtbl -> put_Bayer(This,newVal)

#define IAVTActiveCam_get_BayerLayout(This,pVal)	\
    (This)->lpVtbl -> get_BayerLayout(This,pVal)

#define IAVTActiveCam_put_BayerLayout(This,newVal)	\
    (This)->lpVtbl -> put_BayerLayout(This,newVal)

#define IAVTActiveCam_get_Display(This,pVal)	\
    (This)->lpVtbl -> get_Display(This,pVal)

#define IAVTActiveCam_put_Display(This,newVal)	\
    (This)->lpVtbl -> put_Display(This,newVal)

#define IAVTActiveCam_get_Flip(This,pVal)	\
    (This)->lpVtbl -> get_Flip(This,pVal)

#define IAVTActiveCam_put_Flip(This,newVal)	\
    (This)->lpVtbl -> put_Flip(This,newVal)

#define IAVTActiveCam_get_Brightness(This,pVal)	\
    (This)->lpVtbl -> get_Brightness(This,pVal)

#define IAVTActiveCam_put_Brightness(This,newVal)	\
    (This)->lpVtbl -> put_Brightness(This,newVal)

#define IAVTActiveCam_get_BrightnessControl(This,pVal)	\
    (This)->lpVtbl -> get_BrightnessControl(This,pVal)

#define IAVTActiveCam_put_BrightnessControl(This,newVal)	\
    (This)->lpVtbl -> put_BrightnessControl(This,newVal)

#define IAVTActiveCam_get_AutoExposureRef(This,pVal)	\
    (This)->lpVtbl -> get_AutoExposureRef(This,pVal)

#define IAVTActiveCam_put_AutoExposureRef(This,newVal)	\
    (This)->lpVtbl -> put_AutoExposureRef(This,newVal)

#define IAVTActiveCam_get_AutoExposure(This,pVal)	\
    (This)->lpVtbl -> get_AutoExposure(This,pVal)

#define IAVTActiveCam_put_AutoExposure(This,newVal)	\
    (This)->lpVtbl -> put_AutoExposure(This,newVal)

#define IAVTActiveCam_get_Sharpness(This,pVal)	\
    (This)->lpVtbl -> get_Sharpness(This,pVal)

#define IAVTActiveCam_put_Sharpness(This,newVal)	\
    (This)->lpVtbl -> put_Sharpness(This,newVal)

#define IAVTActiveCam_get_SharpnessControl(This,pVal)	\
    (This)->lpVtbl -> get_SharpnessControl(This,pVal)

#define IAVTActiveCam_put_SharpnessControl(This,newVal)	\
    (This)->lpVtbl -> put_SharpnessControl(This,newVal)

#define IAVTActiveCam_get_Gamma(This,pVal)	\
    (This)->lpVtbl -> get_Gamma(This,pVal)

#define IAVTActiveCam_put_Gamma(This,newVal)	\
    (This)->lpVtbl -> put_Gamma(This,newVal)

#define IAVTActiveCam_get_GammaControl(This,pVal)	\
    (This)->lpVtbl -> get_GammaControl(This,pVal)

#define IAVTActiveCam_put_GammaControl(This,newVal)	\
    (This)->lpVtbl -> put_GammaControl(This,newVal)

#define IAVTActiveCam_get_Shutter(This,pVal)	\
    (This)->lpVtbl -> get_Shutter(This,pVal)

#define IAVTActiveCam_put_Shutter(This,newVal)	\
    (This)->lpVtbl -> put_Shutter(This,newVal)

#define IAVTActiveCam_get_ShutterControl(This,pVal)	\
    (This)->lpVtbl -> get_ShutterControl(This,pVal)

#define IAVTActiveCam_put_ShutterControl(This,newVal)	\
    (This)->lpVtbl -> put_ShutterControl(This,newVal)

#define IAVTActiveCam_get_Gain(This,pVal)	\
    (This)->lpVtbl -> get_Gain(This,pVal)

#define IAVTActiveCam_put_Gain(This,newVal)	\
    (This)->lpVtbl -> put_Gain(This,newVal)

#define IAVTActiveCam_get_GainControl(This,pVal)	\
    (This)->lpVtbl -> get_GainControl(This,pVal)

#define IAVTActiveCam_put_GainControl(This,newVal)	\
    (This)->lpVtbl -> put_GainControl(This,newVal)

#define IAVTActiveCam_get_Iris(This,pVal)	\
    (This)->lpVtbl -> get_Iris(This,pVal)

#define IAVTActiveCam_put_Iris(This,newVal)	\
    (This)->lpVtbl -> put_Iris(This,newVal)

#define IAVTActiveCam_get_IrisControl(This,pVal)	\
    (This)->lpVtbl -> get_IrisControl(This,pVal)

#define IAVTActiveCam_put_IrisControl(This,newVal)	\
    (This)->lpVtbl -> put_IrisControl(This,newVal)

#define IAVTActiveCam_get_WhiteBalanceUB(This,pVal)	\
    (This)->lpVtbl -> get_WhiteBalanceUB(This,pVal)

#define IAVTActiveCam_put_WhiteBalanceUB(This,newVal)	\
    (This)->lpVtbl -> put_WhiteBalanceUB(This,newVal)

#define IAVTActiveCam_get_WhiteBalanceVR(This,pVal)	\
    (This)->lpVtbl -> get_WhiteBalanceVR(This,pVal)

#define IAVTActiveCam_put_WhiteBalanceVR(This,newVal)	\
    (This)->lpVtbl -> put_WhiteBalanceVR(This,newVal)

#define IAVTActiveCam_get_WhiteBalanceControl(This,pVal)	\
    (This)->lpVtbl -> get_WhiteBalanceControl(This,pVal)

#define IAVTActiveCam_put_WhiteBalanceControl(This,newVal)	\
    (This)->lpVtbl -> put_WhiteBalanceControl(This,newVal)

#define IAVTActiveCam_get_Hue(This,pVal)	\
    (This)->lpVtbl -> get_Hue(This,pVal)

#define IAVTActiveCam_put_Hue(This,newVal)	\
    (This)->lpVtbl -> put_Hue(This,newVal)

#define IAVTActiveCam_get_HueControl(This,pVal)	\
    (This)->lpVtbl -> get_HueControl(This,pVal)

#define IAVTActiveCam_put_HueControl(This,newVal)	\
    (This)->lpVtbl -> put_HueControl(This,newVal)

#define IAVTActiveCam_get_Saturation(This,pVal)	\
    (This)->lpVtbl -> get_Saturation(This,pVal)

#define IAVTActiveCam_put_Saturation(This,newVal)	\
    (This)->lpVtbl -> put_Saturation(This,newVal)

#define IAVTActiveCam_get_SaturationControl(This,pVal)	\
    (This)->lpVtbl -> get_SaturationControl(This,pVal)

#define IAVTActiveCam_put_SaturationControl(This,newVal)	\
    (This)->lpVtbl -> put_SaturationControl(This,newVal)

#define IAVTActiveCam_get_Focus(This,pVal)	\
    (This)->lpVtbl -> get_Focus(This,pVal)

#define IAVTActiveCam_put_Focus(This,newVal)	\
    (This)->lpVtbl -> put_Focus(This,newVal)

#define IAVTActiveCam_get_FocusControl(This,pVal)	\
    (This)->lpVtbl -> get_FocusControl(This,pVal)

#define IAVTActiveCam_put_FocusControl(This,newVal)	\
    (This)->lpVtbl -> put_FocusControl(This,newVal)

#define IAVTActiveCam_get_Zoom(This,pVal)	\
    (This)->lpVtbl -> get_Zoom(This,pVal)

#define IAVTActiveCam_put_Zoom(This,newVal)	\
    (This)->lpVtbl -> put_Zoom(This,newVal)

#define IAVTActiveCam_get_ZoomControl(This,pVal)	\
    (This)->lpVtbl -> get_ZoomControl(This,pVal)

#define IAVTActiveCam_put_ZoomControl(This,newVal)	\
    (This)->lpVtbl -> put_ZoomControl(This,newVal)

#define IAVTActiveCam_get_Pan(This,pVal)	\
    (This)->lpVtbl -> get_Pan(This,pVal)

#define IAVTActiveCam_put_Pan(This,newVal)	\
    (This)->lpVtbl -> put_Pan(This,newVal)

#define IAVTActiveCam_get_PanControl(This,pVal)	\
    (This)->lpVtbl -> get_PanControl(This,pVal)

#define IAVTActiveCam_put_PanControl(This,newVal)	\
    (This)->lpVtbl -> put_PanControl(This,newVal)

#define IAVTActiveCam_get_Tilt(This,pVal)	\
    (This)->lpVtbl -> get_Tilt(This,pVal)

#define IAVTActiveCam_put_Tilt(This,newVal)	\
    (This)->lpVtbl -> put_Tilt(This,newVal)

#define IAVTActiveCam_get_TiltControl(This,pVal)	\
    (This)->lpVtbl -> get_TiltControl(This,pVal)

#define IAVTActiveCam_put_TiltControl(This,newVal)	\
    (This)->lpVtbl -> put_TiltControl(This,newVal)

#define IAVTActiveCam_get_Temperature(This,pVal)	\
    (This)->lpVtbl -> get_Temperature(This,pVal)

#define IAVTActiveCam_put_Temperature(This,newVal)	\
    (This)->lpVtbl -> put_Temperature(This,newVal)

#define IAVTActiveCam_get_TemperatureControl(This,pVal)	\
    (This)->lpVtbl -> get_TemperatureControl(This,pVal)

#define IAVTActiveCam_put_TemperatureControl(This,newVal)	\
    (This)->lpVtbl -> put_TemperatureControl(This,newVal)

#define IAVTActiveCam_get_OpticalFilter(This,pVal)	\
    (This)->lpVtbl -> get_OpticalFilter(This,pVal)

#define IAVTActiveCam_put_OpticalFilter(This,newVal)	\
    (This)->lpVtbl -> put_OpticalFilter(This,newVal)

#define IAVTActiveCam_get_OpticalFilterControl(This,pVal)	\
    (This)->lpVtbl -> get_OpticalFilterControl(This,pVal)

#define IAVTActiveCam_put_OpticalFilterControl(This,newVal)	\
    (This)->lpVtbl -> put_OpticalFilterControl(This,newVal)

#define IAVTActiveCam_get_TriggerDelay(This,pVal)	\
    (This)->lpVtbl -> get_TriggerDelay(This,pVal)

#define IAVTActiveCam_put_TriggerDelay(This,newVal)	\
    (This)->lpVtbl -> put_TriggerDelay(This,newVal)

#define IAVTActiveCam_put_WhiteShading(This,clr)	\
    (This)->lpVtbl -> put_WhiteShading(This,clr)

#define IAVTActiveCam_get_WhiteShading(This,pclr)	\
    (This)->lpVtbl -> get_WhiteShading(This,pclr)

#define IAVTActiveCam_get_WhiteShadingControl(This,pVal)	\
    (This)->lpVtbl -> get_WhiteShadingControl(This,pVal)

#define IAVTActiveCam_put_WhiteShadingControl(This,newVal)	\
    (This)->lpVtbl -> put_WhiteShadingControl(This,newVal)

#define IAVTActiveCam_AboutBox(This)	\
    (This)->lpVtbl -> AboutBox(This)

#define IAVTActiveCam_put_OverlayColor(This,clr)	\
    (This)->lpVtbl -> put_OverlayColor(This,clr)

#define IAVTActiveCam_get_OverlayColor(This,pclr)	\
    (This)->lpVtbl -> get_OverlayColor(This,pclr)

#define IAVTActiveCam_get_Overlay(This,pVal)	\
    (This)->lpVtbl -> get_Overlay(This,pVal)

#define IAVTActiveCam_put_Overlay(This,newVal)	\
    (This)->lpVtbl -> put_Overlay(This,newVal)

#define IAVTActiveCam_putref_OverlayFont(This,Font)	\
    (This)->lpVtbl -> putref_OverlayFont(This,Font)

#define IAVTActiveCam_put_OverlayFont(This,Font)	\
    (This)->lpVtbl -> put_OverlayFont(This,Font)

#define IAVTActiveCam_get_OverlayFont(This,pFont)	\
    (This)->lpVtbl -> get_OverlayFont(This,pFont)

#define IAVTActiveCam_get_BkgCorrect(This,pVal)	\
    (This)->lpVtbl -> get_BkgCorrect(This,pVal)

#define IAVTActiveCam_put_BkgCorrect(This,newVal)	\
    (This)->lpVtbl -> put_BkgCorrect(This,newVal)

#define IAVTActiveCam_get_BkgName(This,pVal)	\
    (This)->lpVtbl -> get_BkgName(This,pVal)

#define IAVTActiveCam_put_BkgName(This,newVal)	\
    (This)->lpVtbl -> put_BkgName(This,newVal)

#define IAVTActiveCam_GetBrightnessMin(This,pVal)	\
    (This)->lpVtbl -> GetBrightnessMin(This,pVal)

#define IAVTActiveCam_GetBrightnessMax(This,pVal)	\
    (This)->lpVtbl -> GetBrightnessMax(This,pVal)

#define IAVTActiveCam_GetExposureMin(This,pVal)	\
    (This)->lpVtbl -> GetExposureMin(This,pVal)

#define IAVTActiveCam_GetExposureMax(This,pVal)	\
    (This)->lpVtbl -> GetExposureMax(This,pVal)

#define IAVTActiveCam_GetSharpnessMin(This,pVal)	\
    (This)->lpVtbl -> GetSharpnessMin(This,pVal)

#define IAVTActiveCam_GetSharpnessMax(This,pVal)	\
    (This)->lpVtbl -> GetSharpnessMax(This,pVal)

#define IAVTActiveCam_GetWhiteBalanceMin(This,pVal)	\
    (This)->lpVtbl -> GetWhiteBalanceMin(This,pVal)

#define IAVTActiveCam_GetWhiteBalanceMax(This,pVal)	\
    (This)->lpVtbl -> GetWhiteBalanceMax(This,pVal)

#define IAVTActiveCam_GetHueMin(This,pVal)	\
    (This)->lpVtbl -> GetHueMin(This,pVal)

#define IAVTActiveCam_GetHueMax(This,pVal)	\
    (This)->lpVtbl -> GetHueMax(This,pVal)

#define IAVTActiveCam_GetSaturationMin(This,pVal)	\
    (This)->lpVtbl -> GetSaturationMin(This,pVal)

#define IAVTActiveCam_GetSaturationMax(This,pVal)	\
    (This)->lpVtbl -> GetSaturationMax(This,pVal)

#define IAVTActiveCam_GetGammaMin(This,pVal)	\
    (This)->lpVtbl -> GetGammaMin(This,pVal)

#define IAVTActiveCam_GetGammaMax(This,pVal)	\
    (This)->lpVtbl -> GetGammaMax(This,pVal)

#define IAVTActiveCam_GetShutterMin(This,pVal)	\
    (This)->lpVtbl -> GetShutterMin(This,pVal)

#define IAVTActiveCam_GetShutterMax(This,pVal)	\
    (This)->lpVtbl -> GetShutterMax(This,pVal)

#define IAVTActiveCam_GetGainMin(This,pVal)	\
    (This)->lpVtbl -> GetGainMin(This,pVal)

#define IAVTActiveCam_GetGainMax(This,pVal)	\
    (This)->lpVtbl -> GetGainMax(This,pVal)

#define IAVTActiveCam_GetIrisMin(This,pVal)	\
    (This)->lpVtbl -> GetIrisMin(This,pVal)

#define IAVTActiveCam_GetIrisMax(This,pVal)	\
    (This)->lpVtbl -> GetIrisMax(This,pVal)

#define IAVTActiveCam_GetFocusMin(This,pVal)	\
    (This)->lpVtbl -> GetFocusMin(This,pVal)

#define IAVTActiveCam_GetFocusMax(This,pVal)	\
    (This)->lpVtbl -> GetFocusMax(This,pVal)

#define IAVTActiveCam_GetZoomMin(This,pVal)	\
    (This)->lpVtbl -> GetZoomMin(This,pVal)

#define IAVTActiveCam_GetZoomMax(This,pVal)	\
    (This)->lpVtbl -> GetZoomMax(This,pVal)

#define IAVTActiveCam_GetPanMin(This,pVal)	\
    (This)->lpVtbl -> GetPanMin(This,pVal)

#define IAVTActiveCam_GetPanMax(This,pVal)	\
    (This)->lpVtbl -> GetPanMax(This,pVal)

#define IAVTActiveCam_GetTiltMin(This,pVal)	\
    (This)->lpVtbl -> GetTiltMin(This,pVal)

#define IAVTActiveCam_GetTiltMax(This,pVal)	\
    (This)->lpVtbl -> GetTiltMax(This,pVal)

#define IAVTActiveCam_GetOpticalFilterMin(This,pVal)	\
    (This)->lpVtbl -> GetOpticalFilterMin(This,pVal)

#define IAVTActiveCam_GetOpticalFilterMax(This,pVal)	\
    (This)->lpVtbl -> GetOpticalFilterMax(This,pVal)

#define IAVTActiveCam_GetTriggerDelayMin(This,pVal)	\
    (This)->lpVtbl -> GetTriggerDelayMin(This,pVal)

#define IAVTActiveCam_GetTriggerDelayMax(This,pVal)	\
    (This)->lpVtbl -> GetTriggerDelayMax(This,pVal)

#define IAVTActiveCam_GetWhiteShadingMin(This,pVal)	\
    (This)->lpVtbl -> GetWhiteShadingMin(This,pVal)

#define IAVTActiveCam_GetWhiteShadingMax(This,pVal)	\
    (This)->lpVtbl -> GetWhiteShadingMax(This,pVal)

#define IAVTActiveCam_get_TriggerSource(This,pVal)	\
    (This)->lpVtbl -> get_TriggerSource(This,pVal)

#define IAVTActiveCam_put_TriggerSource(This,newVal)	\
    (This)->lpVtbl -> put_TriggerSource(This,newVal)

#define IAVTActiveCam_get_BitShift(This,pVal)	\
    (This)->lpVtbl -> get_BitShift(This,pVal)

#define IAVTActiveCam_put_BitShift(This,newVal)	\
    (This)->lpVtbl -> put_BitShift(This,newVal)

#define IAVTActiveCam_get_Timeout(This,pVal)	\
    (This)->lpVtbl -> get_Timeout(This,pVal)

#define IAVTActiveCam_put_Timeout(This,newVal)	\
    (This)->lpVtbl -> put_Timeout(This,newVal)

#define IAVTActiveCam_get_AntiTearing(This,pVal)	\
    (This)->lpVtbl -> get_AntiTearing(This,pVal)

#define IAVTActiveCam_put_AntiTearing(This,newVal)	\
    (This)->lpVtbl -> put_AntiTearing(This,newVal)

#define IAVTActiveCam_get_Rotate(This,pVal)	\
    (This)->lpVtbl -> get_Rotate(This,pVal)

#define IAVTActiveCam_put_Rotate(This,newVal)	\
    (This)->lpVtbl -> put_Rotate(This,newVal)

#define IAVTActiveCam_ReadBlock(This,offset,pData,nBytes)	\
    (This)->lpVtbl -> ReadBlock(This,offset,pData,nBytes)

#define IAVTActiveCam_WriteBlock(This,offset,pData,nBytes)	\
    (This)->lpVtbl -> WriteBlock(This,offset,pData,nBytes)

#define IAVTActiveCam_GetRawData(This,isPointer,value)	\
    (This)->lpVtbl -> GetRawData(This,isPointer,value)

#define IAVTActiveCam_GetImageWindow(This,x,y,sx,sy,value)	\
    (This)->lpVtbl -> GetImageWindow(This,x,y,sx,sy,value)

#define IAVTActiveCam_GetPixel(This,x,y,value)	\
    (This)->lpVtbl -> GetPixel(This,x,y,value)

#define IAVTActiveCam_GetRGBPixel(This,x,y,value)	\
    (This)->lpVtbl -> GetRGBPixel(This,x,y,value)

#define IAVTActiveCam_GetImageLine(This,y,value)	\
    (This)->lpVtbl -> GetImageLine(This,y,value)

#define IAVTActiveCam_GetComponentLine(This,y,component,value)	\
    (This)->lpVtbl -> GetComponentLine(This,y,component,value)

#define IAVTActiveCam_GetImageData(This,value)	\
    (This)->lpVtbl -> GetImageData(This,value)

#define IAVTActiveCam_GetComponentData(This,component,value)	\
    (This)->lpVtbl -> GetComponentData(This,component,value)

#define IAVTActiveCam_GetImagePointer(This,x,y,value)	\
    (This)->lpVtbl -> GetImagePointer(This,x,y,value)

#define IAVTActiveCam_SaveImage(This,filepath,compression)	\
    (This)->lpVtbl -> SaveImage(This,filepath,compression)

#define IAVTActiveCam_SoftTrigger(This,newVal)	\
    (This)->lpVtbl -> SoftTrigger(This,newVal)

#define IAVTActiveCam_GetWidth(This,pVal)	\
    (This)->lpVtbl -> GetWidth(This,pVal)

#define IAVTActiveCam_GetHeight(This,pVal)	\
    (This)->lpVtbl -> GetHeight(This,pVal)

#define IAVTActiveCam_GetBytesPerPixel(This,pVal)	\
    (This)->lpVtbl -> GetBytesPerPixel(This,pVal)

#define IAVTActiveCam_ReadRegister(This,address,pVal)	\
    (This)->lpVtbl -> ReadRegister(This,address,pVal)

#define IAVTActiveCam_WriteRegister(This,address,Val)	\
    (This)->lpVtbl -> WriteRegister(This,address,Val)

#define IAVTActiveCam_GetCameraList(This,value)	\
    (This)->lpVtbl -> GetCameraList(This,value)

#define IAVTActiveCam_GetModeList(This,value)	\
    (This)->lpVtbl -> GetModeList(This,value)

#define IAVTActiveCam_GetRateList(This,value)	\
    (This)->lpVtbl -> GetRateList(This,value)

#define IAVTActiveCam_GetTriggerList(This,value)	\
    (This)->lpVtbl -> GetTriggerList(This,value)

#define IAVTActiveCam_StartCapture(This,filepath,delay)	\
    (This)->lpVtbl -> StartCapture(This,filepath,delay)

#define IAVTActiveCam_StopCapture(This)	\
    (This)->lpVtbl -> StopCapture(This)

#define IAVTActiveCam_LoadChannel(This,channel)	\
    (This)->lpVtbl -> LoadChannel(This,channel)

#define IAVTActiveCam_SaveChannel(This,channel)	\
    (This)->lpVtbl -> SaveChannel(This,channel)

#define IAVTActiveCam_GetMaxChannel(This,pChannel)	\
    (This)->lpVtbl -> GetMaxChannel(This,pChannel)

#define IAVTActiveCam_GetIsoSpeed(This,pSpeed)	\
    (This)->lpVtbl -> GetIsoSpeed(This,pSpeed)

#define IAVTActiveCam_SetIsoSpeed(This,value)	\
    (This)->lpVtbl -> SetIsoSpeed(This,value)

#define IAVTActiveCam_ShowProperties(This,CameraList)	\
    (This)->lpVtbl -> ShowProperties(This,CameraList)

#define IAVTActiveCam_GetDIB(This,hDIB)	\
    (This)->lpVtbl -> GetDIB(This,hDIB)

#define IAVTActiveCam_SetImageWindow(This,x,y,value)	\
    (This)->lpVtbl -> SetImageWindow(This,x,y,value)

#define IAVTActiveCam_GetTriggerInfo(This,value)	\
    (This)->lpVtbl -> GetTriggerInfo(This,value)

#define IAVTActiveCam_GetF7Info(This,value)	\
    (This)->lpVtbl -> GetF7Info(This,value)

#define IAVTActiveCam_GetPicture(This,pVal)	\
    (This)->lpVtbl -> GetPicture(This,pVal)

#define IAVTActiveCam_Draw(This)	\
    (This)->lpVtbl -> Draw(This)

#define IAVTActiveCam_OverlayRectangle(This,StartX,StartY,EndX,EndY,Width)	\
    (This)->lpVtbl -> OverlayRectangle(This,StartX,StartY,EndX,EndY,Width)

#define IAVTActiveCam_OverlayEllipse(This,StartX,StartY,EndX,EndY,Width)	\
    (This)->lpVtbl -> OverlayEllipse(This,StartX,StartY,EndX,EndY,Width)

#define IAVTActiveCam_OverlayLine(This,StartX,StartY,EndX,EndY,Width)	\
    (This)->lpVtbl -> OverlayLine(This,StartX,StartY,EndX,EndY,Width)

#define IAVTActiveCam_OverlayText(This,X,Y,str)	\
    (This)->lpVtbl -> OverlayText(This,X,Y,str)

#define IAVTActiveCam_OverlayClear(This)	\
    (This)->lpVtbl -> OverlayClear(This)

#define IAVTActiveCam_OverlayPixel(This,X,Y)	\
    (This)->lpVtbl -> OverlayPixel(This,X,Y)

#define IAVTActiveCam_GetItemList(This,item,count,list)	\
    (This)->lpVtbl -> GetItemList(This,item,count,list)

#define IAVTActiveCam_IsControls(This,list)	\
    (This)->lpVtbl -> IsControls(This,list)

#define IAVTActiveCam_ReadPIO(This,value)	\
    (This)->lpVtbl -> ReadPIO(This,value)

#define IAVTActiveCam_WritePIO(This,value)	\
    (This)->lpVtbl -> WritePIO(This,value)

#define IAVTActiveCam_SetSIO(This,settings)	\
    (This)->lpVtbl -> SetSIO(This,settings)

#define IAVTActiveCam_ReadSIO(This,count,timeout,str)	\
    (This)->lpVtbl -> ReadSIO(This,count,timeout,str)

#define IAVTActiveCam_WriteSIO(This,str,timeout)	\
    (This)->lpVtbl -> WriteSIO(This,str,timeout)

#define IAVTActiveCam_SetStrobe(This,index,duration,delay,polarity)	\
    (This)->lpVtbl -> SetStrobe(This,index,duration,delay,polarity)

#define IAVTActiveCam_LoadSettings(This,profile)	\
    (This)->lpVtbl -> LoadSettings(This,profile)

#define IAVTActiveCam_SaveSettings(This,profile)	\
    (This)->lpVtbl -> SaveSettings(This,profile)

#define IAVTActiveCam_DrawText(This,X,Y,str,red,green,blue,opacity)	\
    (This)->lpVtbl -> DrawText(This,X,Y,str,red,green,blue,opacity)

#define IAVTActiveCam_DrawLine(This,X1,Y1,X2,Y2,width,red,green,blue,opacity)	\
    (This)->lpVtbl -> DrawLine(This,X1,Y1,X2,Y2,width,red,green,blue,opacity)

#define IAVTActiveCam_DrawRectangle(This,X1,Y1,X2,Y2,width,red,green,blue,opacity)	\
    (This)->lpVtbl -> DrawRectangle(This,X1,Y1,X2,Y2,width,red,green,blue,opacity)

#define IAVTActiveCam_DrawEllipse(This,X1,Y1,X2,Y2,width,red,green,blue,opacity)	\
    (This)->lpVtbl -> DrawEllipse(This,X1,Y1,X2,Y2,width,red,green,blue,opacity)

#define IAVTActiveCam_DrawPixel(This,X,Y,red,green,blue,opacity)	\
    (This)->lpVtbl -> DrawPixel(This,X,Y,red,green,blue,opacity)

#define IAVTActiveCam_GetHistogram(This,component,bins,step,value)	\
    (This)->lpVtbl -> GetHistogram(This,component,bins,step,value)

#define IAVTActiveCam_GetImageStat(This,component,step,value)	\
    (This)->lpVtbl -> GetImageStat(This,component,step,value)

#define IAVTActiveCam_SetROI(This,X1,Y1,X2,Y2,l1,l2)	\
    (This)->lpVtbl -> SetROI(This,X1,Y1,X2,Y2,l1,l2)

#define IAVTActiveCam_GetROI(This,value)	\
    (This)->lpVtbl -> GetROI(This,value)

#define IAVTActiveCam_SaveBkg(This,mode,frames)	\
    (This)->lpVtbl -> SaveBkg(This,mode,frames)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_BackColor_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ OLE_COLOR clr);


void __RPC_STUB IAVTActiveCam_put_BackColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_BackColor_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ OLE_COLOR __RPC_FAR *pclr);


void __RPC_STUB IAVTActiveCam_get_BackColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Enabled_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vbool);


void __RPC_STUB IAVTActiveCam_put_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Enabled_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbool);


void __RPC_STUB IAVTActiveCam_get_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propputref] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_putref_Font_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ IFontDisp __RPC_FAR *Font);


void __RPC_STUB IAVTActiveCam_putref_Font_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Font_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ IFontDisp __RPC_FAR *Font);


void __RPC_STUB IAVTActiveCam_put_Font_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Font_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ IFontDisp __RPC_FAR *__RPC_FAR *pFont);


void __RPC_STUB IAVTActiveCam_get_Font_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_Grab_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ SHORT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_Grab_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_SizeX_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_SizeX_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_SizeX_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_SizeX_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_SizeY_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_SizeY_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_SizeY_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_SizeY_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_StartX_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_StartX_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_StartX_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_StartX_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_StartY_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_StartY_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_StartY_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_StartY_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Acquire_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Acquire_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Acquire_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IAVTActiveCam_put_Acquire_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Magnification_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ float __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Magnification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Magnification_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ float newVal);


void __RPC_STUB IAVTActiveCam_put_Magnification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_ScrollBars_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_ScrollBars_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_ScrollBars_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IAVTActiveCam_put_ScrollBars_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_ScrollX_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_ScrollX_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_ScrollX_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_ScrollX_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_ScrollY_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_ScrollY_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_ScrollY_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_ScrollY_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Palette_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Palette_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Palette_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Palette_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Edge_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Edge_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Edge_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IAVTActiveCam_put_Edge_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Camera_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Camera_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Camera_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Camera_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Mode_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Mode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Mode_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Mode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Rate_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ float __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Rate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Rate_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ float newVal);


void __RPC_STUB IAVTActiveCam_put_Rate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Trigger_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Trigger_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Trigger_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IAVTActiveCam_put_Trigger_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_TriggerMode_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_TriggerMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_TriggerMode_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_TriggerMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_TriggerCounter_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_TriggerCounter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_TriggerCounter_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_TriggerCounter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_TriggerPolarity_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_TriggerPolarity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_TriggerPolarity_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IAVTActiveCam_put_TriggerPolarity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Asynch_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Asynch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Asynch_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IAVTActiveCam_put_Asynch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_PacketSize_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_PacketSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_PacketSize_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_PacketSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_SwapBytes_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_SwapBytes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_SwapBytes_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IAVTActiveCam_put_SwapBytes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Bayer_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Bayer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Bayer_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_Bayer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_BayerLayout_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_BayerLayout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_BayerLayout_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_BayerLayout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Display_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Display_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Display_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IAVTActiveCam_put_Display_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Flip_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Flip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Flip_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Flip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Brightness_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Brightness_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Brightness_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Brightness_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_BrightnessControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_BrightnessControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_BrightnessControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_BrightnessControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_AutoExposureRef_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_AutoExposureRef_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_AutoExposureRef_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_AutoExposureRef_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_AutoExposure_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_AutoExposure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_AutoExposure_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IAVTActiveCam_put_AutoExposure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Sharpness_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Sharpness_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Sharpness_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Sharpness_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_SharpnessControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_SharpnessControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_SharpnessControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_SharpnessControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Gamma_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Gamma_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Gamma_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Gamma_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_GammaControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_GammaControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_GammaControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_GammaControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Shutter_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Shutter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Shutter_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Shutter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_ShutterControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_ShutterControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_ShutterControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_ShutterControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Gain_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Gain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Gain_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Gain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_GainControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_GainControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_GainControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_GainControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Iris_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Iris_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Iris_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Iris_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_IrisControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_IrisControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_IrisControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_IrisControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_WhiteBalanceUB_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_WhiteBalanceUB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_WhiteBalanceUB_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_WhiteBalanceUB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_WhiteBalanceVR_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_WhiteBalanceVR_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_WhiteBalanceVR_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_WhiteBalanceVR_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_WhiteBalanceControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_WhiteBalanceControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_WhiteBalanceControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_WhiteBalanceControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Hue_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Hue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Hue_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Hue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_HueControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_HueControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_HueControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_HueControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Saturation_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Saturation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Saturation_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Saturation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_SaturationControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_SaturationControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_SaturationControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_SaturationControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Focus_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Focus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Focus_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Focus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_FocusControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_FocusControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_FocusControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_FocusControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Zoom_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Zoom_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Zoom_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Zoom_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_ZoomControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_ZoomControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_ZoomControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_ZoomControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Pan_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Pan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Pan_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Pan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_PanControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_PanControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_PanControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_PanControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Tilt_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Tilt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Tilt_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Tilt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_TiltControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_TiltControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_TiltControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_TiltControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Temperature_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Temperature_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Temperature_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Temperature_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_TemperatureControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_TemperatureControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_TemperatureControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_TemperatureControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_OpticalFilter_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_OpticalFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_OpticalFilter_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_OpticalFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_OpticalFilterControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_OpticalFilterControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_OpticalFilterControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_OpticalFilterControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_TriggerDelay_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_TriggerDelay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_TriggerDelay_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_TriggerDelay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_WhiteShading_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ OLE_COLOR clr);


void __RPC_STUB IAVTActiveCam_put_WhiteShading_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_WhiteShading_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ OLE_COLOR __RPC_FAR *pclr);


void __RPC_STUB IAVTActiveCam_get_WhiteShading_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_WhiteShadingControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_WhiteShadingControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_WhiteShadingControl_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_WhiteShadingControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_AboutBox_Proxy( 
    IAVTActiveCam __RPC_FAR * This);


void __RPC_STUB IAVTActiveCam_AboutBox_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_OverlayColor_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ OLE_COLOR clr);


void __RPC_STUB IAVTActiveCam_put_OverlayColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_OverlayColor_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ OLE_COLOR __RPC_FAR *pclr);


void __RPC_STUB IAVTActiveCam_get_OverlayColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Overlay_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Overlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Overlay_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IAVTActiveCam_put_Overlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propputref] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_putref_OverlayFont_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ IFontDisp __RPC_FAR *Font);


void __RPC_STUB IAVTActiveCam_putref_OverlayFont_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_OverlayFont_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ IFontDisp __RPC_FAR *Font);


void __RPC_STUB IAVTActiveCam_put_OverlayFont_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_OverlayFont_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ IFontDisp __RPC_FAR *__RPC_FAR *pFont);


void __RPC_STUB IAVTActiveCam_get_OverlayFont_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_BkgCorrect_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_BkgCorrect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_BkgCorrect_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short newVal);


void __RPC_STUB IAVTActiveCam_put_BkgCorrect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_BkgName_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_BkgName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_BkgName_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IAVTActiveCam_put_BkgName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetBrightnessMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetBrightnessMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetBrightnessMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetBrightnessMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetExposureMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetExposureMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetExposureMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetExposureMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetSharpnessMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetSharpnessMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetSharpnessMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetSharpnessMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetWhiteBalanceMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetWhiteBalanceMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetWhiteBalanceMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetWhiteBalanceMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetHueMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetHueMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetHueMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetHueMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetSaturationMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetSaturationMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetSaturationMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetSaturationMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetGammaMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetGammaMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetGammaMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetGammaMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetShutterMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetShutterMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetShutterMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetShutterMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetGainMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetGainMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetGainMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetGainMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetIrisMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetIrisMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetIrisMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetIrisMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetFocusMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetFocusMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetFocusMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetFocusMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetZoomMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetZoomMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetZoomMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetZoomMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetPanMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetPanMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetPanMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetPanMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetTiltMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetTiltMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetTiltMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetTiltMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetOpticalFilterMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetOpticalFilterMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetOpticalFilterMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetOpticalFilterMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetTriggerDelayMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetTriggerDelayMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetTriggerDelayMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetTriggerDelayMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetWhiteShadingMin_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetWhiteShadingMin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetWhiteShadingMax_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetWhiteShadingMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_TriggerSource_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_TriggerSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_TriggerSource_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_TriggerSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_BitShift_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_BitShift_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_BitShift_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_BitShift_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Timeout_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Timeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Timeout_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Timeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_AntiTearing_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_AntiTearing_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_AntiTearing_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IAVTActiveCam_put_AntiTearing_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propget] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_get_Rotate_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_get_Rotate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][requestedit][bindable][id][propput] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_put_Rotate_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IAVTActiveCam_put_Rotate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_ReadBlock_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long offset,
    /* [in] */ long pData,
    /* [in] */ long nBytes);


void __RPC_STUB IAVTActiveCam_ReadBlock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_WriteBlock_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long offset,
    /* [in] */ long pData,
    long nBytes);


void __RPC_STUB IAVTActiveCam_WriteBlock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetRawData_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL isPointer,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetRawData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetImageWindow_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ SHORT x,
    /* [in] */ SHORT y,
    /* [in] */ SHORT sx,
    /* [in] */ SHORT sy,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetImageWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetPixel_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ SHORT x,
    /* [in] */ SHORT y,
    /* [retval][out] */ long __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetPixel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetRGBPixel_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ SHORT x,
    /* [in] */ SHORT y,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetRGBPixel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetImageLine_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ SHORT y,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetImageLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetComponentLine_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ SHORT y,
    /* [in] */ SHORT component,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetComponentLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetImageData_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetImageData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetComponentData_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ SHORT component,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetComponentData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetImagePointer_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ SHORT x,
    /* [in] */ SHORT y,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetImagePointer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_SaveImage_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ BSTR filepath,
    /* [defaultvalue][optional][in] */ long compression);


void __RPC_STUB IAVTActiveCam_SaveImage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_SoftTrigger_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IAVTActiveCam_SoftTrigger_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetWidth_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetHeight_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetBytesPerPixel_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetBytesPerPixel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_ReadRegister_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long address,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_ReadRegister_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_WriteRegister_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long address,
    /* [in] */ long Val);


void __RPC_STUB IAVTActiveCam_WriteRegister_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetCameraList_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetCameraList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetModeList_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetModeList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetRateList_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetRateList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetTriggerList_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetTriggerList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_StartCapture_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ BSTR filepath,
    /* [defaultvalue][optional][in] */ float delay);


void __RPC_STUB IAVTActiveCam_StartCapture_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_StopCapture_Proxy( 
    IAVTActiveCam __RPC_FAR * This);


void __RPC_STUB IAVTActiveCam_StopCapture_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_LoadChannel_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ SHORT channel);


void __RPC_STUB IAVTActiveCam_LoadChannel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_SaveChannel_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ SHORT channel);


void __RPC_STUB IAVTActiveCam_SaveChannel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetMaxChannel_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pChannel);


void __RPC_STUB IAVTActiveCam_GetMaxChannel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetIsoSpeed_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pSpeed);


void __RPC_STUB IAVTActiveCam_GetIsoSpeed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_SetIsoSpeed_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short value);


void __RPC_STUB IAVTActiveCam_SetIsoSpeed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_ShowProperties_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL CameraList);


void __RPC_STUB IAVTActiveCam_ShowProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetDIB_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *hDIB);


void __RPC_STUB IAVTActiveCam_GetDIB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_SetImageWindow_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ SHORT x,
    /* [in] */ SHORT y,
    /* [in] */ VARIANT value);


void __RPC_STUB IAVTActiveCam_SetImageWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetTriggerInfo_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetTriggerInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetF7Info_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetF7Info_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetPicture_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ IPictureDisp __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IAVTActiveCam_GetPicture_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_Draw_Proxy( 
    IAVTActiveCam __RPC_FAR * This);


void __RPC_STUB IAVTActiveCam_Draw_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_OverlayRectangle_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short StartX,
    /* [in] */ short StartY,
    /* [in] */ short EndX,
    /* [in] */ short EndY,
    /* [defaultvalue][optional][in] */ short Width);


void __RPC_STUB IAVTActiveCam_OverlayRectangle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_OverlayEllipse_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short StartX,
    /* [in] */ short StartY,
    /* [in] */ short EndX,
    /* [in] */ short EndY,
    /* [defaultvalue][optional][in] */ short Width);


void __RPC_STUB IAVTActiveCam_OverlayEllipse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_OverlayLine_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short StartX,
    /* [in] */ short StartY,
    /* [in] */ short EndX,
    /* [in] */ short EndY,
    /* [defaultvalue][optional][in] */ short Width);


void __RPC_STUB IAVTActiveCam_OverlayLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_OverlayText_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short X,
    /* [in] */ short Y,
    /* [in] */ BSTR str);


void __RPC_STUB IAVTActiveCam_OverlayText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_OverlayClear_Proxy( 
    IAVTActiveCam __RPC_FAR * This);


void __RPC_STUB IAVTActiveCam_OverlayClear_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_OverlayPixel_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short X,
    /* [in] */ short Y);


void __RPC_STUB IAVTActiveCam_OverlayPixel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetItemList_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long item,
    /* [out] */ long __RPC_FAR *count,
    /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *list);


void __RPC_STUB IAVTActiveCam_GetItemList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_IsControls_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *list);


void __RPC_STUB IAVTActiveCam_IsControls_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_ReadPIO_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_ReadPIO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_WritePIO_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long value);


void __RPC_STUB IAVTActiveCam_WritePIO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_SetSIO_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [defaultvalue][optional][in] */ BSTR settings);


void __RPC_STUB IAVTActiveCam_SetSIO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_ReadSIO_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [defaultvalue][optional][in] */ long count,
    long timeout,
    /* [retval][out] */ BSTR __RPC_FAR *str);


void __RPC_STUB IAVTActiveCam_ReadSIO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_WriteSIO_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ BSTR str,
    /* [defaultvalue][optional][in] */ long timeout);


void __RPC_STUB IAVTActiveCam_WriteSIO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_SetStrobe_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short index,
    /* [in] */ long duration,
    /* [in] */ long delay,
    /* [in] */ VARIANT_BOOL polarity);


void __RPC_STUB IAVTActiveCam_SetStrobe_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_LoadSettings_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ BSTR profile);


void __RPC_STUB IAVTActiveCam_LoadSettings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_SaveSettings_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ BSTR profile);


void __RPC_STUB IAVTActiveCam_SaveSettings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_DrawText_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short X,
    /* [in] */ short Y,
    /* [in] */ BSTR str,
    /* [in] */ long red,
    /* [defaultvalue][optional][in] */ long green,
    /* [defaultvalue][optional][in] */ long blue,
    /* [defaultvalue][optional][in] */ SHORT opacity);


void __RPC_STUB IAVTActiveCam_DrawText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_DrawLine_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short X1,
    /* [in] */ short Y1,
    /* [in] */ short X2,
    /* [in] */ short Y2,
    /* [in] */ short width,
    /* [in] */ long red,
    /* [defaultvalue][optional][in] */ long green,
    /* [defaultvalue][optional][in] */ long blue,
    /* [defaultvalue][optional][in] */ short opacity);


void __RPC_STUB IAVTActiveCam_DrawLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_DrawRectangle_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short X1,
    /* [in] */ short Y1,
    /* [in] */ short X2,
    /* [in] */ short Y2,
    /* [in] */ short width,
    /* [in] */ long red,
    /* [defaultvalue][optional][in] */ long green,
    /* [defaultvalue][optional][in] */ long blue,
    /* [defaultvalue][optional][in] */ short opacity);


void __RPC_STUB IAVTActiveCam_DrawRectangle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_DrawEllipse_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short X1,
    /* [in] */ short Y1,
    /* [in] */ short X2,
    /* [in] */ short Y2,
    /* [in] */ short width,
    /* [in] */ long red,
    /* [defaultvalue][optional][in] */ long green,
    /* [defaultvalue][optional][in] */ long blue,
    /* [defaultvalue][optional][in] */ short opacity);


void __RPC_STUB IAVTActiveCam_DrawEllipse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_DrawPixel_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short X,
    /* [in] */ short Y,
    /* [in] */ long red,
    /* [defaultvalue][optional][in] */ long green,
    /* [defaultvalue][optional][in] */ long blue,
    /* [defaultvalue][optional][in] */ SHORT opacity);


void __RPC_STUB IAVTActiveCam_DrawPixel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetHistogram_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short component,
    /* [defaultvalue][optional][in] */ long bins,
    /* [defaultvalue][optional][in] */ short step,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetHistogram_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetImageStat_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short component,
    /* [defaultvalue][optional][in] */ short step,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetImageStat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_SetROI_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ long X1,
    /* [in] */ long Y1,
    /* [in] */ long X2,
    /* [in] */ long Y2,
    /* [defaultvalue][optional][in] */ long l1,
    /* [defaultvalue][optional][in] */ long l2);


void __RPC_STUB IAVTActiveCam_SetROI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_GetROI_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *value);


void __RPC_STUB IAVTActiveCam_GetROI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAVTActiveCam_SaveBkg_Proxy( 
    IAVTActiveCam __RPC_FAR * This,
    /* [in] */ short mode,
    /* [defaultvalue][optional][in] */ short frames);


void __RPC_STUB IAVTActiveCam_SaveBkg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAVTActiveCam_INTERFACE_DEFINED__ */


#ifndef ___IAVTActiveCamEvents_DISPINTERFACE_DEFINED__
#define ___IAVTActiveCamEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IAVTActiveCamEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IAVTActiveCamEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D555F793-E1F4-4E25-933A-294F6788DC0F")
    _IAVTActiveCamEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IAVTActiveCamEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _IAVTActiveCamEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _IAVTActiveCamEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _IAVTActiveCamEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _IAVTActiveCamEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _IAVTActiveCamEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _IAVTActiveCamEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _IAVTActiveCamEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } _IAVTActiveCamEventsVtbl;

    interface _IAVTActiveCamEvents
    {
        CONST_VTBL struct _IAVTActiveCamEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IAVTActiveCamEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IAVTActiveCamEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IAVTActiveCamEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IAVTActiveCamEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IAVTActiveCamEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IAVTActiveCamEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IAVTActiveCamEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IAVTActiveCamEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_AVTActiveCam;

#ifdef __cplusplus

class DECLSPEC_UUID("6E4688A6-4FCE-40FF-9D27-8AC603C7BF43")
AVTActiveCam;
#endif

EXTERN_C const CLSID CLSID_VideoSource;

#ifdef __cplusplus

class DECLSPEC_UUID("6DF116B3-E69A-45A5-B38A-55D38417E268")
VideoSource;
#endif

EXTERN_C const CLSID CLSID_VideoFormat;

#ifdef __cplusplus

class DECLSPEC_UUID("9B725410-FC06-4694-9948-B92ADD18A687")
VideoFormat;
#endif

EXTERN_C const CLSID CLSID_VideoExposure;

#ifdef __cplusplus

class DECLSPEC_UUID("9865E090-2A8D-4704-97CE-751AB51AB688")
VideoExposure;
#endif

EXTERN_C const CLSID CLSID_VideoColor;

#ifdef __cplusplus

class DECLSPEC_UUID("44291247-6CC6-4F1D-BE16-FBB6EFD617A0")
VideoColor;
#endif

EXTERN_C const CLSID CLSID_VideoDisplay;

#ifdef __cplusplus

class DECLSPEC_UUID("6f0be2ac-5a5c-4d1b-8cea-a454d8851b00")
VideoDisplay;
#endif

EXTERN_C const CLSID CLSID_VideoAdvanced;

#ifdef __cplusplus

class DECLSPEC_UUID("E8EDC1C7-3907-488F-AB2F-0D543D4DAD48")
VideoAdvanced;
#endif
#endif /* __AVTACTIVECAMLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
