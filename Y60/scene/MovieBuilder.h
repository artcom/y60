//============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//         $Id: MovieBuilder.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//    $RCSfile: MovieBuilder.h,v $
//     $Author: christian $
//   $Revision: 1.1 $
//       $Date: 2005/03/24 23:35:56 $
//
//  Description: MovieBuilder
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef _ac_MovieBuilder_h_
#define _ac_MovieBuilder_h_

//#include "BuilderBase.h"
#include "ImageBuilder.h"
#include <asl/Ptr.h>

namespace y60 {

    class MovieBuilder : public ImageBuilder {
        public:
            DEFINE_EXCEPTION(MovieBuilderException, asl::Exception);

            MovieBuilder(const std::string & theName,
                         const std::string & theFileName);
            virtual ~MovieBuilder();

            void setLoopCount(unsigned theLoopCount);
            //const std::string & getName() const;
        private:
    };

    typedef asl::Ptr<MovieBuilder>  MovieBuilderPtr;
}

#endif

