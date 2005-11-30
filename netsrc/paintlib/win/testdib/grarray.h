/*
/--------------------------------------------------------------------
|
|      $Id: grarray.h,v 1.4 2002/03/31 13:36:42 uzadow Exp $
|      Array of CGrItems
|
|      Contains an array of graphic items sorted by z-order.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_GRARRAY
#define INCL_GRARRAY

#include "gritem.h"

class CGrItemArray : public CObArray
{

DECLARE_DYNAMIC (CGrItemArray);
// Construction
public:
  CGrItemArray
    ();

  virtual ~CGrItemArray
    ();
    // Deletes the array & all elements in it.

  CGrItem * GetItem
    ( int i
    );
    // Returns item at the i'th position in the array.

  void Empty
    ();
    // Destroys all elements in the array.

  virtual int AddItem
    ( CGrItem * pItem,
      int Pos = -1
    );
    // Type-safe version of CObArray::Add. If the parameter Pos is
    // omitted, the element is inserted in z-Order, otherwise at
    // position Pos. (Specifying Pos can lead to an unsorted array
    // and should only be used when you're inserting in sorted order
    // anyway.)

  void Draw
    ( PLBmp * pCanvas,
      CRect * pRect
    );
    // Draws all the elements on the canvas by invoking their Draw()-
    // methods.

  void GetRect
    ( CRect * pRect
    );
    // Returns smallest rectangle containing all the elements.

private:
  int sortedInsert
    ( CGrItem * pItem
    );
};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: grarray.h,v $
|      Revision 1.4  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.3  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|
--------------------------------------------------------------------
*/
