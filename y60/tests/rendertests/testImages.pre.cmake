
macro(testImages_pre BUILDPATH WORKPATH GENMOVIE_EXECUTABLE GENCOMPRESSEDTEX_EXECUTABLE)
  set(TESTCUBEMAPDIR_B ${BUILDPATH}/testmodels/tex/ENVMAP)
  set(TESTCUBEMAPDIR_S ${WORKPATH}/tex/ENVMAP)
  
  message(" create ${TESTCUBEMAPDIR_B}")
  if(NOT EXISTS ${TESTCUBEMAPDIR_B})
    file(MAKE_DIRECTORY ${TESTCUBEMAPDIR_B})
  endif(NOT EXISTS ${TESTCUBEMAPDIR_B})

  # generate layered i60 cubemap
  set(BACK "pano_backcam.0000.jpg")
  set(DOWN "pano_downcam.0000.jpg")
  set(FRONT "pano_frontcam.0000.jpg")
  set(LEFT "pano_leftcam.0000.jpg")
  set(RIGHT "pano_rightcam.0000.jpg")
  set(UP "pano_upcam.0000.jpg")
  
  execute_process(
    COMMAND ${GENCOMPRESSEDTEX_EXECUTABLE} --outfile ${TESTCUBEMAPDIR_B}/layered_cubemap.i60 --compression S3TC_DXT1 --layer 6 ${TESTCUBEMAPDIR_S}/${FRONT} ${TESTCUBEMAPDIR_S}/${RIGHT} ${TESTCUBEMAPDIR_S}/${BACK} ${TESTCUBEMAPDIR_S}/${LEFT} ${TESTCUBEMAPDIR_S}/${UP} ${TESTCUBEMAPDIR_S}/${DOWN}
    WORKING_DIRECTORY ${WORKPATH}
  )
endmacro(testImages_pre)
