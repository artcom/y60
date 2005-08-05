//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
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

