/*
/--------------------------------------------------------------------
|
|      $Id: plhsvconvert.h,v 1.2 2002/03/31 13:36:42 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/
#ifndef HSVCONVERT_H
#define HSVCONVERT_H

void fp_rgb_to_hsv (double *r,
                    double *g,
                    double *b);

void fp_hsv_to_rgb (double *h,
                    double *s,
                    double *v);

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plhsvconvert.h,v $
|      Revision 1.2  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.1  2000/10/12 21:56:12  uzadow
|      Moved local functions from VideoInvertFilter.cpp to
|      hsvconvert.*
|
|
|
\--------------------------------------------------------------------
*/
