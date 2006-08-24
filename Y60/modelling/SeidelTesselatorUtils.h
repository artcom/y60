//============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef AC_Y60_TESSELATION_UTILS_FUNCTIONS_INCLUDED
#define AC_Y60_TESSELATION_UTILS_FUNCTIONS_INCLUDED


namespace y60 {
    int math_logstar_n(int n);          
    int math_N(int n, int h);

    /* Node types */

    #define T_X     1
    #define T_Y     2
    #define T_SINK  3


    //#define SEGSIZE 200		/* max# of segments. Determines how */
				    /* many points can be specified as */
				    /* input. If your datasets have large */
				    /* number of points, increase this */
				    /* value accordingly. */

    //#define QSIZE   8*SEGSIZE	/* maximum table sizes */
    //#define TRSIZE  4*SEGSIZE	/* max# trapezoids */


    #define TRUE  1
    #define FALSE 0


    #define FIRSTPT 1		/* checking whether pt. is inserted */ 
    #define LASTPT  2


    #define INFINITY 1<<30
    #define C_EPS 1.0e-7		/* tolerance value: Used for making */
				    /* all decisions about collinearity or */
				    /* left/right of segment. Decrease */
				    /* this value if the input points are */
				    /* spaced very close together */


    #define S_LEFT 1		/* for merge-direction */
    #define S_RIGHT 2


    #define ST_VALID 1		/* for trapezium state */
    #define ST_INVALID 2


    #define SP_SIMPLE_LRUP 1	/* for splitting trapezoids */
    #define SP_SIMPLE_LRDN 2
    #define SP_2UP_2DN     3
    #define SP_2UP_LEFT    4
    #define SP_2UP_RIGHT   5
    #define SP_2DN_LEFT    6
    #define SP_2DN_RIGHT   7
    #define SP_NOSPLIT    -1	

    #define TR_FROM_UP 1		/* for traverse-direction */
    #define TR_FROM_DN 2

    #define TRI_LHS 1
    #define TRI_RHS 2


    #define MAX(a, b) (((a) > (b)) ? (a) : (b))
    #define MIN(a, b) (((a) < (b)) ? (a) : (b))

    #define CROSS(v0, v1, v2) (((v1).x - (v0).x)*((v2).y - (v0).y) - \
			    ((v1).y - (v0).y)*((v2).x - (v0).x))

    #define DOT(v0, v1) ((v0).x * (v1).x + (v0).y * (v1).y)

    #define FP_EQUAL(s, t) (fabs(s - t) <= C_EPS)

}
#endif // AC_Y60_TESSELATION_UTILS_FUNCTIONS_INCLUDED

