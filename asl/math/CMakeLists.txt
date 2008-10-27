
set(SOURCES
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

ac_add_library(
    aslmath
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase
    TESTS   ${TESTS}
)
