
set(SOURCE
CoordSpline.cpp
Frustum.cpp
GeometryUtils.cpp
Matrix4.cpp
PerlinNoise.cpp
Spline.cpp
Statistic.cpp
SvgPath.cpp
VectorManipulator.cpp
)

set(HEADERS
Box.h
BSpline.h
Capsule.h
contact.h
CoordSpline.h
FixedVector.h
Frustum.h
GeometryUtils.h
integrate_function.h
intersection.h
linearAlgebra.h
Line.h
Matrix4Base.h
Matrix4.h
numeric_functions.h
ODESolver.h
PerlinNoise.h
Plane.h
Point234.h
Quaternion.h
Sphere.h
Spline.h
Statistic.h
SvgPath.h
Triangle.h
Vector234.h
VectorManipulator.h
)

set(TESTS
Box
BSpline
Contact
FixedVector
Frustum
GeometryUtils
Intersection
LinearAlgebra
Matrix4Base
Matrix4
NumericFunctions
Quaternion
Spline
Statistic
SvgPath
VectorStream
)

add_library(aslmath SHARED ${SOURCE})
set_target_properties(aslmath PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(aslmath aslbase)

install(
  TARGETS aslmath
  LIBRARY
    DESTINATION lib
  PUBLIC_HEADER 
    DESTINATION include/asl/math
)

foreach(test ${TESTS})
  add_executable(test${test} test${test}.tst.cpp)
  target_link_libraries(test${test} aslmath)
  add_test(${test} test${test})
endforeach(test)

