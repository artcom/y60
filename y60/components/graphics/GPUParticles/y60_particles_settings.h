#ifndef Y60_PARTICLES_SETTING_INCLUDED
#define Y60_PARTICLES_SETTING_INCLUDED

#include <asl/base/settings.h>

#ifdef GPUParticles_EXPORTS
#   define Y60_PARTICLES_DECL AC_DLL_EXPORT
#else
#   define Y60_PARTICLES_DECL AC_DLL_IMPORT
#endif

#endif // Y60_PARTICLES_SETTING_INCLUDED