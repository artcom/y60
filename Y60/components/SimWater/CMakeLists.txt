
set(SOURCES
SimWater.cpp
SimWaterPlugin.cpp
WaterRepresentation.cpp
WaterSimulation.cpp
)

set(HEADERS
AGPBufferAllocator.h
SimWater.h
WaterRepresentation.h
WaterSimulation.h
)

y60_add_component(
    SimWater
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
)

