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

#include <y60/ISceneDecoder.h>
#include <y60/SceneBuilder.h>
#include <y60/ElementBuilder.h>
#include <y60/IScriptablePlugin.h>
#include <asl/PlugInBase.h>
#include <dom/Nodes.h>
#include <y60/NodeNames.h>
#include <asl/Exception.h>
#include <asl/Vector234.h>
#include <asl/MappedBlock.h>
#include <asl/Logger.h>
#include <y60/VertexDataRoles.h>
#include <y60/ShapeBuilder.h>


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
