#include <asl/Logger.h>
#include "StlCodec.h"
#include <asl/Assure.h>

using namespace std;
using namespace asl;
using namespace dom;

namespace y60 {

    StlCodec::StlCodec(std::string theFilename, bool theBigEndianFlag) : _myBigEndianFlag(theBigEndianFlag) {
        if (theBigEndianFlag) {
            _myBigStream = asl::Ptr<WriteableArrangedFile<X86ByteOrder> >(new WriteableArrangedFile<X86ByteOrder>(theFilename));
        } else {
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
        myEntityName.append(HEADER_SIZE-myEntityName.length(), ' ');
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
        theStream.appendString(std::string("  "));
    }

    template <class AC_BYTE_ORDER_LOCAL> void 
    StlCodec::exportShapeToStream(const ShapePtr theShape, WriteableArrangedStream<AC_BYTE_ORDER_LOCAL> & theStream) {
        const PrimitiveVector & myPrimitiveVector = theShape->getPrimitives();        
        std::string myName = theShape->getNode().nodeName();
        for (unsigned i = 0; i < myPrimitiveVector.size(); ++i) {
            const Primitive & myPrimitive = (*myPrimitiveVector[i]);
            const PrimitiveType & myType = myPrimitive.getType();
            switch (myType) {
            case GL_TRIANGLES: {
                Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = myPrimitive.getConstLockingPositionsAccessor();
                Ptr<ConstVertexDataAccessor<Vector3f> > myNormalsAccessor = myPrimitive.getConstLockingNormalsAccessor();
                const VertexData3f & myVertexData = myPositionAccessor->get();
                const VertexData3f & myNormalData = myNormalsAccessor->get();
                int myVSize = myVertexData.size();
                int myNSize = myNormalData.size();
                ASSURE(myVSize % 3 == 0);
                ASSURE(myNSize == myVSize);
                unsigned myNumFaces = myVSize / 3;
                exportHeader(myName + "_" + as_string(i), myNumFaces, theStream);
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
            case GL_QUADS: {
                Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = myPrimitive.getConstLockingPositionsAccessor();
                Ptr<ConstVertexDataAccessor<Vector3f> > myNormalsAccessor = myPrimitive.getConstLockingNormalsAccessor();
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
                AC_ERROR << "Not yet implemented: Stl-Export of " << PrimitiveTypeString[myType] << ". Ignoring Primitive";
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
        if (_myBigEndianFlag) {
            if (myShape && _myBigStream) {
                exportShapeToStream(myShape, *_myBigStream);
            } else {
                AC_ERROR << "Not a shape or not a filename while exporting.";
            }
        } else {
            if (myShape && _myLittleStream) {
                exportShapeToStream(myShape, *_myLittleStream);
            } else {
                AC_ERROR << "Not a shape or not a filename while exporting.";
            }
        }
    }
    
    void
    StlCodec::close() {
        _myLittleStream = asl::Ptr<WriteableArrangedFile<PowerPCByteOrder> >(0);
        _myBigStream = asl::Ptr<WriteableArrangedFile<X86ByteOrder> >(0);
    }
}

