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
//   $RCSfile: CTScan.h,v $
//   $Author: martin $
//   $Revision: 1.11 $
//   $Date: 2005/04/21 10:00:39 $
//
//
//=============================================================================

#ifndef _ac_y60_components_ctscan_h_
#define _ac_y60_components_ctscan_h_

#include <asl/dom/Nodes.h>
#include <asl/math/Box.h>
#include <y60/image/PixelEncoding.h>
#include <y60/scene/Scene.h>

#include <asl/raster/pixel_functions.h>
#include <asl/raster/palgo.h>
#include <asl/raster/raster.h>
#include <string>
#include <sigc++/sigc++.h>
#include <glibmm/ustring.h>
#include <asl/raster/raster.h>
#include <y60/base/AcBool.h>

#include <map>

namespace asl {
    class PackageManager;
}

namespace y60 {

DEFINE_EXCEPTION(CTScanException, asl::Exception);

//typedef std::vector<dom::ResizeableRasterPtr> SegmentationBitmap;

class CTScan {
    public:
        enum ScanState {
            LOADING,
            COMPLETE,
            BROKEN
        };
        enum Orientation {
            IDENTITY,
            Y2Z,
            X2Z, 
            ARBITRARY
        };
        CTScan();
        // CTScan(const std::string & theInputSpec, asl::PackageManager & thePackageManager);
        virtual ~CTScan();
       
        int loadSlices(asl::PackageManager & thePackageManager, const std::string & theSubDir, const std::string& thePackage);
        int loadSphere(const asl::Vector3i & size);
        int setSlices(std::vector<dom::ResizeableRasterPtr> theSlices);

        void reconstructToImage(Orientation theOrientation, int theSliceIndex, dom::NodePtr & theImageNode);
        void reconstructToImage(const asl::Quaternionf & theOrientation, const asl::Vector3f & theSlicePosition,
                                dom::NodePtr & theImageNode, bool theTrilliniarInterpolate = true);

        /** Returns the default window center/width */
        const asl::Vector2f & getDefaultWindow() const;

        /** Returns the number of voxels in x/y/z direction */
        asl::Vector3i getVoxelDimensions() const;

        /** Returns the size of a single voxel (in meters) in x/y/z direction. 
         *  Any component may be 0 = unknown. */
        asl::Vector3f getVoxelSize() const;

        /** Set the size of a single voxel (in meters) in x/y/z direction. 
         */
        void setVoxelSize(const asl::Vector3f & theSize);

        /** Returns the aspect ratio of the current voxel size. the X component is always 1.0
         */
        asl::Vector3f getVoxelAspect() const;

        /** Returns the minimum/maximum voxel value possible in the dataset */
        asl::Vector2d getValueRange();

        /** Returns the minimum/maximum voxel value that actually occurs in the dataset */
        asl::Vector2d getOccurringValueRange();

        asl::Vector2i countTrianglesGlobal(const asl::Box3i & theVoxelBox, 
            double theThresholdMin, double theThresholdMax, int theDownSampleRate, bool theCloseAtClippingBoxFlag);

        asl::Vector2i countTrianglesInVolumeMeasurement(const asl::Box3i & theVoxelBox, dom::NodePtr theVolumeNode,
                    dom::NodePtr theThresholdPalette, int theDownSampleRate, bool theCloseAtClippingBoxFlag);

        /** Create an isosurface from the voxel dataset */
        ScenePtr polygonizeGlobal(const asl::Box3i & theVoxelBox, double theThresholdMin, double theThresholdMax, 
            int theDownSampleRate, bool theCloseAtClippingBoxFlag, bool theCreateNormalsFlag, 
            asl::PackageManagerPtr thePackageManager, 
            unsigned int theNumVertices = 0, unsigned int theNumTriangles = 0);

        ScenePtr polygonizeVolumeMeasurement(const asl::Box3i & theVoxelBox, dom::NodePtr theVolumeNode, 
            dom::NodePtr theThresholdPalette, int theDownSampleRate, bool theCloseAtClippingBoxFlag, 
            bool theCreateNormalsFlag, asl::PackageManagerPtr thePackageManager, 
            unsigned int theNumVertices = 0, unsigned int theNumTriangles = 0);

        /** Create a downscaled 3D texture from the dataset */
        void create3DTexture(dom::NodePtr theImageNode, int theMaxTextureSize);

        /** Progress signal for loading */
        sigc::signal<bool, double, Glib::ustring> signal_progress() const { return _myProgressSignal; }
        
        /** Computes the histogram of a given volume of interest */
        void
        computeHistogram(const asl::Box3i & theVOI, std::vector<unsigned> & theHistogram,
                         bool useOccurringRange = true);

        // add a the grey value of a single voxel to the profile 
        void computeProfile(const asl::Point3i & thePoint, std::vector<asl::Signed32> & theProfile);
        // add the grey values of all voxels in the multiline defined by thePoints to theProfile  
        void computeProfile(const std::vector<asl::Point3i> & thePoints,
                std::vector<asl::Signed32> & theProfile, std::vector<asl::Point3i> & thePointsSampled);

        /** Get a pointer to the first voxel of a slice */
        template <class VoxelT>
        VoxelT *
        getSlicePtr(int theIndex) {
            return reinterpret_cast<VoxelT *>(_mySlices[theIndex]->pixels().begin());
        }
        
        template <class VoxelT>
        const VoxelT *
        getSlicePtr(int theIndex) const {
            return reinterpret_cast<const VoxelT *>(_mySlices[theIndex]->pixels().begin());
        }

        /* Some functions that are TNT but not ctscan specific */
        static dom::NodePtr createGrayImage(dom::NodePtr theParent, int theWidth, int theHeight, int theValue);
        static dom::NodePtr createRGBAImage(dom::NodePtr theParent, int theWidth, int theHeight, int theValue);
        static void resizeVoxelVolume(dom::NodePtr theVoxelVolumeNode, const asl::Box3f & theDirtyBox);
        static void copyCanvasToVoxelVolume(dom::NodePtr theMeasurement, dom::NodePtr theCanvas, 
                                            const asl::Box3f & theDirtyBox, Orientation theOrientation,
                                            dom::NodePtr thePaletteNode);
        template <class VoxelT>
        static void copyVoxelVolumeToCanvasImpl(dom::NodePtr theMeasurement, 
            unsigned theGlobalThresholdIndex, dom::NodePtr theCanvas, 
            dom::NodePtr theReconstructedImage,unsigned theSliceIndex,
            Orientation theOrientation, dom::NodePtr thePaletteNode);

        static void copyVoxelVolumeToCanvas(dom::NodePtr theMeasurement, 
            unsigned theGlobalThresholdIndex, dom::NodePtr theCanvas, 
            dom::NodePtr theReconstructedImage,unsigned theSliceIndex,
            Orientation theOrientation, dom::NodePtr thePaletteNode);
        

        static void applyBrush(dom::NodePtr theCanvasImage, unsigned theX, unsigned theY,
                               dom::NodePtr theBrushImage, const asl::Vector4f & theColor);

        static void renderTransferFunction(dom::NodePtr theTransferFunction, dom::NodePtr theTargetImage);

        asl::Matrix4f getModelViewMatrix(const asl::Quaternionf & theOrientation);

        // input in Voxels, output in VoxelSpace
        asl::Planef getVoxelPlane(const asl::Quaternionf & theOrientation, float theMetricOffset);
        
        // everything below this line is deprecated
        bool verifyCompleteness();
        void clear();
        int getSliceCount() const { return _myMaxZ - _myMinZ + 1; }

        bool notifyProgress(double theProgress, const std::string & theMessage = "");

        asl::Vector3i
        getReconstructionDimensions(Orientation theOrientation) const;

        asl::Vector3i
        getReconstructionDimensions(const asl::Quaternionf & theOrientation) const;

        float
        getSliceThickness(const asl::Quaternionf & theOrientation) const;

    private:
        CTScan(const CTScan&); // hide copy constructor
        sigc::signal<bool, double, Glib::ustring> _myProgressSignal;
        void prepareBox(asl::Box3i & theVoxelBox);
        void findOccurringValueRange();
        
        y60::PixelEncoding _myEncoding;
        std::vector<dom::ResizeableRasterPtr> _mySlices;
        asl::Vector2f _myDefaultWindow;
        asl::Vector3f _myVoxelSize;

        inline bool 
        isInside(int x, int y, int z) {
            if ((z < 0 || z >= _mySlices.size()) ||
                (y < 0 || y >= _mySlices[0]->height()) ||
                (x < 0 || x >= _mySlices[0]->width()))
            {
                return false;
            } else {
                return true;
            }                
        }

        void getReconstructionBounds(const asl::Quaterniond & theOrientation, 
                asl::Box3d & theBounds, asl::Vector3i & theSize) const;

        asl::Box3f computeProjectionBounds(const asl::Matrix4f & theProjection) const;

        template <class VoxelT>
        inline float
        linearInterpolate(VoxelT theFloorValue, VoxelT theCeilValue, int theFloor, int theCeil, float thePos) {
            float myWeight = thePos - float(theFloor);
            return (1.0f-myWeight) * theFloorValue + myWeight * theCeilValue;
        }

        /**
         * trillinear interpolated voxel value at thePosition
         */
        template <class VoxelT>
        VoxelT
        interpolatedValueAt(const asl::Vector3d & thePosition);

        /**
         * non interpolated voxel value at thePosition
         */
        template <class VoxelT>
        VoxelT
        fastValueAt(const asl::Vector3d & thePosition);

        template <class VoxelT>
        void reconstructToImageImpl(const asl::Quaternionf & theOrientation, const asl::Vector3f & theSlicePosition, 
                                    dom::NodePtr & theImageNode, bool theTrilliniarInterpolate = true);

        template <class VoxelT>
        void reconstructToImageImpl(Orientation theOrientation, int theSliceIndex, dom::NodePtr & theImageNode);

        asl::Vector2d _myOccurringValueRange;
        
        template <class VoxelT, class SegmentationPolicy>
        bool
        countMarchingCubes(const asl::Box3i & theVoxelBox, int theDownSampleRate, bool theCloseAtClippingBoxFlag,
                           SegmentationPolicy & theSegmentizer,
                           unsigned int & theVertexCount, unsigned int & theTriangleCount);
        template <class VoxelT, class SegmentationPolicy> 
        bool
        applyMarchingCubes(const asl::Box3i & theVoxelBox, int theDownSampleRate, bool theCloseAtClippingBoxFlag,
                             bool theCreateNormalsFlag, ScenePtr theScene,
                             SegmentationPolicy & theSegmentizer,
                             unsigned int theNumVertices = 0, unsigned int theNumTriangles = 0);
        template <class VoxelT>
        void
        countVoxelValues(const asl::Box3i & theVOI, std::vector<unsigned> & theHistogram,
                         bool useOccurringRange);
        
        // computes the grey-scale profile of all voxels
        // from [theStart, theEnd[  (without theEnd)
        // Can be used in a loop to compute multilines,
        // but don't forget to profile the very last voxel
        // separatly.
        void computeLineSegmentProfile(const asl::Point3i & theStart, 
                                       const asl::Point3i & theEnd, 
                                       std::vector<asl::Signed32> & theProfile,
                                       std::vector<asl::Point3i> & thePointsSampled);
        
        
        int appendTo3DTexture(int theSlice, asl::Block & the3dTexture, int theXSize, int theYSize);
        template<class PIXEL> 
        void appendEmptySlices(asl::Block & the3DTexture, unsigned theSliceCount, int theBytesPerSlice);

        std::string setupMaterial(y60::SceneBuilderPtr theSceneBuilder, bool theCreateNormalsFlag);
        void setupScene(y60::ScenePtr theScene, const std::string & theShapeId);
        void resetCamera(ScenePtr theScene, const asl::Box3f & theBox);
        void allocateStencils();

        // comparator for Vector3i
        struct Vector3iCmp {
            bool operator() (const asl::Vector3i & a, const asl::Vector3i & b) const {
                if ( a[0] < b[0] ||
                    (a[0] == b[0] && a[1] < b[1]) ||
                    (a[0] == b[0] && a[1] == b[1] && a[2] < b[2]))
                {
                    return true;
                }
                return false;
            }
        };

        typedef std::map<asl::Vector3i, unsigned char, Vector3iCmp> ColorMap;

        template <class VoxelT>
        struct PaletteInfo {
            asl::Vector2<VoxelT> thresholds;
            unsigned char index;
            PaletteInfo() {};
            PaletteInfo(asl::Vector2<VoxelT> theThreshold, unsigned char theIndex) : 
                thresholds(theThreshold), index(theIndex) {}
        };

        template <class VoxelT>
        struct PaletteItem {
            asl::Vector2<VoxelT> thresholds;
            asl::Vector3i color;
            PaletteItem() {};
            PaletteItem(const asl::Vector2<VoxelT> & theThreshold, const asl::Vector3i & theColor) : 
                thresholds(theThreshold), color(theColor) {}
        };

        template <class VoxelT> 
        class PaletteTable : public std::vector<PaletteItem<VoxelT> >             
        {
            typedef std::vector<PaletteItem<VoxelT> > Base;
        public: 
            PaletteTable(size_t theSize, const PaletteItem<VoxelT> & theItem) : Base(theSize, theItem) {}
        };

        template <class VoxelT>
        static unsigned char
        getSegmentationAlpha(unsigned theIndex, VoxelT theVoxel, const PaletteTable<VoxelT> & thePalette, unsigned char theAlpha);

        static void fillColorMap(dom::NodePtr thePaletteNode, ColorMap & theColorMap);
        // everything below this line is deprecated
        int _myMinZ;  // min Z-Pos of decoded slices
        int _myMaxZ;  // max Z-Pos of decoded slices
        ScanState _myState;
};

typedef asl::Ptr<CTScan> CTScanPtr;

}
#endif
