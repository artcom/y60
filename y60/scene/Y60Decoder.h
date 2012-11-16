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
//   $RCSfile: $
//
//   $Author: $
//
//   $Revision: $
//
//=============================================================================

#ifndef _ac_scene_Y60Decoder_h_
#define _ac_scene_Y60Decoder_h_

#include "y60_scene_settings.h"

#include "ISceneDecoder.h"
#include <asl/base/Exception.h>

namespace y60 {

    DEFINE_EXCEPTION(Y60DecodeException, asl::Exception);

    const std::string MIME_TYPE_X60 = "model/x60";
    const std::string MIME_TYPE_B60 = "model/b60";
    const std::string MIME_TYPE_D60 = "model/d60";
/**
 * @ingroup Y60scene
 * Decoder for y60 files (x60, b60)
 */
class Y60Decoder : public ISceneDecoder {
    public:
        virtual bool decodeScene(asl::Ptr<asl::ReadableStreamHandle>, dom::DocumentPtr theScene);
        /**
         * checks if the given file can be decoded
         * @param theUrl URL to read from
         * @param theStream stream to read from
         * @todo implement support for streamed b60
         * @retval MIME_TYPE_X60 This file is an xml x60 file and can be decoded
         * @retval MIME_TYPE_B60 This file is an binary b60 file and can be decoded
         * @return "" This file cannot be decoded.
         */
        virtual std::string canDecode(const std::string & theUrl,asl::Ptr<asl::ReadableStreamHandle> theStream = asl::Ptr<asl::ReadableStreamHandle>());
        virtual bool setProgressNotifier(IProgressNotifierPtr theNotifier);
        virtual bool setLazy(bool theFlag);
        virtual bool addSource(asl::Ptr<asl::ReadableStreamHandle> theSource);
        virtual bool getLazy() const {
            return _myLazyMode;
        };
    private:
        void loadXmlFile(asl::ReadableStream & theXmlSource, bool theBinaryFlag);
        void removeComments(dom::NodePtr theNode);
        std::vector<asl::Ptr<asl::ReadableStreamHandle> > _mySources;
        bool _myLazyMode;
};

typedef asl::Ptr<Y60Decoder> Y60DecoderPtr;
}

#endif // _ac_scene_Y60Decoder_h_

