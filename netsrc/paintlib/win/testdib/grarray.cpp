/*
/--------------------------------------------------------------------
|
|      $Id: grarray.cpp,v 1.4 2002/03/31 13:36:42 uzadow Exp $
|      Array of CGrItems
|
|      Contains an array of graphic items sorted by z-order. Deletes
|      elements when done.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "stdafx.h"
#include "grarray.h"
#include "gritem.h"

IMPLEMENT_DYNAMIC (CGrItemArray, CObArray);


CGrItemArray::CGrItemArray
    ()
{
}


CGrItemArray::~CGrItemArray ()
{
  Empty();
}


CGrItem * CGrItemArray::GetItem
    ( int i
    )
{
  return (CGrItem *) GetAt (i);
}


void CGrItemArray::Empty
    ()
{
  int i;

  for (i=0; i<GetSize(); i++)
  {
    delete GetAt(i);
  }
  RemoveAll();
}


int CGrItemArray::AddItem
    ( CGrItem * pItem,
      int Pos
    )
    // Typsichere Version von Add. Fügt Element an korrekter Position
    // ein. Falls Pos nicht angegeben wird, wird das Element nach
    // z-Order sortiert eingefügt.
{
  if (Pos == -1)
    return sortedInsert (pItem);
  else
  {
    InsertAt (Pos, pItem);
    return Pos;
  }
}



void CGrItemArray::Draw
    ( PLBmp * pCanvas,
      CRect * pRect
    )
{
  int i;

  for (i=0; i<GetSize(); i++)
    GetItem(i)->Draw (pCanvas, pRect);
}


void CGrItemArray::GetRect
    ( CRect * pRect
    )
{
  int i;
  CRect ItemRect;

  *pRect = CRect (0,0,0,0);

  for (i=0; i<GetSize(); i++)
  {
    GetItem(i)->GetRect (&ItemRect);
    pRect->UnionRect (pRect, &ItemRect);
  }
}


int CGrItemArray::sortedInsert
    ( CGrItem * pItem
    )
{
  int i;
  int z = pItem->m_z;
  for (i=0; i<GetSize(); i++)
  {
    if (GetItem(i)->m_z > z)
    {
      InsertAt (i, pItem);
      return i;
    }
  }
  return Add (pItem);
}
/*
/--------------------------------------------------------------------
|
|      $Log: grarray.cpp,v $
|      Revision 1.4  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.3  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|
--------------------------------------------------------------------
*/
