//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: GlurFilter.h,v 1.2 2004/11/12 13:50:01 janbo Exp $
//   $RCSfile: GlurFilter.h,v $
//   $Author: janbo $
//   $Revision: 1.2 $
//   $Date: 2004/11/12 13:50:01 $
//
//
//  Description:
//
//
//=============================================================================

#ifndef _GLUR_FILTER_h_
#define _GLUR_FILTER_h_

#include <y60/DataTypes.h>

#include <paintlib/plpaintlibdefs.h>
#include <paintlib/Filter/plfilter.h>

#include <vector>

class PLBmp;

class GlurFilter : public PLFilter {
public:
    GlurFilter(const y60::VectorOfFloat & theFactors);        
    virtual void Apply(PLBmpBase * theSource, PLBmp * theDestination) const;
private:
    int _myRadius;
    y60::VectorOfFloat theFactors;
    typedef std::vector<unsigned char> LookUpTable;
    LookUpTable _myLUT;
    LookUpTable _myAlphaLUT;
    asl::Vector4f _myColor;
    unsigned int  _myColorValue;
    unsigned char _myAlpha;
};

#endif
