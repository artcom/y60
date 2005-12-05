//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Primitive.h,v $
//   $Author: martin $
//   $Revision: 1.58 $
//   $Date: 2005/04/21 15:07:04 $
//
//  Description: 
//
//=============================================================================

#ifndef _ac_scene_Primitive_h_
#define _ac_scene_Primitive_h_

#include "VertexData.h"
#include "MaterialBase.h"
#include "ResourceManager.h"

// defined in VertexData.h
#ifdef _USE_GFX_MEM_ //DK this looks broken to me!
#include "GLAlloc.h"
#endif


#ifdef WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN 1
    #endif
    #include <windows.h>
    #undef max
#endif

#ifdef LINUX
// we now retrieve the prototypes using glXGetProcAddressARB
//    #ifndef GL_GLEXT_PROTOTYPES
//    #define GL_GLEXT_PROTOTYPES 1
//    #endif
#endif

#include <GL/gl.h>

#include <y60/DataTypes.h>
#include <asl/Exception.h>
#include <asl/contact.h>
#include <dom/Nodes.h>
#include <asl/FixedVector.h>
#include <asl/numeric_functions.h>

#include <vector>

namespace y60 {

    DEFINE_EXCEPTION(MaterialFault, asl::Exception);
    DEFINE_EXCEPTION(UnknownDataSource, asl::Exception);
    
    enum PrimitiveType {
        POINTS         = GL_POINTS,
        LINES          = GL_LINES,
        LINE_STRIP     = GL_LINE_STRIP,
        LINE_LOOP      = GL_LINE_LOOP,
        TRIANGLES      = GL_TRIANGLES,
        TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
        TRIANGLE_FAN   = GL_TRIANGLE_FAN,
        QUADS          = GL_QUADS,
        QUAD_STRIP     = GL_QUAD_STRIP,
        POLYGON        = GL_POLYGON,
    };

    static const char * PrimitiveTypeString[] = {
        "POINTS",
        "LINES",
        "LINE_STRIP",
        "LINE_LOOP",
        "TRIANGLES",
        "TRIANGLE_STRIP",
        "TRIANGLE_FAN",
        "QUADS",
        "QUAD_STRIP",
        "POLYGON",
        0
    };
    
    inline
    unsigned
    getVerticesPerPrimitive(PrimitiveType theType) {
        switch (theType) {
            case POINTS:
                return 1;
            case LINES:
            case LINE_STRIP:
                return 2;
            case TRIANGLES:
            case TRIANGLE_STRIP:
            case TRIANGLE_FAN:
                return 3;
            case QUADS:
            case QUAD_STRIP:
                return 4;
            default:
                return 0;
        }
    }
    
#ifdef _USE_GFX_MEM_
    typedef std::vector<asl::Vector3f, asl::GLAlloc<asl::Vector3f> > GfxVectorOfVector3f;
    typedef std::vector<asl::Vector4f, asl::GLAlloc<asl::Vector4f> > GfxVectorOfVector4f;
#else
    typedef std::vector<asl::Vector3f> GfxVectorOfVector3f;
    typedef std::vector<asl::Vector4f> GfxVectorOfVector4f;
#endif
    struct BoundingBoxTree;
    typedef asl::Ptr<BoundingBoxTree> BoundingBoxTreePtr;

    /**
     * @ingroup y60scene
     * A list of elements described by their vertexdata. An element is
     * for example a triangle, quad or tringle strip, line, ...
     */
    class Primitive {
        public:
            struct Element {
                Primitive * _myPrimitive;
                unsigned int _myStartVertex;
                unsigned int _myVertexCount;
                PrimitiveType _myType;
            };
            struct Intersection {
                Element _myElement;
                asl::Point3f _myPosition;
                asl::Vector3f _myNormal;
            };
            struct SphereContacts {
                Element _myElement;
                int _myNumberOfContacts;
                asl::SweptSphereContact<float> _myMinContact;
                asl::SweptSphereContact<float> _myMaxContact;
            };

            typedef std::vector<Primitive::Intersection> IntersectionList;
            typedef std::vector<Primitive::SphereContacts> SphereContactsList;
    
            Primitive(PrimitiveType theType, y60::MaterialBasePtr theMaterial, 
                      const std::string & theShapeId,
                      unsigned int theDomIndex = 0);
            
            ~Primitive();

            unsigned int getDomIndex() const { 
                return _myDomIndex; 
            }
           
            /**
             * Loads a Primitive from the indexnode @p theIndicesNode and the data node
             * @p theDataNode.
             * @param theResourceManager ResourceManager for creation of the VertexData
             * @param theIndicesNode Node of indices
             * @param theDataNode Node of data
             * @param theBeginIndex index to begin with
             * @param theEndIndex index to end at
             */
            void load(ResourceManager* theResourceManager, dom::NodePtr theIndicesNode, dom::NodePtr theDataNode,
                      unsigned long theBeginIndex, unsigned long theEndIndex);

            void unload(dom::NodePtr theIndicesNode, dom::NodePtr theDataNode,
                    unsigned long theBeginIndex,
                    unsigned long theEndIndex);

            unsigned size() const;

			bool hasVertexData(VertexDataRole theRole) const;

            VertexDataBase & getVertexData(VertexDataRole theRole);
            const VertexDataBase & getVertexData(VertexDataRole theRole) const;
            VertexDataBasePtr getVertexDataPtr(VertexDataRole theRole);
            const VertexDataBasePtr getVertexDataPtr(VertexDataRole theRole) const;
            static PrimitiveType
            getTypeFromNode(dom::NodePtr thePrimitiveNode);

            void setMaterial(MaterialBasePtr theMaterial) {
                _myMaterial = theMaterial;
            }
            MaterialBase & getMaterial() {
                return *(_myMaterial);
            }
            const MaterialBase & getMaterial() const {
                return *(_myMaterial);
            }
            PrimitiveType getType() const {
                return _myType;
            }

            //TODO: make complete
            asl::Ptr<VertexDataAccessor<asl::Vector3f> > getLockingPositionsAccessor();
            asl::Ptr<VertexDataAccessor<asl::Vector3f> > getLockingNormalsAccessor();
            asl::Ptr<VertexDataAccessor<asl::Vector4f> > getLockingColorsAccessor();

            asl::Ptr<ConstVertexDataAccessor<asl::Vector3f> > getConstLockingPositionsAccessor() const; 
            asl::Ptr<ConstVertexDataAccessor<asl::Vector3f> > getConstLockingNormalsAccessor() const;
            asl::Ptr<ConstVertexDataAccessor<asl::Vector4f> > getConstLockingColorsAccessor() const;
    
            asl::Ptr<VertexDataAccessor<float> >         getLockingTexCoord1fAccessor(unsigned theSlot);
            asl::Ptr<VertexDataAccessor<asl::Vector2f> > getLockingTexCoord2fAccessor(unsigned theSlot);
            asl::Ptr<VertexDataAccessor<asl::Vector3f> > getLockingTexCoord3fAccessor(unsigned theSlot);

            bool intersect(const asl::LineSegment<float> & theStick,  IntersectionList & theIntersectionInfo);
            bool intersect(const asl::Ray<float> & theStick,  IntersectionList & theIntersectionInfo);
            bool intersect(const asl::Line<float> & theStick,  IntersectionList & theIntersectionInfo);
            // return all contacts with the primitive 
            bool collide(const asl::Sphere<float> & theSphere, const asl::Vector3<float> & theMotion, 
                    const asl::Matrix4f & theSphereSpaceTransform, SphereContactsList & theCollisionInfo);
            // return the first contact before theCurrentContact
            bool collide(const asl::Sphere<float> & theSphere, const asl::Vector3<float> & theMotion, 
                    const asl::Matrix4f & theSphereSpaceTransform, const asl::SweptSphereContact<float> & theCurrentContact,
                    SphereContacts & theNewContactInfo);
   
            const std::vector<VertexDataBasePtr> & getVertexData() const {
                return _myVertexData;
            }
            const std::vector<RenderStyleType> & getRenderStyles() const { 
                return _myRenderStyles; 
            }
            std::vector<RenderStyleType> & getRenderStyles() { 
                return _myRenderStyles; 
            }
       private:
            // hide default, copy ctor and assignment
            Primitive(); 
            Primitive(const Primitive &); 
            Primitive & operator= (const Primitive &); 

            template <class DETECTOR>
            bool scanElements(DETECTOR & theDetector,
                             const VertexData3f & thePositions,
                             const VertexData3f * theNormals);

            template <class DETECTOR>
            bool scanHierarchy(DETECTOR & theDetector, BoundingBoxTree & theTree,
                             const VertexData3f & thePositions,
                             const VertexData3f * theNormals);

            template<class T, class Alloc>
            void fillFromDataSource(std::vector<T, Alloc> & theBin, const ParameterFunction & theParameterFunction);

            template <class T>
            void assureTexCoords(const MaterialParameter & theParameter);    

            VertexDataBasePtr createVertexDataBin(ResourceManager* theResourceManager, TypeId theBinType, VertexDataRole theRole);
            void generateMesh(dom::NodePtr theIndicesNode);

            // TODO: this should be a functor or something like that
            template <class T, class Alloc>
            void createTangents(std::vector<T, Alloc> & theBin);
           
#ifdef WIN32            
            template <>
            void 
            createTangents(GfxVectorOfVector3f & theBin);
#endif
            void updateBoundingBoxTree();

            std::string                    _myShapeId;
            PrimitiveType                  _myType;
            MaterialBasePtr                _myMaterial;
            std::vector<VertexDataBasePtr> _myVertexData;
            std::vector<RenderStyleType>   _myRenderStyles;
            unsigned int                   _myDomIndex;
            mutable BoundingBoxTreePtr     _myBoundingBoxTree;
    };
    typedef asl::Ptr<Primitive> PrimitivePtr;
    typedef std::vector<PrimitivePtr> PrimitiveVector;        
       
    template <class T, class Alloc>
    void
    Primitive::createTangents(std::vector<T, Alloc> & theBin) {
        throw MaterialFault("Tangents are only available for three dimensional uv sets.", PLUS_FILE_LINE);
    }

    template<class T, class Alloc>
    void
    Primitive::fillFromDataSource(std::vector<T,Alloc> & theBin, const ParameterFunction & theParameterFunction) {
        switch (theParameterFunction) {
            case TANGENT:
                createTangents(theBin);
            break;
            default:
                throw UnknownDataSource( std::string("Unknown datasource function : ") + 
                                      asl::getStringFromEnum(theParameterFunction, ParameterFunctionStrings),
                                      PLUS_FILE_LINE);
        }
    }


    struct BoundingBoxTree {
        BoundingBoxTree() : 
            myMinIndex(asl::NumericTraits<int>::max()),
            myMaxIndex(asl::NumericTraits<int>::min()),
            myTotalChildren(0)
        {
                myBox.makeEmpty();
        }
        BoundingBoxTree(const asl::Box3<float> & theBox, int theIndex) : 
            myMinIndex(theIndex),
            myMaxIndex(theIndex),
            myTotalChildren(0),
            myBox(theBox)
        {}

        asl::Box3<float> myBox;
        int myMinIndex;
        int myMaxIndex;
        int myTotalChildren;
        std::vector<BoundingBoxTreePtr> myChildren;

        bool isLeaf() const {
            return myMinIndex == myMaxIndex;
        }

        /** insert a new Bounding Box into the tree using the following rules:
            - when the tree bounds are empty, the tree's bounds are set to the new box
            - when a child contains the box, recurse into the child 
            - when a child contains the box, insert into the child 
        */
        void insertBox(const asl::Box3<float> & theBox, int theIndex) {
            if (myBox.isEmpty()) {
                myBox = theBox;
                myMinIndex = theIndex;
                myMaxIndex = theIndex;
                myTotalChildren = 0;
                return;
            }
            if (myChildren.size() == 0) {
                // create a new hierarchy level above leaf
                myChildren.push_back(BoundingBoxTreePtr(new BoundingBoxTree(myBox, myMinIndex)));
                myChildren.push_back(BoundingBoxTreePtr(new BoundingBoxTree(theBox, theIndex)));
                myBox.extendBy(theBox);
                myTotalChildren = 2;
                return;
            }
            // descend into the hierarchy
            myBox.extendBy(theBox);
            myMinIndex = asl::minimum(myMinIndex, theIndex);
            myMaxIndex = asl::maximum(myMaxIndex, theIndex);
             ++myTotalChildren;
            BoundingBoxTreePtr emptiestContainingChild(0);
            for (int i = 0; i < myChildren.size();++i) {
                if (myChildren[i]->myBox.contains(theBox)) {
                    if (emptiestContainingChild) {
                        if (myChildren[i]->myTotalChildren < emptiestContainingChild->myTotalChildren) {
                            emptiestContainingChild = myChildren[i];
                        }
                    } else {
                        emptiestContainingChild = myChildren[i];
                    }
                }
            }
            if (emptiestContainingChild) {
                // a child contains our box, descend
                emptiestContainingChild->insertBox(theBox, theIndex);
                return;
            }
            // now try if there is a child that intersects
            BoundingBoxTreePtr emptiestIntersectingChild(0);
            for (int i = 0; i < myChildren.size();++i) {
                if (myChildren[i]->myBox.intersects(theBox)) {
                    if (emptiestIntersectingChild) {
                        if (myChildren[i]->myTotalChildren < emptiestIntersectingChild->myTotalChildren) {
                            emptiestIntersectingChild = myChildren[i];
                        }
                    } else {
                        emptiestIntersectingChild = myChildren[i];
                    }
                }
            }
            if (emptiestIntersectingChild) {
                // a child intersects our box, descend
                emptiestIntersectingChild->insertBox(theBox, theIndex);
                return;
            }
            // no existing bbox intersects, so just insert a new bbox here
            myChildren.push_back(BoundingBoxTreePtr(new BoundingBoxTree(theBox, theIndex)));
            return;
        }
    };


} // namespace

#endif // _ac_scene_Primitive_h_

