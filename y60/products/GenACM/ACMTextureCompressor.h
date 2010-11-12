/*
/--------------------------------------------------------------------
|
|      $Id: TextureCompressor.h,v 1.2 2002/09/06 18:18:05 valentin Exp $
|
\--------------------------------------------------------------------
*/

#if !defined(INCL_TEXTURECOMPRESSOR)
#define INCL_TEXTURECOMPRESSOR

#include "../ViewACM/CompressionType.h"
#include <paintlib/plbitmap.h>
#include <paintlib/planybmp.h>

#include <stdio.h>
#include <GL/gl.h>

namespace  TextureCompression
{
    class TextureCompressor 
    {
        public:
            TextureCompressor(COMPRESSION_FORMAT myFormat, bool verbose);
            virtual ~TextureCompressor();

            void         setFormat(COMPRESSION_FORMAT myFormat) { _myFormat = myFormat; }
            virtual void compressAndStoreFrame(asl::WriteableFile & theBlock,
                    const std::string & theFileName,
                    PLBmp * pBmp, 
                    long theIndex,
                    int theWidthAspect,
                    int theHeightAspect,
                    int theOrientation,
                    float theFramesPerSecond,
                    int theFramecount,
                    bool theCreateHeaderFlag);
        private:
            static void fillOGLTHeader(int width, int height, OGLT_Header & myHeader,
                    int compressedDataSize,
                    long theCompressionFormat,
                    int theWidthAspect,
                    int theHeightAspect,
                    int theOrientation, 
                    float theFramesPerSecond);

            int compress(PLAnyBmp& AnyBmp, GLubyte*&  img); 
            void storeFrame(asl::WriteableFile & theBlock,
                    GLubyte * img,
                    const std::string & theFileName,
                    int height, int width, 
                    long theIndex, 
                    int theWidthAspect, 
                    int theHeightAspect, 
                    int theOrientation, 
                    float theFramesPerSecond,
                    int compressedSize,
                    int theFramecount,
                    bool theCreateHeaderFlag); 

            COMPRESSION_FORMAT _myFormat;
            bool               _myVerbose;
    };

};

#endif


