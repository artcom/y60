
#ifndef _ac_y60_DShowDecoder_h_
#define _ac_y60_DShowDecoder_h_

#include "DShow.h"
#include <asl/PlugInBase.h>
#include <y60/MovieDecoderBase.h>

#include <asl/Block.h>

namespace y60 {

    
    const std::string MIME_TYPE_WMV = "application/wmv";
            
    class DShowDecoder : public MovieDecoderBase, 
                         public asl::PlugInBase 
    {
        public:
    
            DShowDecoder(asl::DLHandle theDLHandle);
            virtual ~DShowDecoder() {}
            
            virtual asl::Ptr<MovieDecoderBase> instance() const;

            std::string canDecode(const std::string & theUrl, 
                    asl::ReadableStream * theStream = 0);
        
            void load(const std::string & theFilename);
            
            void load(asl::Ptr<asl::ReadableStream> theSource, 
                      const std::string & theFilename) {
                throw NotYetImplemented(JUST_FILE_LINE);
            }
            
            void startMovie(double theStartTime = 0.0);
            void stopMovie();
			void pauseMovie();

            float getVolume();
            void setVolume(float theVolume); // in [0,1]

		/**
         * Reads the current frame.
         * @param theTime is ignored by this decoder
         * @param theFrame is ignored by this decoder
         * @param theTargetRaster image raster to render the frame into
         * @return theTime
         */
            double readFrame(double theTime, unsigned theFrame, 
                    dom::ResizeableRasterPtr theTargetRaster);

        private:
			void setupMovieAttributes();

            IGraphBuilder *_myGraph;
            ISampleGrabber *_mySampleGrabber;
			IMediaControl *_myMediaControl;
			IBasicAudio *_myBasicAudio;
			IMediaSeeking *_myMediaSeeking;
        
    };

}


#endif

