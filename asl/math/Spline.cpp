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
//   $Revision: 1.1 $
//
//      Author: Axel Kilian
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include <math.h>

#include "Spline.h"

using namespace std;  // automatically added!

namespace asl {
    bool
        YrSpline::check_input ( const dvector& x, const dvector& y )
    {

        int xdim = x.size();
        int ydim = y.size();

        // check for equal lengths
        if ( xdim != ydim ) {
            AC_ERROR << "C2: different dimensions of x and y ("<<xdim<<" / "<< ydim << endl;
            return false;
        }

        // check at least two points
        if ( xdim < 2 ) {
            AC_ERROR << "C2: too few points for interpolation ("<<xdim <<")"<<endl;
            return false;
        }

        // check for ascending x values
        for ( int i=1; i<xdim; i++ ) {
            if ( x[i] <= x[i-1] ) {
                AC_ERROR << "C2: x values not in ascending order (x["<<i<<"] = "<<x[i]<<", x["<<i-1<<"] = "<<x[i-1]<<")"<<endl;            }
        }
        
        return true;
    
    }
    
    float
    YrSpline::operator () ( float t, int order )
    {          
        AC_ERROR << "YrSpline::operator (): this function should never be called" << endl;
        return 0.0;  
    }
    
    
    
 
    Hermite::Hermite(void)
    {
        _setup_complete = false;
    }
    
       
    Hermite::~Hermite(void)
    {
    }
    
    //------------------------------------------------------------------------------
    // GET DERIVATIVES BY THE METHOD OF "minimize_fluctuations" or "catmull_rom"
    //------------------------------------------------------------------------------
    
    bool
    Hermite::init ( const dvector& x, const dvector& y, HermiteInitMode initMode, bool ascend_only )
    {
        _x	    = x;
        _y	    = y;
            
        // CHECK INPUT
        if ( check_input( x, y ) == false ) {
            return false;
        } else {
            _dim = x.size();
        }

        // CREATE VECTOR OF APPROPRIATE LENGTH
        _dy_dx_left = _dy_dx_right = y;
        
        // GET DERIVATIVES BY THE APPROPRIATE METHOD
        _set_derivatives( initMode, ascend_only );
        
        _setup_complete = true;
        return true;
    }
    
    
    //------------------------------------------------------------------------------
    // GET DERIVATIVES BY THE METHOD OF WEIGHTED RATIO. THE RATIOS ARE CONTAINED IN "weight"
    //------------------------------------------------------------------------------
    
    bool
    Hermite::init ( const dvector& x, const dvector& y, const dvector& weight )
    {
        _x	    = x;
        _y	    = y;
        
        // CHECK INPUT
        if ( check_input( x, y ) == false ) {
            return false;
        } else {
            _dim = x.size();
        }
    
        // CREATE VECTOR OF APPROPRIATE LENGTH
        _dy_dx_left = _dy_dx_right = y;
        
        // GET DERIVATIVES BY THE METHOD OF WEIGHTED RATIO. THE RATIOS ARE CONTAINED IN "weight"
        _set_derivatives( weight );
        
        _setup_complete = true;
        return true;
    }
    
    //------------------------------------------------------------------------------
    // INIT SPLINE WITH PRESCRIBED DERIVATIVES
    //------------------------------------------------------------------------------
    
    bool
    Hermite::init ( const dvector& x, const dvector& y, const dvector& dy_dx_left, const dvector& dy_dx_right )
    {
        _x		    = x;
        _y		    = y;
        _dy_dx_left	    = dy_dx_left;
        _dy_dx_right    = dy_dx_right;
        
        // CHECK INPUT
        if ( check_input( x, y ) == false ) {
            return false;
        } else {
            _dim = x.size();
        }    
        _setup_complete = true;
        return true;
    }

    float
    Hermite::x_first ( void )
    {
        if ( _setup_complete ) {
            AC_ERROR << "Hermite::x_first: _setup not complete\n" << endl;
            return 0.0;
        }
        dvector::iterator i = _x.begin();
        return *i;          
    }

    float
    Hermite::x_last ( void )
    {    
        if ( ! _setup_complete ) {
            AC_ERROR << "Hermite::x_first: _setup not complete";
            return 0.0;
        }
        
        dvector::iterator i = _x.end();
        i--;
        return *i;          
    }
    
    float
    Hermite::y_first ( void )
    {

        if ( ! _setup_complete ) {
            AC_ERROR << "Hermite::y_first: _setup not complete";
            return 0.0;
        }

        dvector::iterator i = _y.begin();
        return *i;          
    }
   
    float
    Hermite::y_last ( void )
    {
    
        if ( ! _setup_complete ) {
            AC_ERROR << "Hermite::y_first: _setup not complete\n" << endl;
            return 0.0;
        }
        
        dvector::iterator i = _y.end();
        i--;
        return *i;          
    }
    
    void
    Hermite::changeTension( int keyframe, float factor )
    {
    
        if ( keyframe < 0 || keyframe > dim()-1 ) {
            AC_ERROR << "Hermite::changeTension: bad value of keyframe << " << keyframe << endl;
            return;
        }
        
        _dy_dx_left  [keyframe] *= factor;
        _dy_dx_right [keyframe] *= factor;
    
    }
     
    void
    Hermite::zeroCurvature( void )
    {
    
        // SET DERIVATIVES AT THE ENDPOINTS SUCH THAT THE CURVATURE THERE IS ZERO
        float h, yl, yr, dy_o;
        h	= _x[1] -_x[0]; 
        yl	= _y[0]; 
        yr	= _y[1];
        dy_o= _dy_dx_left [1];
        _dy_dx_left [     0] = ( -dy_o * h - 3.0f * (yl-yr) ) / ( 2.0f * h );
        h	= _x[_dim-1] -_x[_dim-2]; 
        yl	= _y[_dim-2]; 
        yr	= _y[_dim-1]; 
        dy_o= _dy_dx_left [_dim-2];
        _dy_dx_left [_dim-1] = ( -dy_o * h + 3.0f * (yl-yr) ) / ( 2.0f * h );
    
    }
    
    void
    Hermite::print ( void )
    {
        if ( ! _setup_complete ) {
            AC_ERROR << "Hermite: cannot print before setup was succesful" << endl;
        }
        AC_PRINT << "x          y          y'left        y'right\n";
        AC_PRINT << "===========================================\n" ;
        for ( int i=0; i<_dim; i++ ) {
            AC_PRINT <<_x[i] << " " << _y[i] << " " << _dy_dx_left[i] << " " << _dy_dx_right[i] << endl;
        }          
    }

    
    
    ///////////////////////////////////////////////////////////////////////////////
    //
    //	slowdown
    //
    ////////////////////////////////////////////////////////////////////////////////
    
    void
    Hermite::slowdown ( const dvector& speed_multiplier )
    {

        // CHECK SETUP COMPLETE
        if ( ! _setup_complete ) {
            AC_ERROR << "Hermite: cannot slowdown before setup was succesful" ;
            return;
        }

        // CHECK SIZES
        int sdim = speed_multiplier.size();
        if ( sdim != _dim ) {
            AC_ERROR << "Hermite: wrong size of speed_multiplier ("<<sdim<<", should be "<<_dim;
            return;
        }
    
        int i = 0;
    
        // CHECK VALUES OF SPEED_MULTIPLIER
        for ( i=0; i<_dim; i++ ) {
            if ( speed_multiplier[i] < 0.0 ) {
                AC_ERROR << "Hermite: bad value of speed_multiplier, cannot be negative) m ="<<speed_multiplier[i];
                return;
            }
        }          
    
        // MULTIPLY DERIVATIVES AT THE CONTROL POINTS BY SPEED_MULTIPLIER
        for ( i=0; i<_dim; i++ ) {
            _dy_dx_left  [i] *= speed_multiplier [i];
        } 
        _dy_dx_right = _dy_dx_left;
    }
       
    ////////////////////////////////////////////////////////////////////////////////
    //
    //	operator ()
    //
    ////////////////////////////////////////////////////////////////////////////////

    float
    Hermite::operator () ( float t )
    {

        float retval = 0.0;

        if ( ! _setup_complete ) {
            return retval;
        }

        // BEFORE OR AT THE FIRST POINT: RETURN FIRST VALUE
        if ( t <= _x[0] ) {
            retval = _y[0];
        } else
            // AFTER OR AT THE LAST POINT: RETURN LAST VALUE
            if ( t >= _x[_dim-1] ) {
                retval = _y[_dim-1];
            }
            // INBETWEEN THE FIRST AND THE LAST POINT: INTERPOLATE
            else {
                for ( int i=1; i<_dim; i++ ) {
                    if ( t >= _x[i-1] && t < _x[i] ) {
                        retval = _spline_segment ( t, _y [i-1], _y [i], _dy_dx_right [i-1], _dy_dx_left [i], _x[i-1], _x[i] );
                        break;
                    }
                } 
            }
            return retval;  
    }
       
    float
    Hermite::operator () ( float t, int order )
    {  
        float retval = 0.0;

        if ( ! _setup_complete ) {
            return retval;
        }

        // ORDER 0: RETURNS FUNCTION
        if ( order == 0 ) {
            return (*this) (t);
        }

        // ORDER != 1: not supported
        if ( order != 1 ) {
            AC_ERROR << "Hermite: order of derivative "<<order<<" not supported";
            return retval;
        }

        // ORDER == 1: RETURN DERIVATIVE

        // BEFORE THE FIRST POINT: RETURN DERIVATIVE AT FIRST POINT
        if ( t < _x[0] ) {
            t = _x[0];  
        }

        // AFTER THE LAST POINT: RETURN DERIVATIVE AT LAST POINT
        else if ( t > _x[_dim-1] ) {
            t = _x[_dim-1];  
        }

        // INBETWEEN THE FIRST AND THE LAST POINT: INTERPOLATE
        for ( int i=1; i<_dim; i++ ) {
            if ( t >= _x[i-1] && t <= _x[i] ) {
                retval = _spline_segment ( t, _y[i-1], _y[i], _dy_dx_right[i-1], _dy_dx_left[i], _x[i-1], _x[i], order );
                break;
            }
        } 

        return retval;  
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    //	_set_derivatives
    //
    ////////////////////////////////////////////////////////////////////////////////
    
    void
    Hermite::_set_derivatives ( HermiteInitMode initMode, bool ascend_only )
    {   
        int i, j;   
        switch ( initMode ) {
        
            // CASE CATMULL_ROM
        case catmull_rom:
            {
                // WHEN THERE ARE ONLY TWO POINTS
                if ( _dim == 2 ) {
                    float dy	    = _y[1] -_y[0];
                    float dx	    = _x[1] -_x[0];
                    _dy_dx_left [0]	    = _dy_dx_left [1] =  dy / dx;
                    break;
                }

                // SET INNER DERIVATIVES
                for ( i=1; i<_dim-1; i++ ) {
                    float dy	    = _y[i+1] -_y[i-1];
                    float dx	    = _x[i+1] -_x[i-1];
                    _dy_dx_left  [i]    = dy / dx;
                }

                // SET DERIVATIVES AT THE ENDPOINTS SUCH THAT THE CURVATURE THERE IS ZERO
                zeroCurvature();

                // WATCH FOR POSITIVE SLOPE
                if ( ascend_only ) {
                    _new_derivative ( _dy_dx_left[0     ], _dy_dx_left[1     ], _x[0     ], _x[1     ], _y[0     ], _y[1     ] );
                    _new_derivative ( _dy_dx_left[_dim-1], _dy_dx_left[_dim-2], _x[_dim-2], _x[_dim-1], _y[_dim-2], _y[_dim-1] );
                    for ( i=0; i<_dim-1; i++ ) {
                        _new_derivative ( _dy_dx_left[i+1], _dy_dx_left[i], _x[i], _x[i+1], _y[i], _y[i+1] );
                    }
                }	    
                break;
            }


            // CASE WEIGHTED_RATIO
        case weighted_ratio:
            {
                _dy_dx_left[0] = (_y[1] -_y[0]) / (_x[1] -_x[0]);
                for ( i=1; i<_dim-1; i++ ) {
                    float dx_left	    = _x[i  ] -_x[i-1];
                    float dy_left	    = _y[i  ] -_y[i-1];
                    float dx_right	    = _x[i+1] -_x[i  ];
                    float dy_right	    = _y[i+1] -_y[i  ];
                    float dist_left    = sqrt( dx_left*dx_left + dy_left*dy_left );
                    float dist_right   = sqrt( dx_right*dx_right + dy_right*dy_right );
                    float dy_dx_left   = dy_left  / dx_left;
                    float dy_dx_right  = dy_right / dx_right;
                    _dy_dx_left  [i]    = (dist_left * dy_dx_right + dist_right * dy_dx_left) / (dist_left+dist_right);
                }
                _dy_dx_left [_dim-1] = (_y[_dim-1] -_y[_dim-2]) / (_x[_dim-1] -_x[_dim-2]);

                // SET DERIVATIVES AT THE ENDPOINTS SUCH THAT THE CURVATURE THERE IS ZERO
                zeroCurvature();


                break;
            }


            // CASE MINIMIZE_FLUCTUATIONS_FORWARD
        case minimize_fluctuations_forward:
            {
                _dy_dx_left [0] = (_y[1] -_y[0]) / (_x[1] -_x[0]);
                for ( i=0; i<_dim-1; i++ ) {
                    _dy_dx_left [i+1] = ( 3.0f*(_y[i+1] -_y[i]) / (_x[i+1] -_x[i]) -_dy_dx_left[i] ) / 2.0f;
                }
                break;
            }

            // CASE MINIMIZE_FLUCTUATIONS
        case minimize_fluctuations:
            {
                const float iteration_speed = 0.5;	// must be between 0..1, 

                // INIT
                _dy_dx_left [0] = (_y[1] -_y[0]) / (_x[1] -_x[0]);

                // WATCH FOR POSITIVE SLOPE
                if (ascend_only && _dy_dx_left[0] < 0.0) _dy_dx_left [0] = 0.0;

                // ITERATION LOOP
                for ( j=0; j<20; j++ ) {

                    // FORWARD
                    for ( i=0; i<_dim-1; i++ ) {
                        float new_dy_dx = ( 3.0f*(_y[i+1] -_y[i]) / (_x[i+1] -_x[i]) -_dy_dx_left[i] ) / 2.0f;

                        // WATCH FOR POSITIVE SLOPE
                        if (ascend_only) {
                            _new_derivative ( new_dy_dx, _dy_dx_left[i], _x[i], _x[i+1], _y[i], _y[i+1] );
                            if ( i != _dim-1 ) {
                                _new_derivative ( new_dy_dx, _dy_dx_left[i+2], _x[i+1], _x[i+2], _y[i+1], _y[i+2] );			
                            }
                        }

                        // ITERATION STEP
                        if ( j==0 ) {
                            _dy_dx_left[i+1] = new_dy_dx;
                        } else {
                            _dy_dx_left[i+1] = float(iteration_speed * new_dy_dx + (1.0-iteration_speed) * _dy_dx_left[i+1]);
                        }

                        // WATCH FOR POSITIVE SLOPE
                        if (ascend_only) {
                            _new_derivative ( _dy_dx_left[i+1], _dy_dx_left[i], _x[i], _x[i+1], _y[i], _y[i+1] );
                            if ( i != _dim-1 ) {
                                _new_derivative ( _dy_dx_left[i+1], _dy_dx_left[i+2], _x[i+1], _x[i+2], _y[i+1], _y[i+2] );			
                            }
                        }
                    }

                    // BACKWARD
                    for ( i=_dim-2; i>=0; i-- ) {
                        float new_dy_dx = ( 3.0f*(_y[i+1] -_y[i]) / (_x[i+1] -_x[i]) -_dy_dx_left[i+1] ) / 2.0f;

                        // WATCH FOR POSITIVE SLOPE
                        if (ascend_only) {
                            _new_derivative ( new_dy_dx, _dy_dx_left[i+1], _x[i], _x[i+1], _y[i], _y[i+1] );
                            if ( i != 0 ) {
                                _new_derivative ( new_dy_dx, _dy_dx_left[i-1], _x[i-1], _x[i], _y[i-1], _y[i] );			
                            }
                        }

                        // ITERATION STEP
                        _dy_dx_left[i] = float(iteration_speed * new_dy_dx + (1.0-iteration_speed) * _dy_dx_left[i]);

                        // WATCH FOR POSITIVE SLOPE
                        if (ascend_only) {
                            _new_derivative ( _dy_dx_left[i], _dy_dx_left[i+1], _x[i], _x[i+1], _y[i], _y[i+1] );
                            if ( i != 0 ) {
                                _new_derivative ( _dy_dx_left[i], _dy_dx_left[i-1], _x[i-1], _x[i], _y[i-1], _y[i] );			
                            }
                        }
                    }
                }
                break;
            }
            // ERROR
        default:
            {
                AC_ERROR << "Hermite::_set_derivatives: unknown initMode "<< initMode;
            }
        }
        // COPY VECTOR
        _dy_dx_right = _dy_dx_left;
    }

    // GET DERIVATIVES BY THE METHOD OF WEIGHTED RATIO. THE RATIOS ARE CONTAINED IN "weight"
    
    void
    Hermite::_set_derivatives ( const dvector& weight )
    {
     
        //_dy_dx_left[0] = (_y[1] -_y[0]) / (_x[1] -_x[0]) / 10.0;
        _dy_dx_left[0] = (_y[1] -_y[0]) / (_x[1] -_x[0]);
        for ( int i=1; i<_dim-1; i++ ) {
    	float dx_left	    = _x[i  ] -_x[i-1];
    	float dy_left	    = _y[i  ] -_y[i-1];
    	float dx_right	    = _x[i+1] -_x[i  ];
    	float dy_right	    = _y[i+1] -_y[i  ];
    	float dy_dx_left   = dy_left  / dx_left;
    	float dy_dx_right  = dy_right / dx_right;
    	_dy_dx_left [i] = float( weight[i] * dy_dx_right + (1.0-weight[i]) * dy_dx_left);
        }
        _dy_dx_left [_dim-1] = (_y[_dim-1] -_y[_dim-2]) / (_x[_dim-1] -_x[_dim-2]);
        //_dy_dx_left [_dim-1] = (_y[_dim-1] -_y[_dim-2]) / (_x[_dim-1] -_x[_dim-2]) / 10.0;
    
    
        // COPY VECTOR
        _dy_dx_right = _dy_dx_left;
    }
    
        
        
    ////////////////////////////////////////////////////////////////////////////////
    //
    //	_new_derivative
    //
    //	if neccessary, d_to_check is altered such that the slope of the spline
    //	segment is everywhere non-negative
    //
    ////////////////////////////////////////////////////////////////////////////////
    
    void
    Hermite::_new_derivative ( float& d_to_check, float d_other_side, float x1, float x2, float y1, float y2 )
    {
        float dmax;
        bool dmax_exists;

        // GENERATE SPLINE WITH POSITIVE SLOPE EVERYWHERE
        if ( d_to_check < 0.0 ) {
            d_to_check = 0.0;
        }
        float dy_dx    = (y2-y1) / (x2-x1);
        float sqrtarg  = 3.0f * d_other_side * ( 4.0f * dy_dx -d_other_side );
        dmax_exists	    = (sqrtarg >= 0);
        if ( dmax_exists ) {
            dmax = ( 6.0f * dy_dx -d_other_side + sqrt(sqrtarg) ) / 2.0f;
            if ( dmax < 0.0 ) {
                AC_ERROR << "Hermite::_new_derivative: negative value of dmax encountered"<< endl;
                d_to_check = 0.0;
                return;
            }
            if ( d_to_check > dmax ) {
                d_to_check = dmax;
                return;
            }
        }

    }
    
    ////////////////////////////////////////////////////////////////////////////////
    //
    //	_spline_segment
    //
    ////////////////////////////////////////////////////////////////////////////////
    
    float
    Hermite::_spline_segment ( float x, float y1, float y2, float dy1, float dy2 )
    {
        // RETURN SPLINE SEGMENT FOR x = 0...1

        if ( x < 0.0 ) { AC_ERROR << "Hermite::_spline_segment: bad value x = "<< x;}
        if ( x > 1.0 )  {AC_ERROR << "Hermite::_spline_segment: bad value x = "<< x;}

        float retval =
            + y1  * ( 1.0f - 3.0f * x*x + 2.0f *x*x*x )
            + y2  * ( 3.0f - 2.0f * x ) *x*x
            + dy1 * ( x - 1.0f ) * ( x - 1.0f ) * x
            + dy2 * ( x - 1.0f ) * x*x;

        return retval;
    }
    
    float
    Hermite::_spline_segment ( float x, float y1, float y2, float dy1, float dy2, float a, float b )
    {
    
        // RETURN SPLINE SEGMENT FOR x = a...b
        
    
        float h = b-a;
        float u = (x-a) / h;
        
        return _spline_segment ( u, y1, y2, dy1*h, dy2*h );
    
    }
    
    
    
    float
    Hermite::_spline_segment ( float x, float y1, float y2, float dy1, float dy2, float a, float b, int order )
    { 
        // RETURN 1. DERIVATIVE OF SPLINE SEGMENT FOR X = A...B

        if ( x < a ) { AC_ERROR << "Hermite::_spline_segment: bad value x = "<< x;}
        if ( x > b ) { AC_ERROR << "Hermite::_spline_segment: bad value x = "<< x;}

        float retval =
            (y1- y2) * (a-x) * (b-x) * 6.0f		/ ((b-a)*(b-a)*(b-a))
            + dy1 * (x-b) * (3.0f*x -b -2.0f*a)	/ ((b-a)*(b-a))
            + dy2 * (x-a) * (3.0f*x -2.0f*b -a)	/ ((b-a)*(b-a));

        return retval;
    }
    
     
    C2::C2(void)
    {
        _setup_complete = false;
    }
    
    C2::~C2(void)
    {
    }
        
    bool
    C2::init (	const dvector&	    x,
    		const dvector&	    y,
    		const C2_InitMode&  initMode, 
    		const float&	    left_gradient,
    		const float&	    right_gradient
    	 )
    {
        _x = x;
        _y = y;
               
        // CHECK INPUT
        if ( check_input( x, y ) == false ) {
    	return false;
        } else {
    	_dim = x.size();
        }
        
        
        // GET _H OF RIGHT SIZE
        _h = x;
        
        
        // GET X-STEPS
        for ( int i=1; i<_dim; i++ ) {
    	_h[i] = _x[i] -_x[i-1];
        }
        
        
        // GET SPLINE PARAMETERS S
        switch ( initMode ) {
    	
    	case setLeftRightDerivatives:
    	    _get_parameters( left_gradient, right_gradient );
    	    break;
    	
    	case leftRightDerivativesAutomatic:
    	    _get_parameters( initMode );
    	    break;
       
    	default:
    	    AC_ERROR << "C2::init: bad value of initMode: "<<initMode;
    	    return false; 
        }
        _setup_complete = true;
        return true;
    }
   
    void
        C2::print ( void ) {

            if ( ! _setup_complete ) {
                AC_ERROR << "C2: cannot print before setup was succesful";
            }

            AC_PRINT << "x         y         s         h          " << endl;
            AC_PRINT << "============================================" << endl;
            AC_PRINT <<  _x[0]<<" "<< _y[0]<<" "<< _s[0] << endl;
            for ( int i=1; i<_dim; i++ ) {
                AC_PRINT << _x[i]<<" "<< _y[i]<<" "<< _s[i]<<" "<< _h[i] << endl;
            }          
            AC_PRINT << "============================================" << endl;
        }
    
    
    
    ////////////////////////////////////////////////////////////////////////////////
    //
    //	operator ()
    //
    ////////////////////////////////////////////////////////////////////////////////
    
    float
    C2::operator () ( float t ) {
    
        float retval = 0.0;
    
        if ( ! _setup_complete ) {
    	return retval;
        }
        
              
        // BEFORE OR AT THE FIRST POINT: RETURN FIRST VALUE
        if ( t <= _x[0] ) {
    	retval = _y[0];
        }
    
        // AFTER OR AT THE LAST POINT: RETURN LAST VALUE
        else if ( t >= _x[_dim-1] ) {
    	retval = _y[_dim-1];
        }
    
        // INBETWEEN THE FIRST AND THE LAST POINT: INTERPOLATE
        else {
    	for ( int i=1; i<_dim; i++ ) {
    	    if ( t >= _x[i-1] && t < _x[i] ) {
    		retval =
    		    -  _s[i-1] / (6.0f*_h[i])			* (t -_x[i  ]) * (t -_x[i  ]) * (t -_x[i  ])
    		    +  _s[i  ] / (6.0f*_h[i])			* (t -_x[i-1]) * (t -_x[i-1]) * (t -_x[i-1])
    		    + (_y[i  ] / _h[i] - _s[i  ] * _h[i] / 6.0f)	* (t -_x[i-1])
    		    - (_y[i-1] / _h[i] - _s[i-1] * _h[i] / 6.0f)	* (t -_x[i  ]);
    	    }
    	} 
        }
              
        return retval;  
    }
    
    
    
    float
    C2::operator () ( float t, int order ) {
    
        float retval = 0.0;

        if ( ! _setup_complete ) {
            return retval;
        }

        // ORDER 0: RETURNS FUNCTION
        if ( order == 0 ) {
            return (t);
        }     

        // ORDER != 1: not supported
        if ( order != 1 ) {
            AC_ERROR <<"C2: order of derivative "<<order<<" not supported";
            return retval;
        }

        // ORDER == 1: return derivative

        // BEFORE THE FIRST POINT: RETURN FIRST POINT
        if ( t < _x[0] ) {
            t = _x[0];  
        }

        // AFTER THE LAST POINT: RETURN LAST POINT
        else if ( t > _x[_dim-1] ) {
            t = _x[_dim-1];  
        }

        // INBETWEEN THE FIRST AND THE LAST POINT: INTERPOLATE
        else {
            for ( int i=1; i<_dim; i++ ) {
                if ( t >= _x[i-1] && t <= _x[i] ) {
                    retval =
                        -  _s[i-1] / (2.0f*_h[i])			* (t -_x[i  ]) * (t -_x[i  ])
                        +  _s[i  ] / (2.0f*_h[i])			* (t -_x[i-1]) * (t -_x[i-1])
                        + (_y[i  ] / _h[i] - _s[i  ] * _h[i] / 6.0f)
                        - (_y[i-1] / _h[i] - _s[i-1] * _h[i] / 6.0f);
                }
            } 
        }

        return retval;  
    }


    void
        C2::_get_parameters ( const float& left_gradient, const float& right_gradient )
    {       
        dvector  a = _x;
        dvector  b = _x;

        // GET a, b
        a[0] =  1.0;
        b[0] =  0.0;
        a[1] = -2.0;
        b[1] =  6.0f /_h[1] * ( (_y[1] -_y[0]) / _h[1] -left_gradient );

        int i = 0;

        for ( i=2; i<_dim; i++ ) {
            float f1 = -2.0f * ( _h[i-1] + _h[i] ) / _h[i];
            float f2 = -_h[i-1] / _h[i];
            a[i] = f1 * a[i-1] + f2 * a[i-2];
            b[i] = f1 * b[i-1] + f2 * b[i-2] +
                6.0f * ( (_y[i  ] - _y[i-1]) / (_h[i  ]*_h[i]) -
                (_y[i-1] - _y[i-2]) / (_h[i-1]*_h[i]) );
        }


        // GET _s
        _s = _x;
        int last = _dim-1;
        _s[0] =
            ( 
            - 2.0f *  b[last  ] * _h[last] * _h[last]
            - 1.0f *  b[last-1] * _h[last] * _h[last]
            + 6.0f * _h[last  ] * right_gradient
                - 6.0f * _y[last  ]
                + 6.0f * _y[last-1]
                )
                    /
                    (
                    ( 2.0f * a[last] + a[last-1] ) * _h[last] * _h[last]
                    );

                    for ( i=1; i<_dim; i++ ) {
                        _s[i] = a[i] * _s[0] + b[i];
                    }
    }
    
    
    void
    C2::_get_parameters ( const C2_InitMode& initMode )
    {       
        dvector  a = _x;
        dvector  b = _x;

        if ( initMode != leftRightDerivativesAutomatic ) {
            AC_ERROR << "C2::_get_parameters: initmode %i not supported:"<<initMode;
            return;
        } else {
            AC_ERROR << "C2::_get_parameters: initmode \"leftRightDerivativesAutomatic\" does not work properly\n";
            return;
        }

    #if 0
    
        // GET a, b
        a[0] =  0.0;
        b[0] =  0.0;
        a[1] =  1.0;
        b[1] =  0.0;
        int last = _dim-1;
        for ( int i=2; i<last; i++ ) {
    	float f1 = -2.0 * ( _h[i-1] + _h[i] ) / _h[i];
    	float f2 = -_h[i-1] / _h[i];
    	a[i] = f1 * a[i-1] + f2 * a[i-2];
    	b[i] = f1 * b[i-1] + f2 * b[i-2]
    	     + 6.0 * (_y[i  ] -_y[i-1]) / (_h[i  ]*_h[i  ])
    	     - 6.0 * (_y[i-1] -_y[i-2]) / (_h[i-1]*_h[i  ]);
        }
        
        
        // GET _s
        _s = _x;
        _s[0] = 0.0;
        _s[1] =
        ( 
    	 (_y[last  ] -_y[last-1])  / _h[last  ]
    	-(_y[last-1] -_y[last-2])  / _h[last-1]
    	-  b[last-2] *_h[last-1]   /  6.0
    	+  b[last-1] * (_h[last-1] + _h[last]) / 3.0
        )
    	/
        (
    	  a[last-2] * _h[last-1] / 6.0
    	+ a[last-1] * (_h[last-1] + _h[last]) / 3.0
        );
    	
        for ( i=2; i<last; i++ ) {
    	_s[i] = a[i] * _s[1] + b[i];
        }
        _s[last] = 0.0;
    
    #endif
    
    }
    
    
    Linear::Linear(void)
    {
        _setup_complete = false;
    }
    
     Linear::~Linear(void)
    {
    }
       
    bool
    Linear::init ( const dvector& x, const dvector& y )
    {
        // CHECK INPUT
        if ( check_input( x, y ) == false ) {
    	return false;
        } else {
    	_dim = x.size();
        }
            
        _x = x;
        _y = y;    
        _setup_complete = true;
        
        return true;
    }
        
    void
    Linear::print ( void ) {
    
        if ( ! _setup_complete ) {
            AC_ERROR <<"C2: cannot print before setup was succesful";
        }

        AC_PRINT << "x          y          \n";
        AC_PRINT << "======================\n";
        for ( int i=0; i<_dim; i++ ) {
            AC_PRINT << _x[i]<<" "<< _y[i]<<endl;
        }          
        AC_PRINT << "======================\n";
    }
    
    
    
    ////////////////////////////////////////////////////////////////////////////////
    //
    //	operator ()
    //
    ////////////////////////////////////////////////////////////////////////////////
    
    float
    Linear::operator () ( float t ) {
    
        float retval = 0.0;
    
        if ( ! _setup_complete ) {
    	return retval;
        }
        
              
        // BEFORE OR AT THE FIRST POINT: RETURN FIRST VALUE
        if ( t <= _x[0] ) {
    	retval = _y[0];
        }
    
        // AFTER OR AT THE LAST POINT: RETURN LAST VALUE
        else if ( t >= _x[_dim-1] ) {
    	retval = _y[_dim-1];
        }
    
        // INBETWEEN THE FIRST AND THE LAST POINT: INTERPOLATE
        else {
    	for ( int i=1; i<_dim; i++ ) {
    	    if ( t >= _x[i-1] && t < _x[i] ) {
    		float dx = _x[i] -_x[i-1];
    		float dy = _y[i] -_y[i-1];
    		retval = _y[i-1] + (t-_x[i-1]) * dy / dx;
    	    }
    	} 
        }
              
        return retval;  
    }
    
    
    
    float
        Linear::operator () ( float t, int order ) {

            float retval = 0.0;

            if ( ! _setup_complete ) {
                return retval;
            }       

            // ORDER 0: RETURNS FUNCTION
            if ( order == 0 ) {
                return (t);
            }

            // ORDER != 1: not supported
            if ( order != 1 ) {
                AC_ERROR << "C2: order of derivative "<<order<<" not supported";
                return retval;
            }

            // ORDER == 1: return derivative

            if ( t < _x[0] ) {
                // BEFORE THE FIRST POINT: RETURN DERIVATIVE AT FIRST POINT
                t = _x[0];  
            } else if ( t > _x[_dim-1] ) {
                // AFTER THE LAST POINT: RETURN DERIVATIVE AT LAST POINT
                t = _x[_dim-1];  
            } else {
                // INBETWEEN THE FIRST AND THE LAST POINT: INTERPOLATE
                for ( int i=1; i<_dim; i++ ) {
                    if ( t >= _x[i-1] && t <= _x[i] ) {
                        float dx = _x[i] -_x[i-1];
                        float dy = _y[i] -_y[i-1];
                        retval = dy / dx;
                    }
                } 
            }
            return retval;  
        }

}

