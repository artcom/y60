
include(${CMAKE_SOURCE_DIR}/netsrc/SDL_ttf2/SDLTTFDefinitions.cmake)
include(${CMAKE_SOURCE_DIR}/netsrc/SDL_ttf2/SDLTTFDependencies.cmake)

set(SDLTTF_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/netsrc/SDL_ttf2/")
set(SDLTTF_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/netsrc/SDL_ttf2/include")
set(SDLTTF_LIBRARIES SDL_ttf2)

