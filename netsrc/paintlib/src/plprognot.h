/*
/--------------------------------------------------------------------
|
|      $Id: plprognot.h,v 1.3 2004/06/06 12:56:38 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLPROGNOT
#define INCL_PLPROGNOT

//! Defines an interface for progress notifications. This interface
//! can be implemented by client classes that wish to display progress
//! bars or similar.
class PLIProgressNotification
{
public:
  //! Called during decoding as progress gets made.
  virtual void OnProgress 
    ( double Part
    ) = 0;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: plprognot.h,v $
|      Revision 1.3  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.2  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|
\--------------------------------------------------------------------
*/
