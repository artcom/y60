/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-97, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: integrate_function.h,v $
//
//   $Revision: 1.1 $
//
//      Author: Axel Kilian
//
// Description: 
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_integrate_function
#define _included_asl_integrate_function

namespace asl {
    
    ////////////////////////////////////////////////////////////////////////////////
    //
    //    
    //	#    #    #   #####  ######   ####   #####     ##     #####  ######
    //	#    ##   #     #    #       #    #  #    #   #  #      #    #
    //	#    # #  #     #    #####   #       #    #  #    #     #    #####
    //	#    #  # #     #    #       #  ###  #####   ######     #    #
    //	#    #   ##     #    #       #    #  #   #   #    #     #    #
    //	#    #    #     #    ######   ####   #    #  #    #     #    ######
    //
    //
    //   estimate the integral of fun(x) from a to b 
    //   to a user provided tolerance. 
    //   an automatic adaptive routine based on 
    //   the 8-panel newton-cotes rule. 
    //
    //
    //
    //   INPUT .. 
    //
    //   fun     the name of the integrand function subprogram fun(x). 
    //   a       the lower limit of integration. 
    //   b       the upper limit of integration.(b may be less than a.) 
    //   relerr  a relative error tolerance. (should be non-negative) 
    //   abserr  an absolute error tolerance. (should be non-negative) 
    //
    //
    //
    //   OUTPUT .. 
    //
    //   result  an approximation to the integral hopefully satisfying the 
    //           least stringent of the two error tolerances. 
    //   errest  an estimate of the magnitude of the actual error. 
    //   nofun   the number of function values used in calculation of result. 
    //   flag    a reliability indicator.  if flag is zero, then result 
    //           probably satisfies the error tolerance.  if flag is 
    //           xxx.yyy , then  xxx = the number of intervals which have 
    //           not converged and  0.yyy = the fraction of the interval 
    //           left to do when the limit on  nofun  was approached. 
    //
    ////////////////////////////////////////////////////////////////////////////////
    
    
    inline int power ( int base,  int exponent )
    {
        int retval = 1;
        int i;
        
        
        for ( i=0; i<exponent; i++ ) {
    	retval *= base;
        }
        
        return retval;
    }
    
    
    template<class ddfunction>
    int
    integrate (
        ddfunction	    fun, 
        const float&   a,
        const float&   b,
        const float&   abserr,
        const float&   relerr,
        float&	    result,
        float&	    errest,
        int&	    nofun,
        float&	    flag_   )
    {
    
    
        // System generated locals (from f2c)
        float d__1, d__2;
    
        // Local variables 
        float area, cor11, temp, step, qnow, f[16];
        int i, j;
        float x[16], qdiff, fsave[240];		    // was [8][30]
        int nofin;
        float qleft;
        int nomax;
        float f0, xsave[240];			    // was [8][30];
        float stone, qprev, w0, w1, w2, w3, w4, x0;
        int levmin, levmax;
        float qright[31], esterr, tolerr;
        int levout, nim, lev;
    
    
    
    //------------------------------------------------------------------------------
    //   ***   stage 1 ***   general initialization 
    //------------------------------------------------------------------------------
    
    //   set constants. 
    
        levmin = 1;
        levmax = 30;
        levout = 6;
        nomax = 5000;
        nofin = nomax - (levmax - levout + (power ( 2, levout + 1 ) << 3));
        //nofin = nomax - (levmax - levout + (power ( 2, levout + 1 ) << 3));
    
    //   trouble when nofun reaches nofin 
    
        w0 = .27908289241622575f;
        w1 = 1.6615167548500882f;
        w2 = -.26186948853615521f;
        w3 = 2.9618342151675483f;
        w4 = -1.2811287477954145f;
    
    //   initialize running sums to zero. 
    
        flag_ = 0.;
        result = 0.;
        cor11 = 0.;
        errest = 0.;
        area = 0.;
        nofun = 0;
        if (a == b) {
    	return 0;
        }
    
    //------------------------------------------------------------------------------
    //   ***   stage 2 ***   initialization for first interval 
    //------------------------------------------------------------------------------
    
        lev = 0;
        nim = 1;
        x0 = a;
        x[15] = b;
        qprev = 0.;
        f0 = fun (x0);
        stone = (b - a) / 16.f;
        x[7] = (x0 + x[15]) / 2.f;
        x[3] = (x0 + x[7]) / 2.f;
        x[11] = (x[7] + x[15]) / 2.f;
        x[1] = (x0 + x[3]) / 2.f;
        x[5] = (x[3] + x[7]) / 2.f;
        x[9] = (x[7] + x[11]) / 2.f;
        x[13] = (x[11] + x[15]) / 2.f;
        for (j = 2; j <= 16; j += 2) {
    	f[j - 1] = fun (x[j - 1]);
        }
        nofun = 9;
    
    //------------------------------------------------------------------------------
    //   ***   stage 3 ***   central calculation 
    //------------------------------------------------------------------------------
    
    //   requires qprev,x0,x2,x4,...,x16,f0,f2,f4,...,f16. 
    //   calculates x1,x3,...x15, f1,f3,...f15,qleft,qright,qnow,qdiff,area. 
    
    L30:
        x[0] = (x0 + x[1]) / 2.f;
        f[0] = fun (x[0]);
        for (j = 3; j <= 15; j += 2) {
    	x[j - 1] = (x[j - 2] + x[j]) / 2.f;
    	f[j - 1] = fun (x[j - 1]);
        }
        nofun += 8;
        step = (x[15] - x0) / 16.f;
        qleft = (w0 * (f0 + f[7]) + w1 * (f[0] + f[6]) + w2 * (f[1] + f[5]) + w3 *
    	     (f[2] + f[4]) + w4 * f[3]) * step;
        qright[lev] = (w0 * (f[7] + f[15]) + w1 * (f[8] + f[14]) + w2 * (f[9] + f[
    	    13]) + w3 * (f[10] + f[12]) + w4 * f[11]) * step;
        qnow = qleft + qright[lev];
        qdiff = qnow - qprev;
        area += qdiff;
    
    //------------------------------------------------------------------------------
    //   ***   stage 4 *** interval convergence test 
    //------------------------------------------------------------------------------
    
        esterr = fabs(qdiff) / 1023.f;
    
    // Computing MAX tolerr
        d__1 = abserr;
        d__2 = relerr * fabs(area);
        tolerr = ( d__1 > d__2 ) ? d__1 : d__2;
        tolerr *= step / stone;
        if (lev < levmin) {
    	goto L50;
        }
        if (lev >= levmax) {
    	goto L62;
        }
        if (nofun > nofin) {
    	goto L60;
        }
        if (esterr <= tolerr) {
    	goto L70;
        }
    
    
    //------------------------------------------------------------------------------
    //   ***   stage 5   ***   no convergence 
    //------------------------------------------------------------------------------
    
    //   locate next interval. 
    
    L50:
        nim <<= 1;
        ++lev;
    
    //   store right hand elements for future use. 
    
        for (i = 1; i <= 8; ++i) {
    	fsave[i + (lev << 3) - 9] = f[i + 7];
    	xsave[i + (lev << 3) - 9] = x[i + 7];
        }
    
    //   assemble left hand elements for immediate use. 
    
        qprev = qleft;
        for (i = 1; i <= 8; ++i) {
    	j = -i;
    	f[(j << 1) + 17] = f[j + 8];
    	x[(j << 1) + 17] = x[j + 8];
    // L55: 
        }
        goto L30;
    
    //------------------------------------------------------------------------------
    //   ***   stage 6   ***   trouble section 
    //------------------------------------------------------------------------------
    
    //   number of function values is about to exceed limit. 
    
    L60:
        nofin <<= 1;
        levmax = levout;
        flag_ += (b - x0) / (b - a);
        goto L70;
    
    //   current level is levmax. 
    
    L62:
        flag_ += 1.;
    
    //------------------------------------------------------------------------------
    //   ***   stage 7   ***   interval converged 
    //------------------------------------------------------------------------------
    
    //   add contributions into running sums. 
    
    L70:
        result += qnow;
        errest += esterr;
        cor11 += qdiff / 1023.f;
    
    //   locate next interval. 
    
    L72:
        if (nim == nim / 2 << 1) {
    	goto L75;
        }
        nim /= 2;
        --lev;
        goto L72;
    L75:
        ++nim;
        if (lev <= 0) {
    	goto L80;
        }
    
    //   assemble elements required for the next interval. 
    
        qprev = qright[lev - 1];
        x0 = x[15];
        f0 = f[15];
        for (i = 1; i <= 8; ++i) {
    	f[(i << 1) - 1] = fsave[i + (lev << 3) - 9];
    	x[(i << 1) - 1] = xsave[i + (lev << 3) - 9];
        }
        goto L30;
    
    //------------------------------------------------------------------------------
    //   ***   stage 8   ***   finalize and return 
    //------------------------------------------------------------------------------
    
    L80:
        result += cor11;
    
    //   make sure errest not less than roundoff level. 
    
        if (errest == 0.) {
    	return 0;
        }
    L82:
        temp = fabs(result) + errest;
        if (temp != fabs(result)) {
    	return 0;
        }
        errest *= 2.;
        goto L82;
        
    }
    
    
    template<class T>
    T derivative ( const T f1, const T f2, const float dt ) { return (f2-f1) / dt; }
    
    template<class ddfunction>
    bool
    integrate ( ddfunction f, const float& a, const float& b, float& result )
    {
    
    
        float  abserr = 1.0e-11f;
        float  relerr = 1.0e-11f;
        float  errest;
        int	    nofun;
        float  flag;
        bool    retval;
    
        integrate ( f, a, b, abserr, relerr, result, errest, nofun, flag );
        
        retval = flag==0.0 ? true : false;
            
        return retval;
    }
}
#endif // _included_asl_integrate_function
