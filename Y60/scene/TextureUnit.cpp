//=============================================================================
// Copyright (C) 1993-`date +%Y`, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "TextureUnit.h"

#include <y60/NodeNames.h>
#include <asl/string_functions.h>
#include <asl/Logger.h>


using namespace asl;

namespace y60 {

    TextureUnit::TextureUnit(dom::Node & theNode) : Facade(theNode),
        TextureUnitTextureIdTag::Plug(theNode),
        TextureUnitApplyModeTag::Plug(theNode),
        TextureUnitCombinerTag::Plug(theNode),
        TextureUnitEnvColorTag::Plug(theNode),
        TextureUnitSpriteTag::Plug(theNode),
		TextureUnitMatrixTag::Plug(theNode),
        TextureUnitProjectorIdTag::Plug(theNode),
        ResizePolicyTag::Plug(theNode),
        dom::FacadeAttributePlug<TextureUnitTexturePtrTag>(this),
        dom::FacadeAttributePlug<TextureUnitCombinerPtrTag>(this)
    {
        AC_TRACE << "TextureUnit::TextureUnit " << (void*) this;
    }

    TextureUnit::~TextureUnit() {
        AC_TRACE << "TextureUnit::~TextureUnit " << (void*) this;
        TexturePtr myTexture = getTexture();
        if (myTexture) {
            myTexture->unrefTexture();
        }
    }

    void
    TextureUnit::registerDependenciesRegistrators() {
        Facade::registerDependenciesRegistrators();
        TextureUnitTexturePtrTag::Plug::setReconnectFunction(&TextureUnit::registerDependenciesForTextureTag);
        TextureUnitCombinerPtrTag::Plug::setReconnectFunction(&TextureUnit::registerDependenciesForCombinerTag);
    }

    void 
    TextureUnit::setTextureManager(const TextureManagerPtr theTextureManager) {
        _myTextureManager = theTextureManager;
    }

    TexturePtr
    TextureUnit::getTexture() const {
        return get<TextureUnitTexturePtrTag>().lock();
    }

    ICombinerPtr
    TextureUnit::getCombiner() const {
        return get<TextureUnitCombinerPtrTag>().lock();
    }

    void
    TextureUnit::registerDependenciesForTextureTag() {
        TextureUnitTexturePtrTag::Plug::dependsOn<TextureUnitTextureIdTag>(*this);  
        TextureUnitTexturePtrTag::Plug::getValuePtr()->setCalculatorFunction(dynamic_cast_Ptr<TextureUnit>(getSelf()), &TextureUnit::updateTexture);
    }

    void
    TextureUnit::registerDependenciesForCombinerTag() {        
        TextureUnitCombinerPtrTag::Plug::dependsOn<TextureUnitCombinerTag>(*this);  
        TextureUnitCombinerPtrTag::Plug::getValuePtr()->setCalculatorFunction(dynamic_cast_Ptr<TextureUnit>(getSelf()), &TextureUnit::updateCombiner);
    }

    void
    TextureUnit::updateTexture() {

        TexturePtr myTexture = getTexture();
        if (myTexture) {
            myTexture->unrefTexture();
        }

        const std::string & myTextureId = get<TextureUnitTextureIdTag>();
        myTexture = _myTextureManager->findTexture(myTextureId);
        if (!myTexture) {
            AC_WARNING << "TextureUnit::updateTexture textureunit references invalid texture id=" << myTextureId;
            return;
        }
        myTexture->refTexture();
        set<TextureUnitTexturePtrTag>(TextureWeakPtr(myTexture));
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
