

add_subdirectory(ASSDriver)
add_subdirectory(Cluster)
add_subdirectory(CMSCache)
add_subdirectory(DemoBehaviourRendex)
add_subdirectory(DemoModellingRendex)
add_subdirectory(DemoRendererExtension)
add_subdirectory(DSADriver)
add_subdirectory(EdgeBlender)
add_subdirectory(EventLoop)
add_subdirectory(FFMpegDecoder1)
add_subdirectory(FFMpegDecoder2)
add_subdirectory(GLBinding)
add_subdirectory(GlurFilter)
add_subdirectory(GPUParticles)
add_subdirectory(JSSound)
add_subdirectory(Nagios)
add_subdirectory(Net)
add_subdirectory(OpenEXR)
#add_subdirectory(OscReceiver) # XXX: ingo fix this later
#add_subdirectory(OscSender)   # XXX: ingo fix this later
add_subdirectory(ProcFunctions)
add_subdirectory(StlImporter)
add_subdirectory(SynergyServer)
add_subdirectory(testRendererExtension)
add_subdirectory(VideoProcessing)
add_subdirectory(WindowCW)
add_subdirectory(X3dImporter)



if(LINUX)
    add_subdirectory(DVB)
    #add_subdirectory(GStreamerCapture)
    add_subdirectory(PowermateInputExtension)
    add_subdirectory(SimWater)
    #add_subdirectory(Wiimote)
endif(LINUX)

if(WIN32)
#    add_subdirectory(DInput)
#    add_subdirectory(DInputFFB)
endif(WIN32)
