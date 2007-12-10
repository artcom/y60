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
        ResizePolicyTag::Plug(theNode)
#ifdef BAD_TX
,
        dom::FacadeAttributePlug<TextureUnitTexturePtrTag>(this),
        dom::FacadeAttributePlug<TextureUnitCombinerPtrTag>(this)
#endif
    {
        AC_TRACE << "TextureUnit::TextureUnit " << (void*) this;
    }

    TextureUnit::~TextureUnit() {
        AC_TRACE << "TextureUnit::~TextureUnit " << (void*) this;
#ifdef BAD_TX
//TODO: unreference texture
        TexturePtr myTexture = getTexture();
        if (myTexture) {
            myTexture->unrefTexture();
        }
#endif
    }

    void
    TextureUnit::registerDependenciesRegistrators() {
        Facade::registerDependenciesRegistrators();
#ifdef BAD_TX
        TextureUnitTexturePtrTag::Plug::setReconnectFunction(&TextureUnit::registerDependenciesForTextureTag);
        TextureUnitCombinerPtrTag::Plug::setReconnectFunction(&TextureUnit::registerDependenciesForCombinerTag);
#endif
    }

    void 
    TextureUnit::setTextureManager(const TextureManagerPtr theTextureManager) {
        _myTextureManager = theTextureManager;
    }

    TexturePtr
    TextureUnit::getTexture() const {
#ifdef BAD_TX
        return get<TextureUnitTexturePtrTag>().lock();
#else
       const std::string & myTextureId = get<TextureUnitTextureIdTag>();
       TexturePtr myTexture = _myTextureManager->findTexture(myTextureId);
        if (!myTexture) {
            AC_WARNING << "TextureUnit::updateTexture textureunit references invalid texture id=" << myTextureId;
            return TexturePtr(0);
        }
        return myTexture;
#endif
    }

    ICombinerPtr
    TextureUnit::getCombiner() const {
#ifdef BAD_TX
        return get<TextureUnitCombinerPtrTag>().lock();
#else
#endif
    }
#ifdef BAD_TX
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
#endif

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
#ifdef BAD_TX
        set<TextureUnitTexturePtrTag>(TextureWeakPtr(myTexture));
#endif
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
