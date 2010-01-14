// Wiim API ©2006 Eric B.
// http://digitalretrograde.com/projects/wiim/

// May be used and modified freely as long as this message is left intact

#ifndef __WIIMOTE_UTILS__
#define __WIIMOTE_UTILS__

#include <asl/math/Vector234.h>

#include <iostream>
#include <string>

#define DEG_TO_RAD(x) (3.1415926 / 180.0 * x)

namespace y60 {

DEFINE_EXCEPTION( WiiException, asl::Exception );


// TODO: replace with asl::Vector3 ...
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

#endif // __WIIMOTE_UTILS__
