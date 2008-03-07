#include "Frustum.h"

#include "numeric_functions.h"
#include "intersection.h"
#include <iostream>

using namespace std;

#define DB(x) // x

#ifdef verify
	#ifndef _SETTING_NO_UNDEF_WARNING_ 
		#warning Symbol 'verify' defined as macro, undefining. (Outrageous namespace pollution by Apples AssertMacros.h, revealing arrogance and incompetence)
	#endif
#undef verify
#endif

static const char * ProjectionTypeStrings[] = {
    "perspective",
    "orthonormal",
    ""
};
IMPLEMENT_ENUM( asl::ProjectionType, ProjectionTypeStrings);

static const char * ResizePolicyStrings[] = {
    "no_adaption",
    "adapt_vertical",
    "adapt_horizontal",
    ""
};
IMPLEMENT_ENUM( asl::ResizePolicy, ResizePolicyStrings);

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
    // default should be diffrent from zero ...
    setSymmetricPerspective(54.0f, 54.0f, 0.1f, 10000.0f);
}

Frustum::Frustum( const Frustum & otherFrustum ) :
    _myLeft( otherFrustum._myLeft),
    _myRight( otherFrustum._myRight),
    _myTop( otherFrustum._myTop),
    _myBottom( otherFrustum._myBottom),
    _myNear( otherFrustum._myNear),
    _myFar( otherFrustum._myFar),
    _myProjectionType( otherFrustum._myProjectionType),
    _myLeftPlane( otherFrustum._myLeftPlane),
    _myRightPlane( otherFrustum._myRightPlane),
    _myTopPlane( otherFrustum._myTopPlane),
    _myBottomPlane( otherFrustum._myBottomPlane),
    _myNearPlane( otherFrustum._myNearPlane),
    _myFarPlane( otherFrustum._myFarPlane)
{
}

Frustum::Frustum(float theLeft, float theRight,
        float theBottom, float theTop,
        float theNear, float theFar, ProjectionType theProjectionType) :
    _myLeft(theLeft),
    _myRight(theRight),
    _myBottom(theBottom),
    _myTop(theTop),
    _myNear(theNear),
    _myFar(theFar),
    _myProjectionType(theProjectionType)
{
}

Frustum::Frustum(float theX, float theY, float theNear, float theFar, ProjectionType theProjectionType) {
    if (theProjectionType == PERSPECTIVE) {
        setSymmetricPerspective(theX, theY, theNear, theFar);
    } else {
        setSymmetricOrtho(theX, theY, theNear, theFar);
    }
}

Frustum::~Frustum() {
}

const Frustum &
Frustum::operator = (const Frustum & other) {
    _myLeft = other._myLeft;
    _myRight = other._myRight;
    _myBottom = other._myBottom;
    _myTop = other._myTop;
    _myNear = other._myNear;
    _myFar = other._myFar;
    _myProjectionType = other._myProjectionType;

    _myLeftPlane = other._myLeftPlane;
    _myRightPlane = other._myRightPlane;
    _myTopPlane = other._myTopPlane;
    _myBottomPlane = other._myBottomPlane;
    _myNearPlane = other._myNearPlane;
    _myFarPlane = other._myFarPlane;

    return * this;
}

void 
Frustum::changeAspectRatio( ResizePolicy thePolicy, float theNewAspect) {
    // save values before members are modified
    float myOldWidth  = getWidth();
    float myOldHeight = getHeight();
    float myOldAspect = myOldWidth / myOldHeight;

    switch (thePolicy) {
        case ADAPT_VERTICAL:
            {
                float myNewHeight = myOldWidth / theNewAspect;
                float myScale = myNewHeight / myOldHeight;
                _myBottom *= myScale;
                _myTop *= myScale;
            }
            break;
        case ADAPT_HORIZONTAL:
            {
                float myNewWidth = myOldHeight * theNewAspect;
                float myScale = myNewWidth / myOldWidth;
                _myLeft *= myScale;
                _myRight *= myScale;
            }
            break;
        case NO_ADAPTION:
            break;
    };
}

void
Frustum::setSymmetricPerspective(float theHFov, float theVFov, float theNear, float theFar) {
    _myRight  = float( tan( radFromDeg( theHFov * 0.5f ) ) * theNear );
    _myLeft   = - _myRight;
    _myTop    = float( tan( radFromDeg( theVFov * 0.5f ) ) * theNear );
    _myBottom = - _myTop;
    _myNear   = theNear;
    _myFar    = theFar;
    _myProjectionType = PERSPECTIVE;
}

void
Frustum::setSymmetricOrtho(float theWidth, float theHeight, float theNear, float theFar) {
    _myRight  = theWidth/2;
    _myLeft   = - _myRight;
    _myTop    = theHeight/2;
    _myBottom = - _myTop;
    _myNear   = theNear;
    _myFar    = theFar;
    _myProjectionType = ORTHONORMAL;
}

void
Frustum::set(float theLeft, float theRight, float theBottom, float theTop, float theNear, float theFar) {
    _myLeft   = theLeft;
    _myRight  = theRight;
    _myBottom = theBottom;
    _myTop    = theTop;
    _myNear   = theNear;
    _myFar    = theFar;
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
    } else if (_myProjectionType == ORTHONORMAL) {
        // corrected version - don't trust the red book, trust me! 
        theProjectionMatrix.assign(float(2.0 / (_myRight - _myLeft)), 0.0f, 0.0f, 0.0f,
                0.0f, float(2.0 / (_myTop - _myBottom)), 0.0f, 0.0f,
                0.0f, 0.0f, float(-2.0f/(_myFar - _myNear)), 0.0f,
                float(-(_myRight+_myLeft) / (_myRight-_myLeft)),
                float(-(_myTop+_myBottom) / (_myTop-_myBottom)),
                float(-(_myFar + _myNear) / (_myFar - _myNear)), 1.0f, AFFINE);
        return true;
    }
    return false;
}

Matrix4f
Frustum::getProjectionMatrix() const {
    Matrix4f myMatrix;
    getProjectionMatrix( myMatrix );
    return myMatrix;
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

ProjectionType
Frustum::getType() const {
    return _myProjectionType;
}
void 
Frustum::setType( ProjectionType theProjection) {
    _myProjectionType = theProjection;
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

const float &
Frustum::getLeft() const {
    return _myLeft;
}
const float &
Frustum::getRight() const {
    return _myRight;
}
const float &
Frustum::getTop() const {
    return _myTop;
}
const float &
Frustum::getBottom() const {
    return _myBottom;
}
const float &
Frustum::getNear() const {
    return _myNear;
}
const float &
Frustum::getFar() const {
    return _myFar;
}

void
Frustum::setLeft(const float & theValue) {
     _myLeft = theValue;
}
void
Frustum::setRight(const float & theValue){
     _myRight = theValue;
}
void
Frustum::setTop(const float & theValue) {
     _myTop = theValue;
}
void
Frustum::setBottom(const float & theValue) {
     _myBottom = theValue;
}
void
Frustum::setNear(const float & theValue) {
     if (_myProjectionType == PERSPECTIVE) {
        // correct bounds to maintain the same field of view
        float myScale = theValue / _myNear;
        _myRight *= myScale;
        _myLeft *= myScale;
        _myTop *= myScale;
        _myBottom *= myScale;
     }
     _myNear = theValue;
}
void
Frustum::setFar(const float & theValue) {
     _myFar = theValue;
}

float
Frustum::getWidth() const {
    return _myRight - _myLeft;
}

void 
Frustum::setWidth( const float & theWidth) {
    float myShift = 0.0;
    if ( ! almostEqual( getWidth(), 0 )) {
        myShift = getHShift();
    }
    _myLeft = ( - myShift - 0.5f ) * theWidth;
    _myRight = ( - myShift + 0.5f) * theWidth;
}

float
Frustum::getHeight() const {
    return _myTop - _myBottom;
}

void 
Frustum::setHeight( const float & theHeight) {
    float myShift = 0.0;
    if ( ! almostEqual( getHeight(), 0 )) {
        myShift = getVShift();
    }
    _myBottom = ( - myShift - 0.5f) * theHeight;
    _myTop = ( - myShift + 0.5f) * theHeight;
}

float
Frustum::getHFov() const {
    if (_myProjectionType == PERSPECTIVE ) {
        float myHfov =  float( degFromRad( atan2( _myRight , _myNear ) ) -
                        degFromRad( atan2( _myLeft, _myNear )) ); 
        return myHfov;
    } else {
        throw FrustumException("Field of view requested on orthonormal frustum.",
                PLUS_FILE_LINE );
    }
}

void 
Frustum::setHFov(const float & theFOV ) {
    // save value before members are modified
    float myOldShift = getHShift();
    _myRight  = float( tan( radFromDeg( theFOV * 0.5f ) ) * _myNear );
    _myLeft   = - _myRight;
    setHShift( myOldShift );
    _myProjectionType = PERSPECTIVE;
}

float
Frustum::getVFov() const {
    if (_myProjectionType == PERSPECTIVE ) {
        float myVfov =  float( degFromRad( atan2( _myTop , _myNear ) ) -
                        degFromRad( atan2( _myBottom, _myNear )) );
        return myVfov;
    } else {
        throw FrustumException("Field of view requested on orthonormal frustum.",
                PLUS_FILE_LINE );
    }
}

void 
Frustum::setVFov(const float & theFOV ) {
    // save value before members are modified
    float myOldShift = getVShift();
    _myTop    = float( tan( radFromDeg( theFOV * 0.5f ) ) * _myNear );
    _myBottom = - _myTop;
    setVShift( myOldShift );
    _myProjectionType = PERSPECTIVE;
}



void 
Frustum::setHShift(const float & theShift) {
    float myWidth = getWidth();
    float myOffset = theShift * myWidth;
    _myRight = 0.5f * myWidth - myOffset;
    _myLeft = -0.5f * myWidth - myOffset;
}

float
Frustum::getHShift() const {
    if ( almostEqual( getWidth(), 0) ) {
        throw FrustumException("Can not compute horizontal shift. Frustum width is zero.",
                PLUS_FILE_LINE);
    }
    float myWidth = getWidth();
    return  - (_myLeft + 0.5f * myWidth) / myWidth;
}
void 
Frustum::setVShift(const float & theShift) {
    float myHeight = getHeight();
    float myOffset = theShift * myHeight;
    _myTop =  0.5f * myHeight - myOffset;
    _myBottom = - 0.5f * myHeight - myOffset;
}
float
Frustum::getVShift() const {
    if ( almostEqual( getHeight(), 0) ) {
        throw FrustumException("Can not compute vertical shift. Frustum height is zero.",
                PLUS_FILE_LINE);
    }
    return  - (_myBottom + 0.5f * getHeight()) / getHeight();
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


Frustum::TupleT 
Frustum::asTuple() const {
    TupleT myTuple;
    myTuple[0] = _myLeft;
    myTuple[1] = _myRight;
    myTuple[2] = _myBottom;
    myTuple[3] = _myTop;
    myTuple[4] = _myNear;
    myTuple[5] = _myFar;
    return myTuple;
}

void 
Frustum::fromTuple(const TupleT & theTuple) {
    _myLeft   = theTuple[0];
    _myRight  = theTuple[1];
    _myBottom = theTuple[2];
    _myTop    = theTuple[3];
    _myNear   = theTuple[4];
    _myFar    = theTuple[5];
}


std::ostream & operator << (std::ostream & os, const Frustum & theFrustum) {
    /*
    return os << "[" << theFrustum.getType() <<
                 "," << theFrustum.getLeft() <<
                 "," << theFrustum.getRight() <<
                 "," << theFrustum.getTop() <<
                 "," << theFrustum.getBottom() <<
                 "," << theFrustum.getNear() <<
                 "," << theFrustum.getFar() << "]";
                 */
    os << theFrustum.getType();
    os << theFrustum.asTuple();
    return os;
}

std::istream & operator>>(std::istream & is, Frustum & theFrustum) {
    //throw asl::Exception("Frustum istream operator not yet implemented", PLUS_FILE_LINE);
    ProjectionType myType;
    is >> myType;

    Frustum::TupleT myTuple;
    is >> myTuple;

    theFrustum.setType( myType );
    theFrustum.fromTuple( myTuple );
    return is;
}


} // end of namespace asl
