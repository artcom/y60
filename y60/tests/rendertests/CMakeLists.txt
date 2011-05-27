
set(CG_TESTS
cubemap
depthtest
emissive
gelenk_ani
glowcube
lambert_blinn_phong
material_test
multiLightEnvmap
multiscene
multiTexturing
normalMap
relief
testEnv
testLights
testNightvision
testParticles
testSpotlight
testSprites
testVertexShaderOnly
)

set(NOCG_TESTS
BoolAnim
color_bias_offset
copyBufferToImage
cube
cube_ortho
cubemap
curve
depthtest
envmap
fogtest
gelenk
gelenk_ani
material_test
multiLightEnvmap
multiTexturing
multi_texture_modes
multiscene
multiview
negativeScale
npot
offscreenBuffer
projector
stlcodec
test16Bit
testAnimationClip
testCheckers
testCullFace
testEnv
testImageUpload
testInclude
testJoint
testLayeredTextureOpt
testLights
testMaterial
testMovies
testOffScreenBackBuffer
testOffScreenFramebuffer
testOffscreenRendering
testPixelTransfer
testRaster
testSprites
testSvgPath
testtripletex
testBillBoard
tex_nearest
transparency_test
uvrepeat
# disabled textrender test, due to flakiness and the lack of fonts
#coloredText
#testText
#testImages
#testZip
)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/TestImages)

function(add_rendertest TEST SUFFIX SHADERLIBRARY)
  set(LOAD)

  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}.js)
      list(APPEND LOAD ${TEST}.js)
  else(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}.js)
      list(APPEND LOAD scenetest.js)
  endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}.js)

  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}.x60)
      list(APPEND LOAD ${TEST}.x60)
  endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}.x60)

  list(APPEND LOAD ${SHADERLIBRARY})

  y60_add_rendertest(
      ${TEST}_${SUFFIX}
      LOAD ${LOAD}

      OUTPUT_DIR   ${CMAKE_CURRENT_BINARY_DIR}/TestImages
      BASELINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/BaselineImages

      IMAGE_PREFIX ${TEST}
      IMAGE_SUFFIX ${SUFFIX}
  )
endfunction(add_rendertest)

foreach(TEST ${CG_TESTS})
  add_rendertest(${TEST} cg shaderlibrary.xml)
endforeach(TEST)

foreach(TEST ${NOCG_TESTS})
  add_rendertest(${TEST} nocg shaderlibrary_nocg.xml)
endforeach(TEST)

#y60_add_scenetest_custom(testText testmodels 1.5 30)

