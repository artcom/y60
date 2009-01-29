
set(SOURCES
JSParticleSystem.cpp
JSParticlesPlugin.cpp
ParticleSystem.cpp
)

set(HEADERS
JSParticleSystem.h
ParticleSystem.h
y60_particles_settings.h
)

y60_add_component(
    GPUParticles
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS SPIDERMONKEY GLEW OPENEXR
)

