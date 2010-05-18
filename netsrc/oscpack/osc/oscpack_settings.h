#ifndef NETSRC_OSCPACK_SETTING_INCLUDED
#define NETSRC_OSCPACK_SETTING_INCLUDED

#if defined(_WIN32)
#   pragma warning( disable:  4251)
#   define AC_DLL_EXPORT __declspec( dllexport )
#   define AC_DLL_IMPORT __declspec( dllimport )
#else
#   define AC_DLL_EXPORT
#   define AC_DLL_IMPORT
#endif

#if defined(oscpackosc_EXPORTS) || defined(oscpackip_EXPORTS)
#   define NETSRC_OSCPACK_EXPORT AC_DLL_EXPORT
#else
#   define NETSRC_OSCPACK_EXPORT AC_DLL_IMPORT
#endif

#endif // NETSRC_OSCPACK_SETTING_INCLUDED
