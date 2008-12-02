#include <asl/base/Logger.h>
#include "StlCodec.h"
#include <asl/base/Assure.h>

using namespace std;
using namespace asl;
using namespace dom;

namespace y60 {

    StlCodec::StlCodec(asl::Ptr<asl::ResizeableBlock> theBlock) : _myBlock(theBlock) {
        _myOutputMode = BLOCK;
    }

    StlCodec::StlCodec(std::string theFilename, bool theBigEndianFlag) {
        if (theBigEndianFlag) {
            _myOutputMode = BIGENDIAN;
            _myBigStream = asl::Ptr<WriteableArrangedFile<X86ByteOrder> >(new WriteableArrangedFile<X86ByteOrder>(theFilename));
        } else {
            _myOutputMode = LITTLEENDIAN;
            _myLittleStream = asl::Ptr<WriteableArrangedFile<PowerPCByteOrder> >(new WriteableArrangedFile<PowerPCByteOrder>(theFilename));
        }
    }

    StlCodec::~StlCodec() {
        _myLittleStream = asl::Ptr<WriteableArrangedFile<PowerPCByteOrder> >(0);
        _myBigStream = asl::Ptr<WriteableArrangedFile<X86ByteOrder> >(0);
        AC_DEBUG << "StlCodec destroyed.";
    }

    template <class AC_BYTE_ORDER_LOCAL> void 
    StlCodec::exportHeader(const std::string & theName, unsigned theNumShapes, WriteableArrangedStream<AC_BYTE_ORDER_LOCAL> & theStream) {
        // Append 80 chars of entity name
        std::string myEntityName;
        myEntityName.assign(theName, 0, theName.length() > HEADER_SIZE ? HEADER_SIZE : theName.length());
        myEntityName.append(HEADER_SIZE-myEntityName.length(), '\0');
        ASSURE(myEntityName.length() == HEADER_SIZE);
        theStream.appendString(myEntityName);
        // Append number of facets
        theStream.appendSigned32(theNumShapes);
    }

    template <class AC_BYTE_ORDER_LOCAL> void 
    StlCodec::exportFacet(const StlFacet & theFacet, WriteableArrangedStream<AC_BYTE_ORDER_LOCAL> & theStream) {
        theStream.appendFloat32(theFacet.normal[0]);
        theStream.appendFloat32(theFacet.normal[1]);
        theStream.appendFloat32(theFacet.normal[2]);
        for (unsigned i = 0; i < 3; ++i) {
            theStream.appendFloat32(theFacet.vertex[i][0]);
            theStream.appendFloat32(theFacet.vertex[i][1]);
            theStream.appendFloat32(theFacet.vertex[i][2]);
        }
        theStream.appendSigned16(0);
    }

    template <class AC_BYTE_ORDER_LOCAL> void 
    StlCodec::exportShapeToStream(const ShapePtr theShape, WriteableArrangedStream<AC_BYTE_ORDER_LOCAL> & theStream, bool theWriteHeadersFlag) {
        const PrimitiveVector & myPrimitiveVector = theShape->getPrimitives();        
        std::string myName = theShape->getNode().nodeName();
        for (unsigned i = 0; i < myPrimitiveVector.size(); ++i) {
            const Primitive & myPrimitive = (*myPrimitiveVector[i]);
            const PrimitiveType & myType = myPrimitive.get<PrimitiveTypeTag>();
            switch (myType) {
            case TRIANGLES: {
                asl::Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = myPrimitive.getConstLockingPositionsAccessor();
                asl::Ptr<ConstVertexDataAccessor<Vector3f> > myNormalsAccessor = myPrimitive.getConstLockingNormalsAccessor();
                const VertexData3f & myVertexData = myPositionAccessor->get();
                const VertexData3f & myNormalData = myNormalsAccessor->get();
                int myVSize = myVertexData.size();
                int myNSize = myNormalData.size();
                ASSURE(myVSize % 3 == 0);
                ASSURE(myNSize == myVSize);
                unsigned myNumFaces = myVSize / 3;
                if (theWriteHeadersFlag) {
                    AC_TRACE << "Exporting header";
                    exportHeader(myName + "_" + as_string(i), myNumFaces, theStream);
                }
                AC_TRACE << "Exporting triangles";
                for (unsigned myFaceIndex = 0; myFaceIndex < myNumFaces; ++myFaceIndex) {
                    StlFacet myFacet;
                    int myStartVertex = myFaceIndex*3;
                    // Compute Face normal by summing up the vertex normals.
                    myFacet.normal = myNormalData[myStartVertex] + myNormalData[myStartVertex+1] + myNormalData[myStartVertex+2];
                    myFacet.normal.normalize();
                    myFacet.vertex[0] = myVertexData[myStartVertex];
                    myFacet.vertex[1] = myVertexData[myStartVertex+1];
                    myFacet.vertex[2] = myVertexData[myStartVertex+2];
                    exportFacet(myFacet, theStream);
                }
                               }
                break;
            case QUADS: {
                asl::Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = myPrimitive.getConstLockingPositionsAccessor();
                asl::Ptr<ConstVertexDataAccessor<Vector3f> > myNormalsAccessor = myPrimitive.getConstLockingNormalsAccessor();
                const VertexData3f & myVertexData = myPositionAccessor->get();
                const VertexData3f & myNormalData = myNormalsAccessor->get();
                int myVSize = myVertexData.size();
                int myNSize = myNormalData.size();
                ASSURE(myVSize % 4 == 0);
                ASSURE(myNSize == myVSize);
                unsigned myNumFaces = myVSize / 4;
                exportHeader(myName + "_" + as_string(i), myNumFaces*2, theStream);
                AC_TRACE << "Exporting triangles";
                for (unsigned myFaceIndex = 0; myFaceIndex < myNumFaces; ++myFaceIndex) {
                    StlFacet myFacet;
                    int myStartVertex = myFaceIndex*4;
                    // Compute Face normal by summing up the vertex normals.
                    myFacet.normal = myNormalData[myStartVertex] + myNormalData[myStartVertex+1] + myNormalData[myStartVertex+2];
                    myFacet.normal.normalize();
                    myFacet.vertex[0] = myVertexData[myStartVertex];
                    myFacet.vertex[1] = myVertexData[myStartVertex+1];
                    myFacet.vertex[2] = myVertexData[myStartVertex+2];
                    exportFacet(myFacet, theStream);
                    myFacet.vertex[0] = myVertexData[myStartVertex];
                    myFacet.vertex[1] = myVertexData[myStartVertex+2];
                    myFacet.vertex[2] = myVertexData[myStartVertex+3];
                    exportFacet(myFacet, theStream);
                }
                           }
                break;
            default:
                AC_ERROR << "Not yet implemented: Stl-Export of " << PrimitiveTypeStrings[myType] << ". Ignoring Primitive";
                break;
            }
            //const VertexDataBase & myVertexData = myPrimitive.getVertexData(y60::POSITIONS);
            //asl::Ptr<ConstVertexDataAccessor<Vector3f> > myAccessor = myVertexData.getConstLockingPositionsAccessor();
            //ConstVertexDataAccessor<Vector3f> & myAccessor = myVertexData.getVertexDataAccessor();
        } // for
    } // StlImport::exportShape

    void 
    StlCodec::exportShape(const dom::NodePtr theNode) {
        ShapePtr myShape = theNode->getFacade<Shape>();
        if (myShape) {
            switch (_myOutputMode) {
                case BIGENDIAN:
                    exportShapeToStream(myShape, *_myBigStream);
                    break;
                case LITTLEENDIAN:
                    exportShapeToStream(myShape, *_myLittleStream);
                    break;
                case BLOCK:
                    exportShapeToStream(myShape, *_myBlock);
                    break;
            }
        } else {
            AC_ERROR << "Not a shape or not a filename while exporting.";
        }
    }

    void 
    StlCodec::exportShapes(const dom::NodePtr theNode, const std::vector<std::string> & theIds) {
        //int myNumChildren = theNode->childNodesLength();
        int myNumFaces = 0;
        // First get the total amount of triangles
        for (std::vector<std::string>::size_type j = 0; j < theIds.size(); ++j) {
            dom::NodePtr myNode = theNode->getChildElementById(theIds[j], "id");
            if (myNode) {
                ShapePtr myShape = myNode->getFacade<Shape>();
                const PrimitiveVector & myPrimitiveVector = myShape->getPrimitives();
                for (unsigned i = 0; i < myPrimitiveVector.size(); ++i) {
                    const Primitive & myPrimitive = (*myPrimitiveVector[i]);
                    const PrimitiveType & myType = myPrimitive.get<PrimitiveTypeTag>();
                    asl::Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = myPrimitive.getConstLockingPositionsAccessor();
                    asl::Ptr<ConstVertexDataAccessor<Vector3f> > myNormalsAccessor = myPrimitive.getConstLockingNormalsAccessor();
                    const VertexData3f & myVertexData = myPositionAccessor->get();
                    int myVSize = myVertexData.size();
                    switch (myType) {
                    case TRIANGLES:
                        myNumFaces += myVSize / 3;
                        break;
                    case QUADS: 
                        myNumFaces += myVSize / 2;
                        break;
                    default:
                        AC_WARNING << "Can't STL-Export Primitives of different types than GL_TRIANGLES and GL_QUADS";
                        break;
                    }
                }
            }
        }
        std::string myName("EXPORT");
        // Export a header
        switch (_myOutputMode) {
            case BIGENDIAN:
                exportHeader(myName, myNumFaces, *_myBigStream);
                break;
            case LITTLEENDIAN:
                exportHeader(myName, myNumFaces, *_myLittleStream);
                break;
            case BLOCK:
                exportHeader(myName, myNumFaces, *_myBlock);
                break;
        }
        // export shapes
        for (std::vector<std::string>::size_type j = 0; j < theIds.size(); ++j) {
            dom::NodePtr myNode = theNode->getChildElementById(theIds[j], "id");
            if (myNode) {
                ShapePtr myShape = myNode->getFacade<Shape>();
                switch (_myOutputMode) {
                    case BIGENDIAN:
                        exportShapeToStream(myShape, *_myBigStream, false);
                        break;
                    case LITTLEENDIAN:
                        exportShapeToStream(myShape, *_myLittleStream, false);
                        break;
                    case BLOCK:
                        exportShapeToStream(myShape, *_myBlock, false);
                        break;
                }
            } 
        } 
    }


    void
    StlCodec::close() {
        _myLittleStream = asl::Ptr<WriteableArrangedFile<PowerPCByteOrder> >(0);
        _myBigStream = asl::Ptr<WriteableArrangedFile<X86ByteOrder> >(0);
    }
}

