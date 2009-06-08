
# NOTE: order is significant here with
#       regard to which tests will have
#       paths available for what plugins

add_subdirectory(ProcFunctions)
add_subdirectory(Cairo)
add_subdirectory(EdgeBlender)
add_subdirectory(EventLoop)
add_subdirectory(GLBinding)
add_subdirectory(GlurFilter)
add_subdirectory(GPUParticles)
add_subdirectory(JSSound)
add_subdirectory(Nagios)
add_subdirectory(OpenEXR)
add_subdirectory(SynergyServer)
add_subdirectory(testRendererExtension)
add_subdirectory(WindowCW)

add_subdirectory(codecs)
add_subdirectory(example)
add_subdirectory(formats)
add_subdirectory(input)
add_subdirectory(networking)
add_subdirectory(video)

option(Y60_APE_EXPERIMENT "Enable experimental *monkey wrappers" NO)
if(Y60_APE_EXPERIMENT)
    add_subdirectory(yape)
endif(Y60_APE_EXPERIMENT)

if(LINUX)
    add_subdirectory(DVB)
    add_subdirectory(SimWater)
endif(LINUX)

