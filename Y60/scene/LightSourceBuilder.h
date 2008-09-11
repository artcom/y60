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
//   $Id: LightSourceBuilder.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: LightSourceBuilder.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//  Description: XML-File-Export Plugin
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef _ac_y60_LightSourceBuilder_h_
#define _ac_y60_LightSourceBuilder_h_

#include "BuilderBase.h"
#include <y60/base/NodeValueNames.h>
#include <asl/math/Vector234.h>

namespace y60 {

    class LightSourceBuilder : public BuilderBase {
        public:
            LightSourceBuilder(const std::string & theName);
            virtual ~LightSourceBuilder();
            void setAmbient    (const asl::Vector4f & theAmbient);
            void setDiffuse    (const asl::Vector4f & theDiffuse);
            void setSpecular   (const asl::Vector4f & theSpecular);
            void setAttenuation(const float & theAttenuation);
            void setType       (const LightSourceType theLightSourceType);
            void setSpotLight  (const float & theCutoff, const float & theExponent);
        private:
    };
    typedef asl::Ptr<LightSourceBuilder>  LightSourceBuilderPtr;

}
#endif
