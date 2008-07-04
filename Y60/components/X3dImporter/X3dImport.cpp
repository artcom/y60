//=============================================================================
// Copyright (C) 2000-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include <iostream>
#include <fstream>
#include <map>

#include <asl/Logger.h>
#include <asl/file_functions.h>
#include <asl/string_functions.h>
#include <asl/GeometryUtils.h>
#include <y60/WorldBuilder.h>
#include <y60/CameraBuilder.h>
#include <y60/MaterialBuilder.h>
#include <y60/SceneBuilder.h>
#include <y60/ElementBuilder.h>
#include <y60/ShapeBuilder.h>
#include <y60/BodyBuilder.h>
#include <y60/TransformBuilder.h>

#include <y60/Shape.h>

#include <y60/property_functions.h>
#include <y60/PropertyNames.h>

#include "X3dImport.h"


/* Specs for X3D at
 * http://www.web3d.org/x3d/specifications/ISO-IEC-19775-IS-X3DAbstractSpecification/Part01/index.html
 */

extern "C"
EXPORT asl::PlugInBase* y60X3dImporter_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::X3dImport(myDLHandle);
}

namespace y60 {

    //only interpreted node types are defined here
    //unused node types are silenced out (comments)
    //still far from all are listed here...
    enum {
        UNKNOWN_X3D_NODE = 0,

        NAVIGATION_X3D_NODE,

        /* Grouping component  */

        GROUP_X3D_NODE,
        TRANSFORM_X3D_NODE,
        SWITCH_X3D_NODE,

        /* Navigation component */

        COLLISION_X3D_NODE,

        /* Shape component */

        SHAPE_X3D_NODE,
        APPEARANCE_X3D_NODE,
        MATERIAL_X3D_NODE,

        //FILLPROPERTIES_X3D_NODE,
        //LINEPROPERTIES_X3D_NODE,

        /* concrete X3DTextureNode from Texture component */

        IMAGETEXTURE_X3D_NODE,
        //MOVIETEXTURE_X3D_NODE,
        //MULTITEXTURE_X3D_NODE,
        //PIXELTEXTURE_X3D_NODE,

        TEXTURECOORDINATE_X3D_NODE,

        /* concrete X3DGeometryNode from Rendering component */

        COORDINATE_X3D_NODE,
        NORMAL_X3D_NODE,

        //INDEXEDLINESET_X3D_NODE,
        //INDEXEDTRIANGLEFANSET_X3D_NODE,
        //INDEXEDTRIANGLESET_X3D_NODE,
        //INDEXEDTRIANGLESTRIPSET_X3D_NODE,
        //LINESET_X3D_NODE,
        //POINTSET_X3D_NODE,
        //TRIANGLEFANSET_X3D_NODE,
        //TRIANGLESET_X3D_NODE,
        //TRIANGLESTRIPSET_X3D_NODE,

        /* concrete X3DGeometryNode from Geometry2D component */

        //ARC2D_X3D_NODE,
        //CIRCLE2D_X3D_NODE,
        //POLYLINE2D_X3D_NODE,
        //RECTANGLE2D_X3D_NODE,
        //TRIANGLESET2D_X3D_NODE,

        /* concrete X3DGeometryNode from Geometry3D component */

        //BOX_X3D_NODE,
        //CONE_X3D_NODE,
        //CYLINDER_X3D_NODE,
        //SPHERE_X3D_NODE,
        INDEXEDFACESET_X3D_NODE,

        END_X3D_NODE
    };

    //order *must* be the same as above
    static const char * ourX3DNodeNames[] = {
        "???",

        "NavigationInfo",
        "Group",
        "Transform",
        "Switch",

        "Collision",

        "Shape",
        "Appearance",
        "Material",

        //"FillProperties",
        //"LineProperties",

        "ImageTexture",
        //"MovieTexture",
        //"MultiTexture",
        //"PixelTexture",

        "TextureCoordinate",

        "Coordinate",
        "Normal",

        //"IndexedLineSet",
        //"IndexedLineSet",
        //"IndexedTriangleFanSet",
        //"IndexedTriangleSet",
        //"IndexedTriangleStripSet",
        //"LineSet",
        //"PointSet",
        //"TriangleFanSet",
        //"TriangleSet",
        //"TriangleStripSet"

        //"Box",
        //"Cone",
        //Cylinder",
        //Sphere",
        "IndexedFaceSet",

        ""
    };

    static std::map<const dom::DOMString, int> ourX3DNodeTypeMap;

    static void initX3DNodeMap() {
        for(int i = 0; i <  END_X3D_NODE; ++i) {
            ourX3DNodeTypeMap[ourX3DNodeNames[i]] = i;
        }
    }

    static const char * getX3DNodeName(int theNodeType) {
        return ourX3DNodeNames[theNodeType];
    }

    static int getX3DNodeType(const dom::DOMString & theNodeName) {
        if (ourX3DNodeTypeMap.size() == 0) {
            initX3DNodeMap();
        }
        /*
        for( std::map<dom::DOMString, int>::iterator i = ourX3DNodeTypeMap.begin();
             i != ourX3DNodeTypeMap.end(); ++i) {
            AC_TRACE << " map elem " << i->first << " : " << i->second;
        }
        */
        return (ourX3DNodeTypeMap.find(theNodeName) == ourX3DNodeTypeMap.end()) ?
                UNKNOWN_X3D_NODE : ourX3DNodeTypeMap[theNodeName];
    }

    template <class T>
    std::istream &
        operator >> (std::istream & theStream, asl::Vector3<T> & theVector) {
            theStream >> theVector[0];
            theStream >> theVector[1];
            theStream >> theVector[2];
            return theStream;
    }

    template <class T>
    std::istream &
        operator >> (std::istream & theStream, asl::Vector2<T> & theVector) {
            theStream >> theVector[0];
            theStream >> theVector[1];
            return theStream;
    }

    template <class T>
    std::istream &
        operator >> (std::istream & theStream, asl::Quaternion<T> & theQuaternion) {
            theStream >> theQuaternion[0];
            theStream >> theQuaternion[1];
            theStream >> theQuaternion[2];
            theStream >> theQuaternion[3];
            return theStream;
    }

    template <class T>
    bool readAttribute(const dom::NodePtr theNode, const std::string theName, T & theValue) {
        const dom::NodePtr myAttribute = theNode->getAttribute(theName);
        if (myAttribute) {
            std::stringstream myStream(myAttribute->nodeValue());
            myStream >> theValue;
            AC_TRACE << "read attr " << theName << " = " << theValue;
            return true;
        }
        return false;
    }

    /**********************************************************************
     *
     * Public
     *
     **********************************************************************/

    bool 
    X3dImport::setProgressNotifier(IProgressNotifierPtr theNotifier) {
        _myProgressNotifier = theNotifier;
        return true;
    }

    std::string
    X3dImport::canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theSource) {
        if (theSource) {
            asl::ReadableStream & theStream = theSource->getStream();
            if (theStream) {
                static const std::string myExampleHeader =
                    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
                    <!DOCTYPE X3D PUBLIC \"ISO//Web3D//DTD X3D 3.0//EN\" \"http://www.web3d.org/specifications/x3d-3.0.dtd\">\
                    <X3D profile=\"Immersive\">";

                std::string myXML;
                //lets say some exotic but valid X3D header wont be longer than twice myExampleHeader...
                theStream.readString(myXML, 2*myExampleHeader.size(), 0);
                if (myXML.find("<X3D") != std::string::npos) {
                    return MIME_TYPE_X3D;
                }
            }
            return "";
        }
        if (asl::toLowerCase(asl::getExtension(theUrl)) == "x3d") {
            return MIME_TYPE_X3D;
        }
        return "";
    }

    bool
    X3dImport::decodeScene(asl::Ptr<asl::ReadableStreamHandle> theStreamHandle, dom::DocumentPtr theScene) {
        asl::ReadableStream & theSource = theStreamHandle->getStream();
        asl::Time decodeStart;

        if (_myProgressNotifier) {
            _myProgressNotifier->onProgress(0, "loading");
        }
        std::string myXML;
        theSource.readString(myXML, theSource.size(), 0);

        //strip of the DOCTYPE line (not necessary anymore)
        /*
        const std::string DOCTYPE_BGN("<!DOCTYPE");
        const std::string DOCTYPE_END(">");
        int myStartDOCTYPE = myXML.find(DOCTYPE_BGN);
        int myEndDOCTYPE = myXML.find(DOCTYPE_END, myStartDOCTYPE) + DOCTYPE_END.size();
        if (myStartDOCTYPE >= 0 && myEndDOCTYPE > myStartDOCTYPE) {
            myXML = myXML.erase(myStartDOCTYPE, myEndDOCTYPE - myStartDOCTYPE + 1);
        }
        */

        _mySceneBuilder = y60::SceneBuilderPtr(new y60::SceneBuilder(theScene));

        _myWorldBuilder = y60::WorldBuilderPtr(new y60::WorldBuilder);
        _mySceneBuilder->appendWorld(*_myWorldBuilder);

        y60::CameraBuilder myCameraBuilder("X3DCamera");
        _myWorldBuilder->appendObject(myCameraBuilder);
        const std::string myCameraId = myCameraBuilder.getNode()->getAttribute("id")->nodeValue();
        _mySceneBuilder->setInitialCamera(myCameraId);

        //myCameraBuilder.setHFov(45.0);
        //myCameraBuilder.setPosition(asl::Vector3f(0,0,-100));

        // Insert a 'Main' Transform above all X3D stuff
        y60::TransformBuilderPtr myTransformBuilder(new y60::TransformBuilder("Main"));
        _myWorldBuilder->appendObject(*myTransformBuilder);
        _myCurrentTransform = &*myTransformBuilder;

        dom::Document myDom(myXML);
        const dom::Node & myX3dScene = myDom("X3D")("Scene");
        _myNodesToProcess = getDeepNodeCount(myX3dScene);
        _myNodesProcessed = 0;
        for( int i = 0; i < myX3dScene.childNodesLength(); ++i) {
            processNode(myX3dScene.childNode(i));
            countNodesProcessed(1);
        }
        if (_myProgressNotifier) {
            _myProgressNotifier->onProgress(1.0f);
        }

        asl::Time decodeEnd;
        AC_INFO  << "Parse time X3D " << decodeEnd -decodeStart << " sec.";

        return true;
    }

    /**********************************************************************
     *
     * Private
     *
     **********************************************************************/


    unsigned 
    X3dImport::getDeepNodeCount(const dom::Node & theNode) const {
        unsigned myNodeCount = 1;
        unsigned myChildCount = theNode.childNodesLength();
        for (int i = 0; i < myChildCount; ++i) {
            myNodeCount += getDeepNodeCount(*(theNode.childNode(i)));
        }
        return myNodeCount;
    }
   
    void
    X3dImport::countNodesProcessed(unsigned theCount) {
        _myNodesProcessed += theCount;
        if (_myProgressNotifier) {
            _myProgressNotifier->onProgress(float(_myNodesProcessed)/float(_myNodesToProcess));
        }
    }
    // dispatch the processing of a node
    // we should be here if the node could not be interpreted by its parent
    void
    X3dImport::processNode(const dom::NodePtr theNode) {
        //AC_DEBUG  << "analyzing node " << theNode->nodeName();

        switch ( getX3DNodeType(theNode->nodeName()) ) {
            case GROUP_X3D_NODE:
            case COLLISION_X3D_NODE:
                processGroupNode(theNode);
                break;

            case TRANSFORM_X3D_NODE:
                processTransformNode(theNode);
                break;

            case SWITCH_X3D_NODE:
                processSwitchNode(theNode);
                break;

            case SHAPE_X3D_NODE:
                processShapeNode(theNode);
                break;
            case NAVIGATION_X3D_NODE:
				{AC_INFO << "ignoring x3d node type " << theNode->nodeName();}
                break;
            case UNKNOWN_X3D_NODE:
            default:
				{AC_INFO << "unknown x3d node type " << theNode->nodeName();}
        }
    }


    // a Group node is just that: a grouping mechanism
    // it is also base node for many other, e.g. Transform node
    void
    X3dImport::processGroupNode(const dom::NodePtr theNode) {
        AC_DEBUG  << ">>> start grouping " << theNode->nodeName();
        for( int i = 0; i < theNode->childNodesLength(); ++i) {
            countNodesProcessed(1);
            processNode( theNode->childNode(i) );
        }
        AC_DEBUG  << "<<< end grouping " << theNode->nodeName();
    }


    // a Transform node
    void
    X3dImport::processTransformNode(const dom::NodePtr theTransformNode) {
        AC_DEBUG  << "processing Transform node";
        std::string myTransformName("mX3D");
        y60::TransformBuilder myTransformBuilder(myTransformName);

        //insert a child transform in the transform hierarchy
        _myCurrentTransform->appendObject(myTransformBuilder);

        //make the child transform current
        y60::WorldBuilderBase * myParentTransform = _myCurrentTransform;
        _myCurrentTransform = & myTransformBuilder;


        //center / pivot
        asl::Vector3f myPivot(0,0,0);
        readAttribute(theTransformNode, "center", myPivot);
        myTransformBuilder.setPivot(myPivot);

        //TODO scale-rotation

        //scale
        asl::Vector3f myScale(1,1,1);
        readAttribute(theTransformNode, "scale", myScale);
        myTransformBuilder.setScale(myScale);

        //rotation / orientation
        asl::Quaternionf myOrientation(0,0,0,1);
        readAttribute(theTransformNode, "rotation", myOrientation);
        myTransformBuilder.setOrientation(myOrientation);

        //translation / position
        asl::Vector3f myPosition(0,0,0);
        readAttribute(theTransformNode, "translation", myPosition);
        myTransformBuilder.setPosition(myPosition);

        //process the descendants
        processGroupNode(theTransformNode);

        //go back to the parent transform
        _myCurrentTransform = myParentTransform;
    }

    /* a Switch node
     * the attribute whichChoice determines which of the childNodes is active
     * Note: typically this attribute is dynamic and changed by e.g. mouse events
     *       we do not support that and take the initial value as final
     */
    void
    X3dImport::processSwitchNode(const dom::NodePtr theSwitchNode) {
        AC_DEBUG  << "processing Switch node";
        int  myChoice = asl::as_int( (*theSwitchNode)["whichChoice"].nodeValue() );
        AC_DEBUG << "taking choice " << myChoice << " from " << theSwitchNode->childNodesLength();
        processNode( theSwitchNode->childNode(myChoice) );
        countNodesProcessed(theSwitchNode->childNodesLength());
    }

    // a Shape node should contain one Appearance node and must contain one Geometry node
    void
    X3dImport::processShapeNode(const dom::NodePtr theShapeNode) {
        AC_DEBUG  << "processing Shape node";

        std::string myMaterialId;

        //make sure we process the appearance node first to get the materialId
        for (int i = 0; i < theShapeNode->childNodesLength(); ++i) {
            dom::NodePtr myChildNode = theShapeNode->childNode(i);
            if (myChildNode->nodeName() == getX3DNodeName(APPEARANCE_X3D_NODE) ) {
                processAppearanceNode(myChildNode, myMaterialId);
                countNodesProcessed(1);
            }
        }

        for (int i = 0; i < theShapeNode->childNodesLength(); ++i) {
            dom::NodePtr myChildNode = theShapeNode->childNode(i);
            if (myChildNode->nodeName() != getX3DNodeName(APPEARANCE_X3D_NODE) ) {
                processGeometryNode(myChildNode, myMaterialId);
                countNodesProcessed(1);
            }
        }
    }

    //TODO for now appearance nodes always create a new material :-(
    void
    X3dImport::processAppearanceNode(const dom::NodePtr theAppearanceNode,
                                          std::string & theMaterialId)
    {
        AC_DEBUG  << "processing Appearance node";

        const std::string myMaterialName("mX3D");

        //dont inline the textures, cause we don't have any path where to look for them
        y60::MaterialBuilder myMaterialBuilder(myMaterialName, false);
        theMaterialId = _mySceneBuilder->appendMaterial(myMaterialBuilder);

        for (int i = 0; i < theAppearanceNode->childNodesLength(); ++i) {
            dom::NodePtr myChildNode = theAppearanceNode->childNode(i);

            switch ( getX3DNodeType(myChildNode->nodeName()) ) {

                case MATERIAL_X3D_NODE:
                    processMaterialNode(myChildNode, myMaterialBuilder);
                    break;

                case IMAGETEXTURE_X3D_NODE:
                    processImageTextureNode(myChildNode, myMaterialBuilder);
                    break;

                default:
                    AC_INFO << " appearance child node " << theAppearanceNode->nodeName() << " unknown. ";
            }
            countNodesProcessed(1);
        }

        myMaterialBuilder.computeRequirements();
    }


    void
    X3dImport::processMaterialNode(const dom::NodePtr theMaterialNode,
                                        y60::MaterialBuilder & theMaterialBuilder)
    {
        AC_DEBUG  << "processing Material node";

        asl::Vector4f myOpaqueWhite(1.0, 1.0, 1.0, 1.0);
        y60::setPropertyValue<asl::Vector4f>(theMaterialBuilder.getNode(),
              "vector4f", y60::AMBIENT_PROPERTY, myOpaqueWhite);

        y60::setPropertyValue<asl::Vector4f>(theMaterialBuilder.getNode(),
              "vector4f", y60::DIFFUSE_PROPERTY, myOpaqueWhite);

        y60::VectorOfRankedFeature myLighting;
        y60::createLightingFeature(myLighting, y60::LAMBERT);
        theMaterialBuilder.setType(myLighting);
        //...StlImporter
    }

    void
    X3dImport::processImageTextureNode(const dom::NodePtr theImageTextureNode,
                                            y60::MaterialBuilder & theMaterialBuilder)
    {
        AC_DEBUG  << "processing Image Texture node";
        //TODO intepret X3D fields repeatS repeatT

        //TODO (multiple) URL are given ?
        const std::string & myImageURL = (*theImageTextureNode)["url"].nodeValue();
        int myFirstStart = myImageURL.find("\"")+1;
        int myFirstEnd = myImageURL.find("\"",myFirstStart);
        std::string myImageName = myImageURL.substr(myFirstStart,myFirstEnd-myFirstStart);
        std::string myTextureName("tex" + std::string("X3D"));

        AC_DEBUG  << "Image URL " << myImageURL << " importing " << myImageName;

        dom::NodePtr myImageNode = theMaterialBuilder.createImageNode(*_mySceneBuilder, 
                                           myTextureName, myImageName, y60::PAINT,
                                           y60::SINGLE);

        //X3D texture coords start at the BOTTOM left of the image
        asl::Matrix4f myTextureTransform;
        myTextureTransform.makeIdentity();
        myTextureTransform.setTranslation(asl::Vector3f(0.0,1.0,0.0));
        myTextureTransform.setScale(asl::Vector3f(1.0,-1.0,1.0));

        theMaterialBuilder.createTextureNode(*_mySceneBuilder,
                myTextureName, myImageNode->getAttributeString(ID_ATTRIB), 
                y60::REPEAT, false, myTextureTransform);
    }

    // a Geometry node is an abstract node.. this func dispatches accordingly
    void
    X3dImport::processGeometryNode(const dom::NodePtr theNode,
                                        const std::string & theMaterialId)
    {
        AC_DEBUG  << "processing Geometry node";

        switch ( getX3DNodeType(theNode->nodeName()) ) {

            case INDEXEDFACESET_X3D_NODE:
                processIndexedFaceSetNode(theNode, theMaterialId);
                break;

            default:
                AC_INFO << " geometry node " << theNode->nodeName()
                        << " unknown. ";
        }
    }


    /* A face in X3D is any arbitrary (2D) polygon
     * for now we assume a list of triangles,
     * TODO we should at least support quads here
     */
    void
    X3dImport::processIndexedFaceSetNode(const dom::NodePtr theIndexedFaceNode,
                                              const std::string & theMaterialId)
    {
        AC_DEBUG  << "processing Indexed Face Set node";

        const char *myPrimitiveType = PrimitiveTypeStrings[TRIANGLES];

        std::string myShapeName("sX3D");
        y60::ShapeBuilder myShapeBuilder(myShapeName);
        _mySceneBuilder->appendShape(myShapeBuilder);

        y60::ElementBuilder myElementBuilder(myPrimitiveType, theMaterialId);
        myShapeBuilder.appendElements(myElementBuilder);

        bool myCCWFlag = true;
        const dom::NodePtr myCCWField = theIndexedFaceNode->getAttribute("ccw");
        if (myCCWField && myCCWField->nodeValue() == "false") {
            myCCWFlag = false;
        }

        /*
         * the face coordinate indices
         */

        const dom::NodePtr myCoordIndexField = theIndexedFaceNode->getAttribute("coordIndex");
        unsigned myFaceCount = createIndices(myElementBuilder, myPrimitiveType,
                                             myCoordIndexField->nodeValue(), y60::POSITIONS, myCCWFlag);

        /*
         * the vertex coordinates
         */

        const std::string & myCoordinates
            = (*theIndexedFaceNode)( getX3DNodeName(COORDINATE_X3D_NODE) )["point"].nodeValue();

        unsigned myVertexCount
            = createVertices<asl::Vector3f>(myShapeBuilder, myCoordinates, y60::POSITIONS);

        /*
         * the normal coordinates
         */

        const dom::NodePtr myNormalNode
            = theIndexedFaceNode->childNode( getX3DNodeName(NORMAL_X3D_NODE) );
        unsigned myNormalCount = 0;
        if (myNormalNode) {
            AC_DEBUG  << "found vertex normals";
            myNormalCount = createVertices<asl::Vector3f>(myShapeBuilder,
                                                          (*myNormalNode)["vector"].nodeValue(),
                                                          y60::NORMALS);

            dom::NodePtr myNormalIndexField = theIndexedFaceNode->getAttribute("normalIndex");
            if ( ! myNormalIndexField) {
                //use myCoordIndexField
                myElementBuilder.copyIndexBin(y60::POSITIONS, y60::NORMALS, y60::NORMAL_ROLE);
            } else {
                createIndices(myElementBuilder, myPrimitiveType,
                        myNormalIndexField->nodeValue(), y60::NORMALS, myCCWFlag);
            }

        } else {
            // generate face normals (currently for triangles only)
            AC_DEBUG  << "generating face normals for " << myFaceCount << " faces";

            myShapeBuilder.createVertexDataBin<asl::Vector3f>(y60::NORMAL_ROLE, myVertexCount);
            myElementBuilder.createIndex(y60::NORMAL_ROLE, y60::NORMALS);

            dom::NodePtr myPositionsNode
                = myShapeBuilder.getNode()->childNode(y60::VERTEX_DATA_NAME)->childNode(0);
            const y60::VectorOfVector3f & myPositions
                = myPositionsNode->childNode("#text")->nodeValueRef<y60::VectorOfVector3f>();
            dom::NodePtr myElementNode
                = myShapeBuilder.getNode()->childNode(y60::PRIMITIVE_LIST_NAME)->childNode(0);
            const y60::VectorOfUnsignedInt & myPositionIndices
                = myElementNode->childNode("indices")->childNode("#text")->nodeValueRef<y60::VectorOfUnsignedInt>();
            
            for(int i = 0, p = 0;i < myFaceCount; ++i) {
                const asl::Vector3f & myPositionA(myPositions[myPositionIndices[p++]]);
                const asl::Vector3f & myPositionB(myPositions[myPositionIndices[p++]]);
                const asl::Vector3f & myPositionC(myPositions[myPositionIndices[p++]]);
                
                // AC_DEBUG << "positions are " << myPositionA << "," << myPositionB << ","  << myPositionC; 
                asl::Vector3f myNormal(asl::generateFaceNormal<float>(myPositionA, myPositionC, myPositionB));
                // AC_DEBUG << "added normal #"  << i << " = " << myNormal;
                myShapeBuilder.appendVertexData(y60::NORMAL_ROLE, myNormal);

                myElementBuilder.appendIndex(y60::NORMALS, i);
                myElementBuilder.appendIndex(y60::NORMALS, i);
                myElementBuilder.appendIndex(y60::NORMALS, i);
            }
            // AC_INFO << *(myShapeBuilder.getNode());
        }

        /*
         * the texture coordinates
         */

        const dom::NodePtr myTextureCoordinateNode
            = theIndexedFaceNode->childNode( getX3DNodeName(TEXTURECOORDINATE_X3D_NODE) );
        unsigned myTextureCoordinateCount = 0;
        if (myTextureCoordinateNode) {
            myTextureCoordinateCount
                = createVertices<asl::Vector2f>(myShapeBuilder,
                                                (*myTextureCoordinateNode)["point"].nodeValue(),
                                                y60::getTextureRole(0));

            dom::NodePtr myTexCoordIndexField = theIndexedFaceNode->getAttribute("texCoordIndex");
            if ( ! myTexCoordIndexField) {
                myElementBuilder.copyIndexBin(y60::POSITIONS, y60::getTextureRole(0),
                        asl::getStringFromEnum(y60::getTextureRole(0), y60::VertexDataRoleString));
            } else {
                createIndices(myElementBuilder, myPrimitiveType,
                        myTexCoordIndexField->nodeValue(), y60::getTextureRole(0), myCCWFlag);
            }
        }

        AC_INFO  << "#### processed " << myFaceCount << " faces | "
                 << myVertexCount << " vertices | "
                 << myNormalCount << " normals | "
                 << myTextureCoordinateCount << " texturecoordinates "
                 << (myCCWFlag ? "" : "| faces inverted ")
                 << " ####";


        //add body
        const std::string myShapeId
            = myShapeBuilder.getNode()->getAttribute("id")->nodeValue();
        std::string myBodyName("bX3D");
        y60::BodyBuilder myBodyBuilder(myShapeId, myBodyName);
        _myCurrentTransform->appendObject(myBodyBuilder);
        countNodesProcessed(theIndexedFaceNode->childNodesLength());
    }

    /*
     * this function reads in a vector of T from the string theVertices
     * and creates vertex data in the shape builder
     * the vector values are comma separated
     * and at the end there may or may not be a comma (and whitespace)
     */
    template <class T>
    unsigned
    X3dImport::createVertices(y60::ShapeBuilder & theShapeBuilder,
                                       const std::string & theVertices,
                                       const y60::VertexDataRole & theRole,
                                       const unsigned theVertexCount)
    {

        std::stringstream myStream(theVertices);
        const std::string & myRole = asl::getStringFromEnum(theRole, y60::VertexDataRoleString);

        // UH: the y60::ShapeBuilder:: prefix is necessary for gcc to work
        theShapeBuilder.y60::ShapeBuilder::createVertexDataBin<T>(myRole, theVertexCount);
        int myVertexCount = 0;
        while ( ! myStream.eof() ) {
            T myVertex;
            myStream >> myVertex;
            if ( myStream.fail()) {
                break;
            }

            theShapeBuilder.appendVertexData(myRole, myVertex);
            ++myVertexCount;

            std::string mySep;
            myStream >> mySep;
            if (mySep != "" && mySep != ",") {
                throw ImportException("### ERROR while reading Vector3", PLUS_FILE_LINE);
            }
        }
        if (theVertexCount > 0 && theVertexCount != myVertexCount) {
            throw ImportException("### ERROR vertex count was " + asl::as_string(myVertexCount)
                      + " but expected " + asl::as_string(theVertexCount), PLUS_FILE_LINE);
        }
        return myVertexCount;
    }


    //helper for createIndices()
    static inline void appendIndexElement(y60::ElementBuilder & theElementBuilder,
                                          const std::vector<unsigned> & theElement,
                                          unsigned theElementVertexCount,
                                          const y60::VertexDataRole & theRole, bool theCCWFlag)
    {
        if (theElement.size() != theElementVertexCount) {
            throw X3dImport::ImportException("#### ERROR X3D Face with "
                + asl::as_string(theElement.size())
                + " vertices found but expected " + asl::as_string(theElementVertexCount),
                PLUS_FILE_LINE);
        }
        std::vector<unsigned>::const_iterator myIndex;
        if (theCCWFlag) {
            for(myIndex = theElement.begin(); myIndex != theElement.end(); ++myIndex) {
                theElementBuilder.appendIndex(theRole, *myIndex);
            }
        } else {
            myIndex = theElement.end();
            while ( myIndex-- != theElement.begin() ) {
                theElementBuilder.appendIndex(theRole, *myIndex);
            }
        }
    }

    /*
     * read in a vector of indices from the string theIndices
     * the indices represent a set of primitives
     * each primitive is a (small) vector of indices (triangles have three indices)
     * primitive vectors are -1 separated
     * at the end there may or may not be a -1
     * primitive indices are given either in CCW or non-CCW direction
     * if they are non-CCW we reverse their order (see appendIndexElement() )
     *
     * currently only primitive type triangle is supported
     */
    unsigned X3dImport::createIndices(y60::ElementBuilder & theElementBuilder,
                                      const char * thePrimitiveType,
                                      const std::string & theIndices,
                                      const y60::VertexDataRole & theRole,
                                      bool theCCWFlag)
    {
        unsigned myElementVertexCount;
        if (thePrimitiveType == PrimitiveTypeStrings[TRIANGLES]) {
            myElementVertexCount = 3;
        } else {
            throw ImportException("#### ERROR Primitive type " + asl::as_string(thePrimitiveType)
                      + " not supported ", PLUS_FILE_LINE);
        }

        theElementBuilder.createIndex(asl::getStringFromEnum(theRole, y60::VertexDataRoleString),
                                      theRole);

        std::stringstream myStream(theIndices);
        int myFaceVertexCount = 0;
        int myFaceCount = 0;
        std::vector<unsigned> myElement(myElementVertexCount);
        while ( ! myStream.eof() ) {
            int myCoordIndex;
            myStream >> myCoordIndex;
            if (myCoordIndex != -1) {
                myElement[myFaceVertexCount++] = myCoordIndex;
                //AC_TRACE  << " index " << myCoordIndex;

            } else {
                appendIndexElement(theElementBuilder, myElement, myElementVertexCount,
                                   theRole, theCCWFlag);
                myFaceVertexCount = 0;
                ++myFaceCount;
            }
        }
        if (myFaceVertexCount > 0) {
            appendIndexElement(theElementBuilder,  myElement, myElementVertexCount,
                               theRole, theCCWFlag);
            ++myFaceCount;
        }
        return myFaceCount;
    }

}



