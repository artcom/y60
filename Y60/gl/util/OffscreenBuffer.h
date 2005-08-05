//============================================================================
//
// Copyright (C) 2002, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: OffscreenBuffer.h,v $
//
//     $Author: janbo $
//
//   $Revision: 1.6 $
//
// Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef AC_Y60_OFFSCREEN_BUFFER_INCLUDED
#define AC_Y60_OFFSCREEN_BUFFER_INCLUDED

#include "FrameBuffer.h"
#include "GLContext.h"

#include <asl/Exception.h>
#include <asl/Box.h>
#include <asl/Ptr.h>

#include <vector>
#include <string>

namespace y60 {

    class OffscreenBuffer : public FrameBuffer {
    public:
#ifdef WIN32
        typedef HPBUFFERARB OSHandle;
#endif
#ifdef LINUX
        typedef GLXPbuffer OSHandle;
#endif        
        enum TextureType { TEXTURE2D, CUBE };
        enum PixelType { RGB, RGBA, DEPTH, RGBA_FLOAT };

        OffscreenBuffer(int theWidth, int theHeight, PixelType theType = RGBA, TextureType theTextureType = TEXTURE2D)
            : _myWidth(theWidth), _myHeight(theHeight), _myPixelType(theType), _myTextureType(theTextureType), 
              _myOkFlag(false)
        {
            _myOkFlag = create();
        }
        ~OffscreenBuffer() {
            shutdown();
        }
        virtual PixelType getPixelType() const {
            return _myPixelType;
        }

        void startTexture();
        void finishTexture(int theTargetTextureID);
        
        virtual DrawingContext & getContext() {
             return *_myContext;
         }
        virtual const DrawingContext & getContext() const {
             return *_myContext;
         }
        virtual int getWidth() const {
            return _myWidth;
        }
        virtual int getHeight() const {
            return _myHeight;
        }  
        asl::Vector2<int> getSize() const {
            return asl::Vector2<int>(_myWidth, _myHeight);
        }
        bool ok() const {
            return _myOkFlag;
        }
        OSHandle getOSHandle()  {
            return _myHpbuffer;
        }
#ifdef LINUX
        Display * getDisplay() const {
            return _myDisplay;
        }
#endif        
    private:
	   bool       create();
	   void       shutdown();

       asl::Ptr<GLContext> _myContext;
       asl::Vector2<unsigned int> _mySize;
       int _myWidth;
       int _myHeight;
       PixelType _myPixelType;
       TextureType _myTextureType;
       bool _myOkFlag;

       OSHandle _myHpbuffer; // Handle to a pbuffer.
#ifdef LINUX
       Display * _myDisplay;  // X-Display
#endif       
    };

/*

class PBuffer : public GLContext{
	public:

		PBuffer(GLContext * theVisibleContext, int theWidth, int theHeight);
		~PBuffer();

        virtual void activate() const;
        virtual void bindAsTexture();
        //void       displayInVisibleContext();
	    bool       create();
	    void       shutdown();

	private:
#ifdef WIN32
	    HPBUFFERARB              _myHpbuffer;      // Handle to a pbuffer.
#endif
		int                      _myScreenWidth;
		int                      _myScreenHeight;
		bool                     _allExtensionsReady;
		GLContext *              _myVisibleContext;
		unsigned                 _myTextureId;
    };
*/

} // namespace
#endif // AC_Y60_PBUFFER
