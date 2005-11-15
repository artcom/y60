//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: DecoderManager.h,v $
//   $Author: pavel $
//   $Revision: 1.2 $
//   $Date: 2005/04/24 00:41:17 $
//
//
//=============================================================================

#ifndef _y60_decoder_manager_included_
#define _y60_decoder_manager_included_

#include "IDecoder.h"

#include <asl/Logger.h>
#include <asl/Singleton.h>

#define DB(x) //x

namespace y60 {

    /**
      DecoderManager holds decoders of all types and finds the (hopefully) correct one for a given ReadableStream.
      <ol>
      <li>Register your decoder by calling addDecoder()
      <li>Find a decoder <i>of a specific type</i> by calling findDecoder<DECODERTYPE>()
      <br>
          e.g. Scene Decoders:
      <br>
               y60::StlImport & y60::X3dImport are specific decoders derived from y60::ISceneDecoder.
               We use findDecoder<ISceneDecoder>() in Scene::load() to decode a scene accordingly.

      </ol>
      */
class DecoderManager : public asl::Singleton<DecoderManager> {
    public:
        DecoderManager() {};
        virtual ~DecoderManager() {};
        void addDecoder(IDecoderPtr theDecoder);

        /**
        * Trys to find a decoder, that accepts the url/stream.
        * @param theUrl can be used to check for known file extensions.
        * @param theStream should also be provided if possible, to check for magic numbers, etc.
        * @return The first decoder, that accapts the given url/stream.
        */
        template <class DECODERTYPE>
        asl::Ptr<DECODERTYPE> findDecoder(const std::string & theUrl, asl::ReadableStream * theSource = 0) {
            for (int i = 0; i < _myDecoders.size(); ++i) {
                AC_TRACE << "checking decoder #" << i;
                asl::Ptr<DECODERTYPE> myDecoder = dynamic_cast_Ptr<DECODERTYPE>(_myDecoders[i]);
                if (myDecoder) {
                    std::string myMimeType = myDecoder->canDecode(theUrl, theSource);
                    AC_TRACE << "decoder returned '" << myMimeType << "'";
                    if (!myMimeType.empty()) {
                        return myDecoder;
                    }
                }
            }
            return asl::Ptr<DECODERTYPE>(0);
        }
    private:
        std::vector<IDecoderPtr> _myDecoders;
};


}

#undef DB

#endif
