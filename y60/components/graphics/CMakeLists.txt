
add_subdirectory(Cairo)
add_subdirectory(EdgeBlender)
add_subdirectory(GLBinding)
add_subdirectory(GlurFilter)
add_subdirectory(GPUParticles)
add_subdirectory(WindowCW)

if(LINUX)
    # build broken on windows. investigation indicated.
    add_subdirectory(SimWater)
endif(LINUX)

