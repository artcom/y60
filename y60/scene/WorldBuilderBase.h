/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
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

#include "y60_scene_settings.h"

#include "BuilderBase.h"

namespace y60 {

    class Y60_SCENE_DECL WorldBuilderBase : public BuilderBase {
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

