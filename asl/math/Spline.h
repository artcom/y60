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
//  Author: Axel Kilian
//
//  $Revision: 1.4 $
//
//  Description:
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_Spline_
#define _included_asl_Spline_

#include "asl_math_settings.h"

#include <asl/base/Logger.h>
#include <vector>
#include <iostream>


namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */

    typedef std::vector<float> dvector;

    inline
    void
    debug_print(const char * theStr, dvector theVector) {
        AC_TRACE << theStr << " : ";
        for (dvector::size_type i = 0; i < theVector.size(); i++) {
            AC_TRACE << theVector[i] << "," ;
        }
        AC_TRACE << std::endl;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    //	YrSpline
    //
    ////////////////////////////////////////////////////////////////////////////////

    class ASL_MATH_DECL YrSpline
    {
      public:
    		    YrSpline	    ( void ) { }
        virtual	    ~YrSpline	    ( void ) { }
        virtual void    print	    ( void ) = 0;
        virtual float  operator ()	    ( float ) = 0;
        virtual float  operator ()	    ( float, int );
        //virtual void    inverse	    ( const float& in, multiset<float>& out );
        bool	    check_input	    ( const dvector& x, const dvector& y );
    };



    ////////////////////////////////////////////////////////////////////////////////
    //
    //	Hermite : public YrSpline
    //
    ////////////////////////////////////////////////////////////////////////////////

    enum HermiteInitMode { catmull_rom, minimize_fluctuations, minimize_fluctuations_forward, weighted_ratio };	// how to init Hermite

    class ASL_MATH_DECL Hermite : public YrSpline
    {
      public:
    		    Hermite	    ( void );
    		    ~Hermite	    ( void );

        void	    print	    ( void );
        void	    slowdown	    ( const dvector& speed_multiplier );
        float	    operator ()	    ( float t );
        float       evaluate        ( float t , int order ) { return this->operator()(t, order); }
        float	    operator ()	    ( float t, int order );
        bool	    init	    ( const dvector& x, const dvector& y, const dvector& dy_dx_left, const dvector& dy_dx_right );
        bool	    init	    ( const dvector& x, const dvector& y, HermiteInitMode initMode, bool ascend_only );
        bool	    init	    ( const dvector& x, const dvector& y, const dvector& weight );
        //void	    inverse	    ( const float& in, multiset<float>& out );
        float	    x_first	    ( void ) const;
        float	    x_last	    ( void ) const;
        float	    y_first	    ( void ) const;
        float	    y_last	    ( void ) const;
        int		    dim		    ( void ) const { return _x.size(); }
        float	    x		    ( int i ) { return _x[i]; }
        float	    y		    ( int i ) { return _y[i]; }
        float	    dyl		    ( int i ) { return _dy_dx_left [i]; }
        float	    dyr		    ( int i ) { return _dy_dx_right[i]; }
        void	    changeTension   ( int keyframe, float factor );


      private:
        dvector	    _x;
        dvector	    _y;
        dvector	    _dy_dx_left;
        dvector	    _dy_dx_right;
        int		    _dim;
        bool	    _setup_complete;

        void	    zeroCurvature   ( void );
        float	    _spline_segment ( float x, float y1, float y2, float dy1, float dy2 );
        float	    _spline_segment ( float x, float y1, float y2, float dy1, float dy2, float a, float b );
        float	    _spline_segment ( float x, float y1, float y2, float dy1, float dy2, float a, float b, int order );
        void	    _set_derivatives( HermiteInitMode initMode, bool ascend_only );
        void	    _set_derivatives( const dvector& weight );
        void	    _new_derivative ( float& d_to_check, float d_other_side, float x1, float x2, float y1, float y2 );

    };



    ////////////////////////////////////////////////////////////////////////////////
    //
    //	C2 : public YrSpline
    //
    ////////////////////////////////////////////////////////////////////////////////

    enum C2_InitMode { setLeftRightDerivatives, leftRightDerivativesAutomatic };	// how to init C2

    class C2 : public YrSpline
    {
      public:
    		    C2		    ( void );
    		    ~C2		    ( void );
        void	    print	    ( void );
        float	    operator ()	    ( float );
        float	    operator ()	    ( float, int );
        //void	    inverse	    ( const float& in, multiset<float>& out );
        bool	    init	    (	const dvector&	    x,
    					const dvector&	    y,
    					const C2_InitMode&  initMode,
    					const float&	    left_gradient  = 0.0,
    					const float&	    right_gradient = 0.0
    				    );

      private:
        dvector	    _x;
        dvector	    _y;
        dvector	    _h;
        dvector	    _s;
        int		    _dim;
        bool	    _setup_complete;
        void	    _get_parameters ( const float& left_gradient, const float& right_gradient );
        void	    _get_parameters ( const C2_InitMode& initMode );

    };



    ////////////////////////////////////////////////////////////////////////////////
    //
    //	Linear : public YrSpline
    //
    ////////////////////////////////////////////////////////////////////////////////

    class Linear : public YrSpline
    {
      public:
    		    Linear	    ( void );
    		    ~Linear	    ( void );
        void	    print	    ( void );
        float	    operator ()	    ( float );
        float	    operator ()	    ( float, int );
        bool	    init	    ( const dvector& x, const dvector& y );

      private:
        dvector	    _x;
        dvector	    _y;
        int		    _dim;
        bool	    _setup_complete;

    };

    /* @} */

}
#endif // _Spline_h_
