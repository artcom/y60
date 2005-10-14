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
//   $RCSfile: CTScan.h,v $
//   $Author: martin $
//   $Revision: 1.11 $
//   $Date: 2005/04/21 10:00:39 $
//
//
//=============================================================================

#ifndef _ac_y60_components_ctscan_h_
#define _ac_y60_components_ctscan_h_

#include <dom/Nodes.h>
#include <asl/Box.h>
#include <y60/PixelEncoding.h>
#include <y60/Scene.h>

#include <asl/pixel_functions.h>
#include <asl/palgo.h>
#include <asl/raster.h>
#include <string>
#include <sigc++/sigc++.h>
#include <glibmm/ustring.h>
#include <asl/raster.h>
#include <y60/AcBool.h>

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
            X2Z
        };
        CTScan();
        // CTScan(const std::string & theInputSpec, asl::PackageManager & thePackageManager);
        virtual ~CTScan();
       
        int loadSlices(asl::PackageManager & thePackageManager, const std::string & theSubDir, const std::string& thePackage);
        int loadSphere(int size);
        int setSlices(std::vector<dom::ResizeableRasterPtr> theSlices);

        void reconstructToImage(Orientation theOrientation, int theSliceIndex, dom::NodePtr & theImageNode);

        /** Returns the default window center/width */
        const asl::Vector2f & getDefaultWindow() const;

        /** Returns the number of voxels in x/y/z direction */
        asl::Vector3i getVoxelDimensions();

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

        asl::Vector2i countTrianglesGlobal(const asl::Box3i & theVoxelBox, 
            double theThresholdMin, double theThresholdMax, int theDownSampleRate);

        asl::Vector2i countTrianglesInVolumeMeasurement(const asl::Box3i & theVoxelBox, dom::NodePtr theVolumeNode,
                    dom::NodePtr theThresholdPalette, int theDownSampleRate);

        /** Create an isosurface from the voxel dataset */
        ScenePtr polygonizeGlobal(const asl::Box3i & theVoxelBox, double theThresholdMin, double theThresholdMax, 
            int theDownSampleRate, bool theCreateNormalsFlag, asl::PackageManagerPtr thePackageManager, 
            unsigned int theNumVertices = 0, unsigned int theNumTriangles = 0);

        ScenePtr polygonizeVolumeMeasurement(const asl::Box3i & theVoxelBox, dom::NodePtr theVolumeNode, dom::NodePtr theThresholdPalette, 
            int theDownSampleRate, bool theCreateNormalsFlag, asl::PackageManagerPtr thePackageManager, 
            unsigned int theNumVertices = 0, unsigned int theNumTriangles = 0);

        /** Create a downscaled 3D texture from the dataset */
        void create3DTexture(dom::NodePtr theImageNode, int theMaxTextureSize);

        /** Progress signal for loading */
        sigc::signal<bool, double, Glib::ustring> signal_progress() const { return _myProgressSignal; }
        
        /** Computes the histogram of a given volume of interest */
        void
        computeHistogram(const asl::Box3i & theVOI, std::vector<unsigned> & theHistogram);

        // add a the grey value of a single voxel to the profile 
        void computeProfile(const asl::Point3i & thePoint, std::vector<asl::Signed16> & theProfile);
        // add the grey values of all voxels in the multiline defined by thePoints to theProfile  
        void computeProfile(const std::vector<asl::Point3i> & thePoints,
                std::vector<asl::Signed16> & theProfile, std::vector<asl::Point3i> & thePointsSampled);

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
        static void copyCanvasToVoxelVolume(dom::NodePtr theMeasurement, dom::NodePtr theCanvasImage,
                                            const asl::Box3f & theDirtyBox, Orientation theOrientation,
                                            dom::NodePtr thePaletteNode);
        template <class VoxelT>
        static void copyVoxelVolumeToCanvasImpl(dom::NodePtr theMeasurement, dom::NodePtr theCanvas, 
            dom::NodePtr theReconstructedImage,unsigned theSliceIndex,
            Orientation theOrientation, dom::NodePtr thePaletteNode);

        static void copyVoxelVolumeToCanvas(dom::NodePtr theMeasurement, dom::NodePtr theCanvas, 
            dom::NodePtr theReconstructedImage,unsigned theSliceIndex,
            Orientation theOrientation, dom::NodePtr thePaletteNode);
        

        static void applyBrush(dom::NodePtr theCanvasImage, unsigned theX, unsigned theY,
                               dom::NodePtr theBrushImage, const asl::Vector4f & theColor);

        // everything below this line is deprecated
        bool verifyCompleteness();
        void clear();
        int getSliceCount() const { return _myMaxZ - _myMinZ + 1; }

        bool notifyProgress(double theProgress, const std::string & theMessage = "");

    private:
        CTScan(const CTScan&); // hide copy constructor
        sigc::signal<bool, double, Glib::ustring> _myProgressSignal;
        void prepareBox(asl::Box3i & theVoxelBox);
        
        y60::PixelEncoding _myEncoding;
        std::vector<dom::ResizeableRasterPtr> _mySlices;
        asl::Vector2f _myDefaultWindow;
        asl::Vector3f _myVoxelSize;
        
        template <class VoxelT, class SegmentationPolicy>
        bool
        countMarchingCubes(const asl::Box3i & theVoxelBox, int theDownSampleRate,
                           SegmentationPolicy & theSegmentizer,
                           unsigned int & theVertexCount, unsigned int & theTriangleCount);
        template <class VoxelT, class SegmentationPolicy> 
        bool
        applyMarchingCubes(const asl::Box3i & theVoxelBox, int theDownSampleRate,
                             bool theCreateNormalsFlag, ScenePtr theScene,
                             SegmentationPolicy & theSegmentizer,
                             unsigned int theNumVertices = 0, unsigned int theNumTriangles = 0);
        template <class VoxelT>
        void
        countVoxelValues(const asl::Box3i & theVOI, std::vector<unsigned> & theHistogram);
        
        // computes the grey-scale profile of all voxels
        // from [theStart, theEnd[  (without theEnd)
        // Can be used in a loop to compute multilines,
        // but don't forget to profile the very last voxel
        // separatly.
        void computeLineSegmentProfile(const asl::Point3i & theStart, 
                                       const asl::Point3i & theEnd, 
                                       std::vector<asl::Signed16> & theProfile,
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
