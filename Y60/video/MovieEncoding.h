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
//   $Id: MovieEncoding.h,v 1.1 2005/03/24 23:36:04 christian Exp $
//   $RCSfile: MovieEncoding.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:36:04 $
//
//
//  Description: This file contains all the wellknown names for the
//               SOM
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef _ac_y60_MovieEncoding_h_
#define _ac_y60_MovieEncoding_h_

namespace y60 {
    /*! @addtogroup Y60video */
    /* @{ */

    enum MovieEncoding {
        MOVIE_RLE,
        MOVIE_DIFF,
        MOVIE_DRLE,
        INVALID_COMPRESSION_FORMAT
    };

    static const char * MovieEncodingString[] = {
        "RLE",
        "DIFF",
        "DRLE",
        0
    };

    /* @} */
}

#endif

