#ifndef AC_Y60_VIDEO_EDGE_BLENDER
#define AC_Y60_VIDEO_EDGE_BLENDER

#include <vector>
#include <asl/Block.h>

namespace y60 {

class EdgeBlender {
	public:
		enum BLENDAREA{TOP, BOTTOM, LEFT, RIGHT, HORIZONTAL_SPLIT, VERTICAL_SPLIT};
        enum EDGEBLENDINGMODE { NONE,
                                BLIT,       // blit
                                PBUFFERED,  // use pbuffered
                                BLENDBORDER // traditional blending regions on the border
                                };

		EdgeBlender(EDGEBLENDINGMODE theMode);
		~EdgeBlender();

        void setupViewport();
		void reshape(int theWidth, int theHeight);
        EDGEBLENDINGMODE getMode() {
            return _myMode;
        }
        void setMode(const EDGEBLENDINGMODE theMode) {
            _myMode = theMode;
        }
        int getWidthCorrection() const ;
		void display(bool theLightingFlag, bool theAntialiasingFlag, bool theTextureingFlag);
		void setBlendValues(BLENDAREA theBlendArea, std::vector<float> & toAlpha);
		const std::vector<float> & getBlendValues(BLENDAREA theBlendArea) const {
			return _toAlpha[theBlendArea];
		}

		void addBlendArea(BLENDAREA theBlendArea, int theWidth);
		void addBlendArea(BLENDAREA theBlendArea, int theWidth, int thePoints, float theGamma);

		void removeBlendArea(BLENDAREA theBlendArea) ;
	private:
		EdgeBlender();
		EdgeBlender & operator=(EdgeBlender & otherEdgeBlender);
		EdgeBlender(EdgeBlender & otherEdgeBlender);

		typedef std::vector<float> BlendCurve;


		bool  _drawAt[6];
		int   _myWidth[6];

		BlendCurve _toAlpha[6];

        EDGEBLENDINGMODE _myMode;
		int              _myScreenWidth;
		int              _myScreenHeight;
        asl::Block       _myData;

        void    preDisplay();
        void    postDisplay(bool theLightingFlag, bool theAntialiasingFlag, bool theTextureingFlag);
        void    preBlitFramebuffer();
        void    blitFramebuffer();
        void    postBlitFramebuffer();

        void    computeGammaRamp(BLENDAREA theBlendArea, int thepoints, float theGamma);
    };

}

#endif // AC_Y60_VIDEO_EDGE_BLENDER

//==============================================================================
//
// $Log: EdgeBlender.h,v $
// Revision 1.3  2005/02/16 10:24:43  christian
// - added texture toggeling
// - fixed key mover with quaternions
//
// Revision 1.2  2004/10/18 16:22:56  janbo
// moved namespaces renderer, scene, renderutil, input to y60
// fixed debug test renderer extension
//
// Revision 1.1  2004/02/11 14:50:08  martin
// moved renderer to gl/renderer, text and util
//
// Revision 1.3  2003/11/21 18:09:58  valentin
// - pbuffer next step
//
// Revision 1.2  2003/11/19 11:08:54  valentin
// - marriage between edgeblending and antialiasing
//
// Revision 1.1  2003/11/17 11:37:07  valentin
// - edgeblending support and control
//
// Revision 1.2  2002/09/06 18:18:03  valentin
// merged linuxport_2nd_try branch into trunk
// -branch tag before merge : branch_before_merge_final
// -trunk tag before merge  : trunk_before_merge
//
//
// -Merge conflicts:
//  image/glutShowImage/Makefile
//  image/glutShowImage/glShowImage.C
//  loader/libPfIv/pfiv.c++
//  lso/script/vrfm/Intercom.h
//  tools/Pfconvert/pfconvert.c
//  vrap/libMover/HMD.c++
//
//  - compiled && testrun
//  - commit
//  - merged trunk tag : trunk_after_merge
//
// Revision 1.1.2.4  2002/08/30 13:41:55  martin
// std-ized header files
//
// Revision 1.1.2.3  2002/08/27 23:16:03  christian
// added setter for gamma
//
// Revision 1.1.2.2  2002/07/10 12:59:30  stefan
// - added gamma support (now computes gamma ramp by itself!)
//
// Revision 1.1.2.1  2002/02/05 09:47:22  david
// added EdgeBlender
//
//
//==============================================================================
