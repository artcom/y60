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
//
//   $Id: StlImport.h,v 1.2 2005/03/24 17:30:29 christian Exp $
//   $Author: christian $
//   $Revision: 1.2 $
//   $Date: 2005/03/24 17:30:29 $
//
// ASCII and binary STL importer.
//
//=============================================================================

#ifndef _ac_scene_StlImport_h_
#define _ac_scene_StlImport_h_

#include <y60/scene/ISceneDecoder.h>
#include <y60/scene/SceneBuilder.h>
#include <y60/scene/ElementBuilder.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <asl/base/PlugInBase.h>
#include <asl/dom/Nodes.h>
#include <y60/base/NodeNames.h>
#include <asl/base/Exception.h>
#include <asl/math/Vector234.h>
#include <asl/base/MappedBlock.h>
#include <asl/base/Logger.h>
#include <y60/base/VertexDataRoles.h>
#include <y60/scene/ShapeBuilder.h>


#include "JSStlCodec.h"

namespace y60 {

    const std::string MIME_TYPE_STL = "application/sla";

    /**
    * @ingroup y60StlImporter
    * Decoder for STL files.
    * @see DecoderManager
    */
    class StlImport :
        public ISceneDecoder,
        public asl::PlugInBase,
        public jslib::IScriptablePlugin
    {
    public:
        DEFINE_EXCEPTION(ImportException, asl::Exception);

        /**
        * Constructor.
        * @param theDLHandle Handle for Plugin Management
        */
        StlImport (asl::DLHandle theDLHandle) : _myGenerateNormal(false), PlugInBase(theDLHandle) {}
        ~StlImport() { }

        bool decodeScene(asl::Ptr<asl::ReadableStreamHandle> theSource, dom::DocumentPtr theScene);
        //bool encodeScene(const dom::DocumentPtr theScene, asl::WriteableStream * theStream);
        virtual bool setProgressNotifier(IProgressNotifierPtr theNotifier);
        std::string canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theSource);
        void initClasses(JSContext * theContext, JSObject *theGlobalObject);
        const char * ClassName() {
            static const char * myClassName = "StlPlugIn";
            return myClassName;
        }
        bool setLazy(bool lazy) {
            if (lazy) {
                AC_WARNING << "StlImport::decodeScene: lazy loading not supported";
            }
            return false;
        }
        bool addSource(asl::Ptr<asl::ReadableStreamHandle> theSource) {
            AC_WARNING << "StlImport::addSource: additional sources not supported";
            return false;
        }

    private:
        bool isLittleEndian(asl::ReadableStream & theSource) const;
        bool isBigEndian(asl::ReadableStream & theSource) const;
        bool _myGenerateNormal;
        IProgressNotifierPtr _myProgressNotifier;
        std::vector<dom::NodePtr> _myStlShapes;

        static asl::Vector3f generateNormal(const asl::Vector3f & v0,
            const asl::Vector3f & v1,
            const asl::Vector3f & v2);

        dom::NodePtr setupMaterial(const std::string & theBaseName,
            y60::SceneBuilder& sceneBuilder);

        template <class AC_BYTE_ORDER_LOCAL>
        unsigned int 
        readFileBinary(const asl::ReadableArrangedStream<AC_BYTE_ORDER_LOCAL> & theData,
                       asl::Unsigned32 theOffset, y60::SceneBuilder & sceneBuilder,
                       const std::string & materialId);
    };
} // namespace

#endif
