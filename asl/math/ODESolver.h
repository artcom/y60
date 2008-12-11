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
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//    $RCSfile: ODESolver.h,v $
//
//     $Author: david $
//
//   $Revision: 1.3 $
//
// Description: Line class
//
//=============================================================================

#ifndef _included_asl_ODESolver_
#define _included_asl_ODESolver_

namespace asl {	

    /*! @addtogroup aslmath */
    /* @{ */

    template <class TIME, class Number>
    class ODE {
        Number (*Function)(TIME t, const Number & x);
    };

    // Solve system dx/dt = F(t,x)

    template<class TIME, class Number, class FUNCTION>
    EulerSolver {
        Number integrate(TIME t, TIME dt, const Number & x) {
            return x + FUNCTION(t, x) * dt;
        };
    };

    template<class TIME, class Number, class FUNCTION>
    RungeKutta4Solver {
        Number integrate(TIME t, TIME dt, const Number & x) {
            Number fx1 = FUNCTION(t, x);
            Number xo = x + fx1 * (dt/2);

            Number fx2 = FUNCTION(t + dt/2, xo);
            xo = x + fx2 * (dt/2);

            Number fx3 = FUNCTION(t + dt/2, xo);
            xo = x + fx3 * dt;

            Number fx4 = FUNCTION(t + dt, xo);
            xo = x + fx3 * dt;
            return x + (fx1 + (fx2 + fx3) * 2 + fx4) * (dt/6);
        };
    };

    /* @} */
}
//----------------------------------------------------------------------------
#endif

