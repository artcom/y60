// Wiim API ©2006 Eric B.
// http://digitalretrograde.com/projects/wiim/

// May be used and modified freely as long as this message is left intact

#ifndef __WIIMOTE_UTILS__
#define __WIIMOTE_UTILS__

#include <asl/Vector234.h>

#include <iostream>
#include <string>

#define DEG_TO_RAD(x) (3.1415926 / 180.0 * x)
/*
struct P2d
{
	double x, y;

	P2d() : x(0), y(0) {}
	P2d(double ix, double iy) : x(ix), y(iy) {}
	
	void rotate(const double angle_rad)
	{
		double tempx = x;
		x = cos(angle_rad) * x - sin(angle_rad) * y;
		y = cos(angle_rad) * y + sin(angle_rad) * tempx;
	}

	void offset(const double x, const double y)
	{
		this->x += x; this->y += y;
	}

	void operator +=(P2d & o) { x += o.x; y += o.y; }
	P2d operator +(P2d & o) { P2d ret(this->x + o.x, this->y + o.y); return ret; }
};

struct P2d_line
{
	P2d p1, p2;

	bool is_ok() { return p1.x < p2.x && p1.y < p2.y; }

	P2d_line() {}
	P2d_line(const P2d ip1, const P2d ip2) : p1(ip1), p2(ip2) {}
	P2d_line(const double p1x, const double p1y, const double p2x, const double p2y) : p1(P2d(p1x, p1y)), p2(P2d(p2x, p2y)) {}

	// alienryderflex.com/intersect
	static bool line_segment_intersection(
		P2d_line & line1,
		P2d_line & line2) 
	{
		return line_segment_intersection(line1, line2, NULL);
	}
	static bool line_segment_intersection(
		P2d_line & line1,
		P2d_line & line2,
		P2d * point) 
	{
		double Ax = line1.p1.x, Ay = line1.p1.y;
		double Bx = line1.p2.x, By = line1.p2.y;
		double Cx = line2.p1.x, Cy = line2.p1.y;
		double Dx = line2.p2.x, Dy = line2.p2.y;

		double  distAB, theCos, theSin, newX, ABpos ;

		//  Fail if either line segment is zero-length.
		if (Ax==Bx && Ay==By || Cx==Dx && Cy==Dy) return false;

		//  (1) Translate the system so that point A is on the origin.
		Bx-=Ax; By-=Ay;
		Cx-=Ax; Cy-=Ay;
		Dx-=Ax; Dy-=Ay;

		//  Discover the length of segment A-B.
		distAB=sqrt(Bx*Bx+By*By);

		//  (2) Rotate the system so that point B is on the positive X axis.
		theCos=Bx/distAB;
		theSin=By/distAB;
		newX=Cx*theCos+Cy*theSin;
		Cy  =Cy*theCos-Cx*theSin; Cx=newX;
		newX=Dx*theCos+Dy*theSin;
		Dy  =Dy*theCos-Dx*theSin; Dx=newX;

		//  Fail if segment C-D doesn't cross line A-B.
		if (Cy<0. && Dy<0. || Cy>=0. && Dy>=0.) return false;

		//  (3) Discover the position of the intersection point along line A-B.
		ABpos=Dx+(Cx-Dx)*Dy/(Dy-Cy);

		//  Fail if segment C-D crosses line A-B outside of segment A-B.
		if (ABpos<0. || ABpos>distAB) return false;

		//  (4) Apply the discovered position to line A-B in the original coordinate system.
		if (!!point)
		{
			point->x=Ax+ABpos*theCos;
			point->y=Ay+ABpos*theSin;
		}

		return true; 
	}

	static bool boxes_collide(P2d_line & b1, P2d_line & b2)
	{
		if (b1.p2.y < b2.p1.y) return false;
		if (b1.p1.y > b2.p2.y) return false;
		if (b1.p2.x < b2.p1.x) return false;
		if (b1.p1.x > b2.p2.x) return false;

		return true;
	}
};

typedef P2d V2d;
*/

namespace y60 {

class MotionData {
public:
    char x, y, z;
    
    MotionData() : x(0), y(0), z(0) {}
    MotionData(unsigned char x, unsigned char y, unsigned char z) {
        this->x = x + 128;
        this->y = y + 128;
        this->z = z + 128;
    }
};


class Button {
protected:
    std::string	_myName;
    int		_myCode;
    bool	_myState;
    bool _myChangedFlag;
    
public:
    Button(std::string name, int code) : _myName(name), _myCode(code), _myState(false), _myChangedFlag( false) {};
    std::string	getName() const { return _myName; }
    int		getCode() const { return _myCode; }
    bool	pressed() const { return _myState; }
    bool  hasChanged()  { bool myFlag = _myChangedFlag; _myChangedFlag = false; return myFlag; }
    
    void setState(const bool set) { _myState = set; }
    bool setCode(const int code) {
        
        bool myNewState  = (bool)(code & _myCode);
        
//        if (myNewState) {
//            std::cerr << "==== PRESSED: " << _myName << std::endl;
//        }
        bool released = _myState && ! myNewState;
//        if ( released ) {
//            std::cerr << "==== RELEASED: " << _myName << std::endl;
//        }

        _myChangedFlag = _myState != myNewState;
        
        _myState = myNewState;


        //       if ( _myChangedFlag) {
        //    std::cerr << "====== CHANGED :" << _myName << std::endl;
        //}

        return _myChangedFlag;
    }
};


}



class Util
{
public:
	static int GetInt2(const unsigned char * report, int offset)
	{
		int ret = 0;

		ret = report[offset + 0];
		ret <<= 8;
		ret |= report[offset + 1];

		return ret;
	}

	static int GetInt4(const unsigned char * report, int offset)
	{
		int ret = 0;

		ret = report[offset + 0];
		ret <<= 24;
		ret |= report[offset + 1];
		ret <<= 16;
		ret |= report[offset + 2];
		ret <<= 8;
		ret |= report[offset + 3];

		return ret;
	}
};

enum WiiEventType {
    WII_BUTTON,
    WII_MOTION,
    WII_INFRARED
};

class WiiEvent {
    public:
    // Ctor for button events
    WiiEvent(unsigned theID, const std::string & theName, bool thePressedFlag) :
        id( theID ),
        type( WII_BUTTON ),
        buttonname( theName ),
        pressed( thePressedFlag ) {}

    // Ctor for motion events
    WiiEvent(unsigned theID, const asl::Vector3f & theAcceleration) :
        id( theID ),
        type( WII_MOTION ),
        acceleration( theAcceleration ) {}

    // Ctor for ir data
    WiiEvent(unsigned theID, const asl::Vector2i theIRData[4], const asl::Vector2f & theScreenPosition ) :
        id( theID ),
        type( WII_INFRARED ),
        screenPosition( theScreenPosition )
    {
        for (unsigned i = 0; i < 4; ++i) {
            irPositions[i] = theIRData[i];
        }
    }

    unsigned id;

    WiiEventType type;
    // Button data
    std::string buttonname;
    bool        pressed;

    // motion data
    asl::Vector3f acceleration;

    // infrared
    asl::Vector2i irPositions[4];
    asl::Vector2f screenPosition;
};

#endif // __WIIMOTE_UTILS__
