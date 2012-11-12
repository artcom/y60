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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "TextureUnit.h"

#include <y60/base/NodeNames.h>
#include <asl/base/string_functions.h>
#include <asl/base/Logger.h>


using namespace asl;

namespace y60 {

    TextureUnit::TextureUnit(dom::Node & theNode)
        : Facade(theNode),
        TextureUnitTextureIdTag::Plug(theNode),
        TextureUnitProjectorIdTag::Plug(theNode),
        TextureUnitApplyModeTag::Plug(theNode),
        TextureUnitCombinerTag::Plug(theNode),
        TextureUnitEnvColorTag::Plug(theNode),
        TextureUnitSpriteTag::Plug(theNode),
		TextureUnitMatrixTag::Plug(theNode),
        ResizePolicyTag::Plug(theNode)
    {
        AC_TRACE << "TextureUnit::TextureUnit " << (void*) this;
    }

    TextureUnit::~TextureUnit() {
        AC_TRACE << "TextureUnit::~TextureUnit " << (void*) this;
    }

    void
    TextureUnit::registerDependenciesRegistrators() {
        Facade::registerDependenciesRegistrators();
    }

    void
    TextureUnit::setTextureManager(const TextureManagerPtr theTextureManager) {
        _myTextureManager = theTextureManager;
    }

    TexturePtr
    TextureUnit::getTexture() const {
       const std::string & myTextureId = get<TextureUnitTextureIdTag>();
       TexturePtr myTexture = _myTextureManager->findTexture(myTextureId);
        if (!myTexture) {
            AC_WARNING << "TextureUnit::updateTexture textureunit references invalid texture id=" << myTextureId;
            return TexturePtr();
        }
        return myTexture;
    }

    ICombinerPtr
    TextureUnit::getCombiner() const {
        return ICombinerPtr();
    }

    void
    TextureUnit::updateCombiner() {

        std::string myCombinerName = get<TextureUnitCombinerTag>();
        AC_TRACE << "TextureUnit::updateCombiner combiner=" << myCombinerName;

        /*CombinerPtr myCombiner;
        //myCombiner = _myTextureManager->findCombiner(myCombinerName);
        if (!myCombiner) {
            AC_WARNING << "TextureUnit::updateTexture textureunit references invalid combiner name=" << myCombinerName;
            return;
        }
        set<TextureUnitCombinerPtrTag>(CombinerWeakPtr(myCombiner));
        */
    }
}
