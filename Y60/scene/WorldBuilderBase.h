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
//   $Id: WorldBuilderBase.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: WorldBuilderBase.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//  Description: XML-File-Export Plugin
//
//
//=============================================================================

#ifndef _ac_Y60_WorldBuilderBase_h_
#define _ac_Y60_WorldBuilderBase_h_

#include "BuilderBase.h"

namespace y60 {

    class WorldBuilderBase : public BuilderBase {
        public:
            WorldBuilderBase(const std::string & theNodeName);

            const std::string & appendObject(WorldBuilderBase & theBuilder);

            void setAssimilateFlag() {
                _myAssimilateFlag = true;
            }
            bool getAssimilateFlag() const {
                return _myAssimilateFlag;
            }
            void setVisiblity(bool theVisibleFlag);
            bool getVisiblity() const;

            WorldBuilderBase(const dom::NodePtr & theNode);
        private:
            bool _myAssimilateFlag;
    };

    typedef asl::Ptr<WorldBuilderBase> WorldBuilderBasePtr;
}

#endif

