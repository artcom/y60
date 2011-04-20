/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
//   $Revision: 1.5 $
//
//      Author: Axel Kilian
//
// Description: Keyframe edit and animation toolset
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "CoordSpline.h"

#include "integrate_function.h"
#include "Matrix4.h"

#include <math.h>

using namespace std;  // automatically added!

namespace asl {

    static const int POSITION_X = 0; 
    static const int POSITION_Y = 1; 
    static const int POSITION_Z = 2;
    static const int TIME       = 3;

    CoordSpline::CoordSpline ( void )
    {
        _init_mode = catmull_rom;
        for ( int i=0; i<5; i++ ) {
            _spline.push_back ( new Hermite() );
        }
    }

    CoordSpline::CoordSpline ( HermiteInitMode initMode )
    {
        _init_mode = initMode;
        for ( int i=0; i<5; i++ ) {
            _spline.push_back ( new Hermite() );
        }
    }
    CoordSpline::~CoordSpline ( void )
    {

        for ( int i=0;i<5; i++ ) {
            delete _spline[i];
        }
    }

    void
        CoordSpline::print ( void )
    {
        // PRINT s(t) SPLINE PARAMETERS
        _spline[TIME]->print();
    }


    bool
        CoordSpline::init ( const vector<asl::QuaternionKeyframe>& keyframes, float& total_path, bool planet_mode )
    {
        dvector    time;    //
        dvector    s;    //  dvector is defined in "Spline.h":
        dvector    sPlanet;  //
        dvector    speed;    //  typedef vector<float> dvector
        vector< dvector >  value;    //
        vector<QuaternionKeyframe>::const_iterator  it;
        typedef std::vector<asl::Quaternionf> VectorOfQuaternionf;
        VectorOfQuaternionf keyFrameOrientations;

        _planet_mode    = planet_mode;
        _error_count    = 0;
        _error_maxnum   = 20;

        // DO WE WANT DEBUG-OUTPUT?
        bool debug = false;


        // GET DELTA TIME LIST
        for ( it=keyframes.begin(); it!=keyframes.end(); it++ ) {
            time.push_back ( (*it).getTime() );
        }
        //debug_print("time", time);
        // GET ABSOLUTE TIME LIST
        time [0] = 0.0;
        for ( dvector::size_type i=1; i<time.size(); i++ ) {
            time [i] = time [i] + time [i-1];
        }


        // FILL VALUE WITH ARBITRARY NUMBERS
        for ( dvector::size_type i=0; i<3; i++ ) {
            value.push_back ( time );
        }


        dvector::size_type i;
        // ASSIGN VALUES TO LIST "Value"
        for ( i=0, it=keyframes.begin(); it!=keyframes.end(); i++, it++ ) {

            value[0] [i] = (*it).getPosition()[0];
            value[1] [i] = (*it).getPosition()[1];
            value[2] [i] = (*it).getPosition()[2];

            const asl::Quaternionf & myOrientation = (*it).getOrientation() ;
            keyFrameOrientations.push_back(myOrientation);
        }


        quaternionSpline = new QuaternionSpline(keyFrameOrientations, time);

        // INIT SPLINES xyz(s)
        s = time;
        for ( int i=0; i<3; i++ ) {
            AC_TRACE << "initializing spline " << i << endl;
            if ( ! _spline[i]->init ( s, value[i], _init_mode, false ) ) {
                AC_ERROR << "CoordSpline::init: could not initialise value "<< i <<endl;
                return false;
            }
        }

        // AVOID EARTH COLLISION
        dvector::size_type iLast = time.size() -1;
        for ( dvector::size_type segment=0; segment<iLast; segment++ ) {
            avoidEarthCollision( segment );
        }

        // INIT SPLINE s(t)
        AC_TRACE << "initializing spline s(t)" << endl;
        if ( ! _spline[TIME]->init ( time, s, _init_mode, true ) ) {
            AC_ERROR << "CoordSpline::init: could not initialise spline" << endl;
            return false;
        }
        if( debug ) _spline[TIME]->print();

        for ( it=keyframes.begin(); it!=keyframes.end(); it++ ) {
            float mySpeed = (*it).getSpeed();
            speed.push_back ( mySpeed );
        }
        _spline[TIME]->slowdown ( speed );

        AC_TRACE << "s(t) after slowdown:" << endl;
        if( debug ) _spline[TIME]->print();

        // ESTIMATE TOTAL_PATH
        if ( _planet_mode ) {
          //   integrate ( v_planet(*this), 0.0, time[iLast], total_path );
          throw(new asl::Exception("not yet implemented"));
        }

        return true;

    }



    ////////////////////////////////////////////////////////////////////////////////
    //
    //  xyz
    //
    ////////////////////////////////////////////////////////////////////////////////

    asl::Vector3f
        CoordSpline::xyz ( float s )
    {
        float x, y, z;

        x = (*_spline[POSITION_X])(s);
        y = (*_spline[POSITION_Y])(s);
        z = (*_spline[POSITION_Z])(s);

        return asl::Vector3f( x, y, z );
    }



    ////////////////////////////////////////////////////////////////////////////////
    //
    //  hpr
    //
    ////////////////////////////////////////////////////////////////////////////////

#if 0
    asl::Vector3f
        CoordSpline::getHPR ( float s )
    {
    	//TODO: this is broken due to quaternion interpolation
        asl::Vector3f right, front, up;
        asl::Vector3f hpr;

        right = Vector3f((*_spline[RIGHT_X])(s), (*_spline[RIGHT_Y])(s), (*_spline[RIGHT_Z])(s) );
        front = Vector3f((*_spline[FRONT_X])(s), (*_spline[FRONT_Y])(s), (*_spline[FRONT_Z])(s) );

        up = cross( front, right );

        asl::Matrix4f myMatrix;
        myMatrix.makeIdentity();
        asl::Vector4f myUp(up[0], up[1], up[2], 0.0);
        myMatrix.assignRow(1,myUp);

        asl::Vector4f myFront(front[0], front[1], front[2], 0.0);
        myMatrix.assignRow(2,myFront);

        asl::Vector4f myRight(right[0], right[1], right[2], 0.0);
        myMatrix.assignRow(0,myRight);

        asl::Vector3f myScale;
        asl::Vector3f myShear;
        asl::Vector3f myOrientation;
        asl::Vector3f myPosition;
        myMatrix.decompose(myScale, myShear, myOrientation, myPosition);
        return myOrientation;
    }

#endif
    ////////////////////////////////////////////////////////////////////////////////
    //
    //  quaternion
    //
    ////////////////////////////////////////////////////////////////////////////////
    asl::Quaternionf
        CoordSpline::getQuaternion ( float s )
    {
    	asl::Quaternionf myOrientation;
        myOrientation  = quaternionSpline->getOrientation(s);
        AC_TRACE << "returning quatertion: " << myOrientation;
        return quaternionSpline->getOrientation(s);
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    //  s
    //
    ////////////////////////////////////////////////////////////////////////////////

    float
        CoordSpline::s ( float t )
    {
        return (*_spline[TIME])(t);
    }



    ////////////////////////////////////////////////////////////////////////////////
    //
    //  sPlanet
    //
    ////////////////////////////////////////////////////////////////////////////////

    float
        CoordSpline::sPlanet ( float t )
    {
        return (*_spline[4])(t);
    }



    ////////////////////////////////////////////////////////////////////////////////
    //
    //  v
    //
    ////////////////////////////////////////////////////////////////////////////////

    float
        CoordSpline::v ( float t )
    {
        return (*_spline[TIME]) ( t, 1 );
    }



    ////////////////////////////////////////////////////////////////////////////////
    //
    //  vPlanet
    //
    ////////////////////////////////////////////////////////////////////////////////
#if 0

    float
        CoordSpline::vPlanet ( float s )
    {
        float radius = magnitude(xyz(s));
        float height = radius;// -EarthRadiusD;
        const float min_height = 1.0;

        //bool debug = false;

        // CHECK HEIGHT VALUE AND CORRECT IT IF NECCESSARY
        if ( height < min_height ) {
            if ( _error_count < _error_maxnum ) {
                AC_ERROR << "CoordSpline::vPlanet: bad value: height("<<s<<") = "<<height<<endl;
            } else if ( _error_count ==  _error_maxnum ) {
                AC_ERROR << "CoordSpline::vPlanet: bad value: stop error report"<<endl;
            }
            ++_error_count;
            height = fabs( height );
            if( height < min_height ) height = min_height;
        }

        asl::Vector3f trans = vCamera( s ) / height;
        asl::Vector3f rot   = vFront( s );
        AC_TRACE << "arcElementTrans / height = "<<trans<<", arcElementRot = "<<rot<<endl;

        float subjectiveArcElement = magnitude(( trans + rot )) + magnitude(vRight(s));
        return 1.0f / subjectiveArcElement;
    }

#endif
    ////////////////////////////////////////////////////////////////////////////////
    //
    //  arcElementTrans
    //
    //  sqrt ( x'^2 + y'^2 + z'^2 )
    //
    ////////////////////////////////////////////////////////////////////////////////

    float
        CoordSpline::arcElementTrans ( float s )
    {

        float result =
            sqrt (
            (*_spline[POSITION_X])(s, 1) * (*_spline[POSITION_X])(s, 1) +
            (*_spline[POSITION_Y])(s, 1) * (*_spline[POSITION_Y])(s, 1) +
            (*_spline[POSITION_Z])(s, 1) * (*_spline[POSITION_Z])(s, 1)
            );

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    //  arcElementRot
    //
    ////////////////////////////////////////////////////////////////////////////////
#if 0
    float
        CoordSpline::arcElementRot ( float s )
    {
        //TODO: broken due to quaternion orientaion
        float result =
            sqrt (
            (*_spline[RIGHT_X])(s, 1) * (*_spline[RIGHT_X])(s, 1) +
            (*_spline[RIGHT_Y])(s, 1) * (*_spline[RIGHT_Y])(s, 1) +
            (*_spline[RIGHT_Z])(s, 1) * (*_spline[RIGHT_Z])(s, 1) +
            (*_spline[FRONT_X])(s, 1) * (*_spline[FRONT_X])(s, 1) +
            (*_spline[FRONT_Y])(s, 1) * (*_spline[FRONT_Y])(s, 1) +
            (*_spline[FRONT_Z])(s, 1) * (*_spline[FRONT_Z])(s, 1)
            );

        return result;
    }
#endif


    ////////////////////////////////////////////////////////////////////////////////
    //
    //  getPath
    //
    ////////////////////////////////////////////////////////////////////////////////

    float
        CoordSpline::getPath ( int i )
    {
        if ( i<0 || i>=_spline[TIME]->dim() ) {
            AC_ERROR << "CoordSpline::getPath: bad value of arg i = "<< i;
            return 0.0;
        }
        return _spline[TIME]->y(i);
    }



    ////////////////////////////////////////////////////////////////////////////////
    //
    //  getTime
    //
    ////////////////////////////////////////////////////////////////////////////////

    float
        CoordSpline::getTime ( int i )
    {
        if ( i<0 || i>=_spline[TIME]->dim() ) {
            AC_ERROR << "CoordSpline::getTime: bad value of arg i = "<< i;
            return 0.0;
        }
        return _spline[TIME]->x(i);
    }


#if 0
    ////////////////////////////////////////////////////////////////////////////////
    //
    //  vFront
    //
    ////////////////////////////////////////////////////////////////////////////////

    asl::Vector3f
        CoordSpline::vFront( float s )
    {
        //
        float x = (*_spline[FRONT_X]) ( s, 1 );
        float y = (*_spline[FRONT_Y]) ( s, 1 );
        float z = (*_spline[FRONT_Z]) ( s, 1 );

        return asl::Vector3f( x, y, z );
    }



    ////////////////////////////////////////////////////////////////////////////////
    //
    //  vRight
    //
    ////////////////////////////////////////////////////////////////////////////////

    asl::Vector3f
        CoordSpline::vRight( float s )
    {

        float x = (*_spline[RIGHT_X]) ( s, 1 );
        float y = (*_spline[RIGHT_Y]) ( s, 1 );
        float z = (*_spline[RIGHT_Z]) ( s, 1 );

        return asl::Vector3f( x, y, z );
    }



    ////////////////////////////////////////////////////////////////////////////////
    //
    //  vCamera
    //
    ////////////////////////////////////////////////////////////////////////////////

    asl::Vector3f
        CoordSpline::vCamera( float s )
    {

        float x = (*_spline[POSITION_X]) ( s, 1 );
        float y = (*_spline[POSITION_Y]) ( s, 1 );
        float z = (*_spline[POSITION_Z]) ( s, 1 );

        return asl::Vector3f( x, y, z );
    }

#endif

    ////////////////////////////////////////////////////////////////////////////////
    //
    //  avoidEarthCollision
    //
    ////////////////////////////////////////////////////////////////////////////////

    void
        CoordSpline::avoidEarthCollision ( int segment )
    {

        float minHeight  = 0.0;
        //    float fLimit  = (minHeight+EarthRadiusF)*(minHeight+EarthRadiusF);
        //bool debug    = false;

        // GET 10 SAMPLEPOINTS
        float x1, x2, x3, y1, y2, y3, step, x, y;
        x1      = _spline[POSITION_X]->x(segment);
        x3      = _spline[POSITION_X]->x(segment+1);
        step    = (x3-x1) / 10.0f;

        x = x2 = x1;
        y = y2 = rad23( x1 );
        while( x<x3 ) {
            x += step;
            y  = rad23( x );
            if( y<y2 ) {
                x2 = x;
                y2 = y;
            }
        }

        // IF NONE OF THE SAMPLE POINTS IS BELOW THE MIN HEIGHT RETURN
        if( y2 > minHeight ) {
            AC_TRACE << "avoidEarthCollision: segment "<<segment<<" ("<<x1<<", "<<x3<<") is ok";
            return;
        } else {
            AC_TRACE << "avoidEarthCollision: segment "<<segment<<" ("<<x1<<", "<<x3<<") is bad";
        }


        y1 = rad23( x1 );
        if( y2 >= y1 ) {
            AC_ERROR << "CoordSpline: no minimum search possible";
            return;
        }

        y3 = rad23( x3 );
        if( y2 >= y3 ) {
            AC_ERROR << "CoordSpline: no minimum search possible";
            return;
        }

        int count = 0;
        int maxcount = 100;
        float tol = 0.001f;
        float xmin;

        // IF THE LEFT KEYFRAME IS CLOSER TO THE EARTH REDUCE THE TENSION AT THE RIGHT SIDE AND VICE VERSA
        int lowerKeyframe, higherKeyframe;
        if( y1<y3 ) {
            lowerKeyframe  = segment;
            higherKeyframe = segment+1;
            minHeight  = y1;
        } else {
            lowerKeyframe  = segment+1;
            higherKeyframe = segment;
            minHeight  = y3;
        }

        bool success = true;
        while( minRad2( x1, x2, x3, tol, &xmin ) < minHeight ) {
            const float h_factor = 0.9f;
            const float l_factor = 0.9f;
            _spline[POSITION_X]->changeTension( higherKeyframe, h_factor );
            _spline[POSITION_Y]->changeTension( higherKeyframe, h_factor );
            _spline[POSITION_Z]->changeTension( higherKeyframe, h_factor );
            _spline[POSITION_X]->changeTension( lowerKeyframe,  l_factor );
            _spline[POSITION_Y]->changeTension( lowerKeyframe,  l_factor );
            _spline[POSITION_Z]->changeTension( lowerKeyframe,  l_factor );
            AC_TRACE << "repairing spline segment "<<segment<<", step "<< count;
            if( count == maxcount ) {
                AC_ERROR << "CoordSpline::avoidEarthCollision: cannot repair splineset";
                success = false;
                break;
            }
            count++;
        }
        if (success) {
            AC_TRACE << "done.";
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    //  rad23
    //
    ////////////////////////////////////////////////////////////////////////////////

    float
        CoordSpline::rad23( float t )
    {
        asl::Vector3f x = xyz( t );
        return magnitude(x);
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    //  minRad2
    //
    //  find minimum by parabolic interpolation.
    //  the algorithm is described in Numerical recipes, parabolic interpolation
    //  and Brent's method. See p 402, eq 10.2.1, see also "brent.c"
    //
    ////////////////////////////////////////////////////////////////////////////////


#    define ITMAX 100
#    define CGOLD 0.3819660
#    define ZEPS 1.0e-10
#    define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);
#    define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

    float
    CoordSpline::minRad2(float ax, float bx, float cx, float tol, float *xmin)
    {
        int iter;
        float a,b,d=0,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm;
        float e=0.0;

        a=(ax < cx ? ax : cx);
        b=(ax > cx ? ax : cx);
        x=w=v=bx;
        fw=fv=fx=rad23(x);
        for (iter=1;iter<=ITMAX;iter++) {
            xm=0.5f*(a+b);
            tol2=float(2.0f*(tol1=float(tol*fabs(x)+ZEPS)));
            if (fabs(x-xm) <= (tol2-0.5*(b-a))) {
                *xmin=x;
                return fx;
            }
            if (fabs(e) > tol1) {
                r=(x-w)*(fx-fv);
                q=(x-v)*(fx-fw);
                p=(x-v)*q-(x-w)*r;
                q=2.0f*(q-r);
                if (q > 0.0) p = -p;
                q=fabs(q);
                etemp=e;
                e=d;
                if (fabs(p) >= fabs(0.5f*q*etemp) || p <= q*(a-x) || p >= q*(b-x))
                    d=float(CGOLD*(e=(x >= xm ? a-x : b-x)));
                else {
                    d=p/q;
                    u=x+d;
                    if (u-a < tol2 || b-u < tol2)
                        d=SIGN(tol1,xm-x);
                }
            } else {
                d=float(CGOLD*(e=(x >= xm ? a-x : b-x)));
            }
            u=(fabs(d) >= tol1 ? x+d : x+SIGN(tol1,d));
            fu=rad23(u);
            if (fu <= fx) {
                if (u >= x) a=x; else b=x;
                SHFT(v,w,x,u)
                    SHFT(fv,fw,fx,fu)
            } else {
                if (u < x) a=u; else b=u;
                if (fu <= fw || w == x) {
                    v=w;
                    w=u;
                    fv=fw;
                    fw=fu;
                } else if (fu <= fv || v == x || v == w) {
                    v=u;
                    fv=fu;
                }
            }
        }
        AC_ERROR << "Too many iterations in brent";
        *xmin=x;
        return fx;
    }
#    undef ITMAX
#    undef CGOLD
#    undef ZEPS
#    undef SHFT


}
