#include "Frustum.h"

#include "numeric_functions.h"
#include "intersection.h"
#include <iostream>

using namespace std;

#define DB(x) // x
namespace asl { 

Frustum::Frustum() :
    _myLeft(0),
    _myRight(0),
    _myTop(0),
    _myBottom(0),
    _myNear(0),
    _myFar(0),
    _myProjectionType(PERSPECTIVE)
{
}

Frustum::Frustum(double theLeft, double theRight,
        double theBottom, double theTop,
        double theNear, double theFar, ProjectionType theProjectionType) :
    _myLeft(theLeft),
    _myRight(theRight),
    _myBottom(theBottom),
    _myTop(theTop),
    _myNear(theNear),
    _myFar(theFar),
    _myProjectionType(theProjectionType)
{
}

Frustum::Frustum(double theX, double theY, double theNear, double theFar, ProjectionType theProjectionType) {
    if (theProjectionType == PERSPECTIVE) {
        setSymmetricPerspective(theX, theY, theNear, theFar);
    } else {
        setSymmetricOrtho(theX, theY, theNear, theFar);
    }
}

Frustum::~Frustum() {
}

void 
Frustum::updateCorners(float theNearPlane, float theFarPlane, float theHFov, float theOrthoWidth, float theAspectRatio) {
    if (theNearPlane == 0.0f) {
        theNearPlane = 0.1f;
    }
    if (theFarPlane == 0.0f) {
        theFarPlane = 1000.0f;
    }

    bool isPerpective = theHFov > 0.0f;
    bool isOrtho = theOrthoWidth > 0.0f;
    if (isPerpective && isOrtho) {
        throw FrustumException("Camera has hfov AND width attributes",
                PLUS_FILE_LINE);
    }
    if (!isPerpective && !isOrtho) {
        throw FrustumException("Camera has neither hfov NOR width attributes",
                PLUS_FILE_LINE);
    }

    if (isPerpective) {
        float myVFov = float(asl::degFromRad(2.0f *
                    atan( 1.0f / theAspectRatio * tan(asl::radFromDeg(theHFov) * 0.5f))));

        setSymmetricPerspective(theHFov, myVFov, theNearPlane, theFarPlane);
        DB(cerr << "Renderer::setPerspective() " << "nearplane: " << theNearPlane <<
                " farplane: " << theFarPlane << " fovy: " << myVFov << " ar: " <<
                theAspectRatio << endl);
    } else {
        float myHeight =  theOrthoWidth / theAspectRatio;
        setSymmetricOrtho(theOrthoWidth, myHeight, theNearPlane, theFarPlane);
        DB(cerr << "Renderer::setPerspective() " << "nearplane: " << theNearPlane <<
                " farplane: " << theFarPlane << " width: " << theOrthoWidth << " ar: " <<
                theAspectRatio << endl);
    }
}

void
Frustum::setSymmetricPerspective(double theHFov, double theVFov, double theNear, double theFar) {
    _myRight  = tan( radFromDeg( theHFov * 0.5 ) ) * theNear;
    _myLeft   = - _myRight;
    _myTop    = tan( radFromDeg( theVFov * 0.5 ) ) * theNear;
    _myBottom = - _myTop;
    _myNear   = theNear;
    _myFar    = theFar;
    _myProjectionType = PERSPECTIVE;
}

void
Frustum::setSymmetricOrtho(double theWidth, double theHeight, double theNear, double theFar) {
    _myRight  = theWidth/2;
    _myLeft   = - _myRight;
    _myTop    = theHeight/2;
    _myBottom = - _myTop;
    _myNear   = theNear;
    _myFar    = theFar;
    _myProjectionType = ORTHO;
}

// See OpenGL Red Book Appendix F page 674
// and errata @ www.woo.com/errata.html
bool
Frustum::getProjectionMatrix(asl::Matrix4f & theProjectionMatrix) const {
    if (_myProjectionType == PERSPECTIVE) {
        theProjectionMatrix.assign(float(2.0 * _myNear / (_myRight - _myLeft)), 0.0f, 0.0f, 0.0f,
                0.0f, float(2.0 * _myNear / (_myTop - _myBottom)), 0.0f, 0.0f,

                float((_myRight + _myLeft) / (_myRight - _myLeft)),
                float((_myTop + _myBottom) / (_myTop - _myBottom)),
                float(-(_myFar + _myNear) / (_myFar - _myNear)),
                -1.0f,

                0.0f, 0.0f, float(-2.0 * _myFar * _myNear / (_myFar - _myNear)), 0.0f);
        return true;
    } else if (_myProjectionType == ORTHO) {
        // corrected version - don't trust the red book, trust me! 
        theProjectionMatrix.assign(float(2.0 / (_myRight - _myLeft)), 0.0f, 0.0f, 0.0f,
                0.0f, float(2.0 / (_myTop - _myBottom)), 0.0f, 0.0f,
                0.0f, 0.0f, float(-2.0f/(_myFar - _myNear)), 0.0f,
                float(-(_myRight+_myLeft) / (_myRight-_myLeft)),
                float(-(_myTop+_myBottom) / (_myTop-_myBottom)),
                float(-(_myFar + _myNear) / (_myFar - _myNear)), 1.0f, Matrix4f::AFFINE);
        return true;
    }
    return false;
}

void
Frustum::updatePlanes(const Matrix4f & theCameraTransform, const Matrix4f & theCameraTransformI) {

    // TODO:: not rotation aware!
    Matrix4f myITCameraMatrix = theCameraTransformI;
    myITCameraMatrix.transpose();

    Vector4f myHViewVector = theCameraTransform.getRow(2);
    Vector3f myViewVector = -normalized(Vector3f(myHViewVector[0], myHViewVector[1], myHViewVector[2]));

    Vector4f myHUpVector = theCameraTransform.getRow(1);
    Vector3f myUpVector  = normalized(Vector3f(myHUpVector[0], myHUpVector[1], myHUpVector[2]));

    Vector4f myHRightVector = theCameraTransform.getRow(0);
    Vector3f myRightVector  = normalized(Vector3f(myHRightVector[0], myHRightVector[1], myHRightVector[2]));

    Point3f myEyePoint = theCameraTransform.getTranslation();

    Vector4f myHLeftBottomVector = Vector4f(float(_myLeft), float(_myBottom),
            float(- _myNear), 1.0f) * myITCameraMatrix;
    Vector3f myLeftBottomVector = Vector3f(myHLeftBottomVector[0], myHLeftBottomVector[1], myHLeftBottomVector[2]);
    Vector4f myHRightTopVector =   Vector4f(float(_myRight), float(_myTop), float(- _myNear), 1) * myITCameraMatrix;
    Vector3f myRightTopVector = Vector3f(myHRightTopVector[0], myHRightTopVector[1], myHRightTopVector[2]);

    Point3f myLeftBottomNearPoint = Point3f(float(_myLeft), float(_myBottom), float(- _myNear)) * theCameraTransform;
    Point3f myRightTopFarPoint = Point3f(float(_myRight), float(_myTop), float(- _myFar)) * theCameraTransform;

    if (_myProjectionType == PERSPECTIVE) {
        _myLeftPlane   = Plane<float>( normalized(cross(myLeftBottomVector, myUpVector)), myEyePoint);
        _myRightPlane  = Plane<float>( normalized(cross(myUpVector, myRightTopVector)),   myEyePoint);

        _myBottomPlane = Plane<float>( normalized(cross(myRightVector, myLeftBottomVector)), myEyePoint);
        _myTopPlane    = Plane<float>( normalized(cross(myRightTopVector, myRightVector)), myEyePoint);

        _myNearPlane   = Plane<float>(myViewVector, myEyePoint + _myNear * myViewVector);
        _myFarPlane    = Plane<float>(- 1.0 * myViewVector, myEyePoint + _myFar*myViewVector);
    } else {
        _myLeftPlane   = Plane<float>( myRightVector, myLeftBottomNearPoint);
        _myRightPlane  = Plane<float>(-myRightVector, myRightTopFarPoint);

        _myBottomPlane = Plane<float>( myUpVector, myLeftBottomNearPoint);
        _myTopPlane    = Plane<float>(-myUpVector, myRightTopFarPoint);

        _myNearPlane   = Plane<float>( myViewVector, myLeftBottomNearPoint);
        _myFarPlane    = Plane<float>(-myViewVector, myRightTopFarPoint);
    }

}

Frustum::ProjectionType
Frustum::getType() const {
    return _myProjectionType;
}

const Plane<float> &
Frustum::getLeftPlane() const {
    return _myLeftPlane;
}

const Plane<float> &
Frustum::getRightPlane() const {
    return _myRightPlane;
}

const Plane<float> &
Frustum::getTopPlane() const {
    return _myTopPlane;
}

const Plane<float> &
Frustum::getBottomPlane() const {
    return _myBottomPlane;
}

const Plane<float> &
Frustum::getNearPlane() const {
    return _myNearPlane;
}

const Plane<float> &
Frustum::getFarPlane() const {
    return _myFarPlane;
}

double
Frustum::getLeft() const {
    return _myLeft;
}
double
Frustum::getRight() const {
    return _myRight;
}
double
Frustum::getTop() const {
    return _myTop;
}
double
Frustum::getBottom() const {
    return _myBottom;
}
double
Frustum::getNear() const {
    return _myNear;
}
double
Frustum::getFar() const {
    return _myFar;
}


bool
intersection(const asl::Box3f & theBox, const Frustum & theFrustum) {
    bool myDummy;
    return intersection(theBox, theFrustum, myDummy);
}

bool
intersection(const asl::Box3f & theBox, const Frustum & theFrustum, bool & theOverlapFlag) {
    theOverlapFlag = false;

    int myLeftPlaneHalfSpace = 1;
    DB(cerr << "l.normal = " << theFrustum.getLeftPlane().normal
            << " offset = " << theFrustum.getLeftPlane().offset << endl);
    DB(cerr << "box = " << theBox << endl);
    intersection(theBox, theFrustum.getLeftPlane(), myLeftPlaneHalfSpace);
    if (myLeftPlaneHalfSpace == -1) {
        DB(cerr << "===========> culled left" << endl;);
        return false;
    }

    int myRightPlaneHalfSpace;
    intersection(theBox, theFrustum.getRightPlane(), myRightPlaneHalfSpace);
    if (myRightPlaneHalfSpace == -1) {
        DB(cerr << "===========> culled right" << endl;);
        return false;
    }

    int myBottomPlaneHalfSpace;
    intersection(theBox, theFrustum.getBottomPlane(), myBottomPlaneHalfSpace);
    if (myBottomPlaneHalfSpace == -1) {
        DB(cerr << "===========> culled bottom" << endl;);
        return false;
    }

    int myTopPlaneHalfSpace;
    intersection(theBox, theFrustum.getTopPlane(), myTopPlaneHalfSpace);
    if (myTopPlaneHalfSpace == -1) {
        DB(cerr << "===========> culled top" << endl;);
        return false;
    }

    int myNearPlaneHalfSpace;
    intersection(theBox, theFrustum.getNearPlane(), myNearPlaneHalfSpace);
    if (myNearPlaneHalfSpace == -1) {
        DB(cerr << "===========> culled near" << endl);
        return false;
    }

    int myFarPlaneHalfSpace;
    intersection(theBox, theFrustum.getFarPlane(), myFarPlaneHalfSpace);
    if (myFarPlaneHalfSpace == -1) {
        DB(cerr << "===========> culled far" << endl);
        return false;
    }

    if (myLeftPlaneHalfSpace   == 0 ||
        myRightPlaneHalfSpace  == 0 ||
        myTopPlaneHalfSpace    == 0 ||
        myBottomPlaneHalfSpace == 0 ||
        myNearPlaneHalfSpace   == 0 ||
        myFarPlaneHalfSpace    == 0
      )
    {
        theOverlapFlag = true;
    }

    return true;

}

void
Frustum::getCorners(asl::Point3f & theLTF, asl::Point3f & theRBF,
                    asl::Point3f & theRTF, asl::Point3f & theLBF,
                    asl::Point3f & theLTBK, asl::Point3f & theRBBK,
                    asl::Point3f & theRTBK, asl::Point3f & theLBBK) const
{
    intersection(_myLeftPlane,  _myTopPlane,    _myNearPlane, theLTF);
    intersection(_myRightPlane, _myBottomPlane, _myNearPlane, theRBF);
    intersection(_myRightPlane, _myTopPlane,    _myNearPlane, theRTF);
    intersection(_myLeftPlane,  _myBottomPlane, _myNearPlane, theLBF);
    intersection(_myLeftPlane,  _myTopPlane,    _myFarPlane, theLTBK);
    intersection(_myRightPlane, _myBottomPlane, _myFarPlane, theRBBK);
    intersection(_myRightPlane, _myTopPlane,    _myFarPlane, theRTBK);
    intersection(_myLeftPlane,  _myBottomPlane, _myFarPlane, theLBBK);
}

std::ostream & operator << (std::ostream & os, const Frustum & theFrustum) {
    return os << "[" << (theFrustum.getType()==Frustum::PERSPECTIVE ? "PERSP:" : "ORTHO:") <<
                 "," << theFrustum.getLeft() <<
                 "," << theFrustum.getRight() <<
                 "," << theFrustum.getTop() <<
                 "," << theFrustum.getBottom() <<
                 "," << theFrustum.getNear() <<
                 "," << theFrustum.getFar() << "]";
}

std::istream & operator>>(std::istream & is, Frustum & theFrustum) {
    throw asl::Exception("Frustum istream operator not yet implemented", PLUS_FILE_LINE);
}


} // end of namespace asl
