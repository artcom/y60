
# NOTE: order is significant here with
#       regard to which tests will have
#       paths available for what plugins

add_subdirectory(ProcFunctions)
add_subdirectory(ASSDriver)
add_subdirectory(Cairo)
add_subdirectory(Cluster)
add_subdirectory(CMSCache)
add_subdirectory(DemoBehaviourRendex)
add_subdirectory(DemoModellingRendex)
add_subdirectory(DemoRendererExtension)
add_subdirectory(DSADriver)
add_subdirectory(EdgeBlender)
add_subdirectory(EventLoop)
add_subdirectory(GLBinding)
add_subdirectory(GlurFilter)
add_subdirectory(GPUParticles)
add_subdirectory(JSSound)
add_subdirectory(Nagios)
add_subdirectory(Net)
add_subdirectory(OpenEXR)
add_subdirectory(OscReceiver)
add_subdirectory(OscSender)
add_subdirectory(StlImporter)
add_subdirectory(SynergyServer)
add_subdirectory(testRendererExtension)
add_subdirectory(VideoProcessing)
add_subdirectory(WindowCW)
add_subdirectory(X3dImporter)

add_subdirectory(FFMpegDecoder1)
add_subdirectory(FFMpegDecoder2)

option(Y60_APE_EXPERIMENT "Enable experimental *monkey wrappers" NO)
if(Y60_APE_EXPERIMENT)
    add_subdirectory(yape)
endif(Y60_APE_EXPERIMENT)

if(GSTREAMER_FOUND)
    add_subdirectory(GStreamerCapture)
endif(GSTREAMER_FOUND)


if(LINUX)
    add_subdirectory(DVB)
    add_subdirectory(PowermateInputExtension)
    add_subdirectory(SimWater)
    #add_subdirectory(Wiimote)
endif(LINUX)

if(WIN32)
    add_subdirectory(WMADecoder)
    add_subdirectory(WMVDecoder)
#    add_subdirectory(DInput)
#    add_subdirectory(DInputFFB)
endif(WIN32)
