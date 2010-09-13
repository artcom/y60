#ifndef NETSRC_TEXTUREATLAS_SETTING_INCLUDED
#define NETSRC_TEXTUREATLAS_SETTING_INCLUDED

#if defined(_WIN32)
#   define AC_DLL_EXPORT __declspec( dllexport )
#   define AC_DLL_IMPORT __declspec( dllimport )
#else
#   define AC_DLL_EXPORT
#   define AC_DLL_IMPORT
#endif


#ifdef textureatlas_EXPORTS
#   define NETSRC_TEXTUREATLAS_EXPORT AC_DLL_EXPORT
#else
#   define NETSRC_TEXTUREATLAS_EXPORT AC_DLL_IMPORT
#endif

#endif // NETSRC_TEXTUREATLAS_SETTING_INCLUDED

