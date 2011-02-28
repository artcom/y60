
set(TESTMOVIE "testmovie")
  
macro(generateMovie BUILDPATH BINWORKPATH GENMOVIE_EXECUTABLE IMAGETYPE ENCODING)
  set(FILENAME ${TESTMOVIE}_${IMAGETYPE}_${ENCODING}.m60)

  if(EXISTS ${BINWORKPATH}/${FILENAME})
    file(REMOVE ${BINWORKPATH}/${FILENAME})
  endif(EXISTS ${BINWORKPATH}/${FILENAME})
  
  execute_process(
    COMMAND  ${GENMOVIE_EXECUTABLE} --source-dir ${TESTMOVIE}_${IMAGETYPE} --outfile ${FILENAME} --encoding ${ENCODING} --resize pad
    WORKING_DIRECTORY ${BINWORKPATH}
  )
endmacro(generateMovie)

macro(testMovies_pre BUILDPATH WORKPATH GENMOVIE_EXECUTABLE GENCOMPRESSEDTEX_EXECUTABLE)

  set(WORKPATH_H ${WORKPATH}/movies)
  set(BINPATH ${BUILDPATH}/testmodels/movies)
  
  if(NOT EXISTS ${BINPATH})
    file(MAKE_DIRECTORY ${BINPATH})
  endif(NOT EXISTS ${BINPATH})  
  
  file(REMOVE_RECURSE ${BINPATH}/${TESTMOVIE}_i60)
  file(MAKE_DIRECTORY ${BINPATH}/${TESTMOVIE}_i60)

  #copy directory from src- to build-tree
  list(INSERT CMAKE_MODULE_PATH 0 ${WORKPATH}/../src/acmake/modules)
  include(AcFileUtils) 
  copy_dir(${WORKPATH_H}/${TESTMOVIE}_png ${BINPATH}/${TESTMOVIE}_png)

  execute_process(
    COMMAND ${GENCOMPRESSEDTEX_EXECUTABLE} --img-dir ${TESTMOVIE}_png --out-dir ${TESTMOVIE}_i60
    WORKING_DIRECTORY ${BINPATH}
  )
  
  generateMovie(${BUILDPATH} ${BINPATH} ${GENMOVIE_EXECUTABLE} i60 RLE)
  generateMovie(${BUILDPATH} ${BINPATH} ${GENMOVIE_EXECUTABLE} i60 DRLE)
  generateMovie(${BUILDPATH} ${BINPATH} ${GENMOVIE_EXECUTABLE} png RLE)
  generateMovie(${BUILDPATH} ${BINPATH} ${GENMOVIE_EXECUTABLE} png DRLE)
endmacro(testMovies_pre)

