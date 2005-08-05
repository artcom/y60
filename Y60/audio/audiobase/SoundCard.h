//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: SoundCard.h,v $
//
//     $Author: thomas $
//
//   $Revision: 1.12 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_SOUNDCARD
#define INCL_SOUNDCARD

#include <asl/ITimeSource.h>
#include <asl/settings.h>

#include <string>
#include <vector>

#ifdef WIN32
#pragma warning( disable : 4244 ) // Disable ffmpeg warning
#define EMULATE_INTTYPES
#endif
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
#ifdef WIN32
#pragma warning( default : 4244 ) // Renable ffmpeg warning
#endif

namespace AudioBase {

class AudioBuffer;    

/**
 * @ingroup audiobase
 * 
 * Base class for different implementations defining a (mostly) platform
 * specific soundcard. 
 *
 * @date 03-11-2005
 *
 * @author Thomas Schüppel <thomas@artcom.de>
 *
 * @todo split soundcard into two classes, one that is an audiosource and one that
 *       is an audio sink.
 * @bug 
 *
 */
class SoundCard: public asl::ITimeSource {
public:

    SoundCard();
    virtual ~SoundCard();

    /**
     * @return the current delay between a byte written by this objects write() function
     *         and its playback.
     */
    virtual double getCurrentDelay() { return 0.0 ;}

    /**
     * Sets some additional soundcard parameters. Derived implementations should
     * call the base class after they did their work on this.
     * @param theLatency latency in miliseconds
     * @param theSampleRate samplerate to set  
     * @param theSampleBits samplebits to set (most implementations only support 16)
     * @param theNumChannels number of channels to set up for
     */
    virtual void setParams(double theLatency, int theSampleRate, int theSampleBits, unsigned theNumChannels);

    /**
     * Opens the output for writing
     * @pre setParams was called.
     */
    virtual void openOutput();
    /**
     * Opens the input for reading
     * @param myNumChannels number of
     * @todo move number of channels into setParams
     */
    virtual void openInput(int myNumChannels);
    /**
     * Closes the output. No more writing is possible.
     * @pre openOutput was called.
     */
    virtual void closeOutput();
    /**
     * Closes the input. No more reading is possible.
     * @pre openInput was called.
     */
    virtual void closeInput();
    virtual std::string asString() const = 0;
    /**
     * @return the number of buffer underruns occured since the buffer was openend
     */
    virtual int getNumUnderruns() const = 0;

    /**
     * plays the AudioBuffer given in theBuffers. The method blocks until the buffer
     * is completely stored so it can be safely discarded afterwards.
     *
     * @param &theBuffers AudioBuffer to play
     * @pre openOutput was called
     */
    virtual void write(std::vector<AudioBuffer*> &theBuffers)=0;

    /**
     * reads data from the soundcard. The method blocks until the buffer is completely
     * filled with data.
     *
     * @param &theBuffers AudioBuffer to read into
     * @pre openInput was called
     */
    virtual void read(std::vector<AudioBuffer*> &theBuffers)=0;
    
    const std::string & getCardName() const;
    const std::string & getDeviceName() const;
    int getMaxOutChannels() const;
    int getMaxInChannels() const;
    
    int getNumOutChannels() const;
    int getNumInChannels() const;
    int getSampleRate() const;
    int getSampleBits() const;
    /**
     * @return the size of one sample in bytes
     */
    int getSampleSize() const;
    /**
     * @return real latency of the soundcard in milliseconds
     */
    double getLatency() const;
    

    /**
     * @retval true if output is open 
     * @retval false otherwise
     */
    bool isOutputOpen() const;
    bool isInputOpen() const;

    /**
     * Initializes the AC3 encoder
     * @param theSampleRate input samplerate
     * @param theBitrate input bitrate
     * @param theChannels number of input channels
     * @return < 0 if initialization failed.
     * @see ffmpeg
     */
    int initAC3encoder(int theSampleRate, int theBitrate, int theChannels);
    /**
     * Deinitializes the AC3 encoder
     * @return < 0 if something went wrong
     * @see ffmpeg
     */
    int stopAC3encoder();
    /**
     * encodes the samples theSamples into AC3 and writes the result into theOutputBuffer. 
     * @param *theOutputBuffer buffer to write the AC3 data into
     * @param theOutputBufferSize maximum number of bytes that can be written into theOutputBuffer
     * @param *theSamples source sample data
     * @return < 0 if something went wrong
     */
    int encodeAC3frame(unsigned char *theOutputBuffer, int theOutputBufferSize, const asl::Signed16 *theSamples);

    /**
     * @return AVCodecContext of the AC3 encoder
     * @see ffmpeg
     */
    const AVCodecContext * getAC3Context() const;
protected:
    /**
     * Initializes the internal members. This should be called by derived classes
     * in their constructor.
     *
     * @param theDevName Identifying name of the device (computer readable)
     * @param theCardName Identifying name of the card (human readable)
     * @param theMaxOutChannels maximum number of channels for output, that this card supports
     * @param theMaxInChannels maximum number of channels for input, that this card supports
     */
    void init(const std::string & theDevName, const std::string & theCardName, 
            int theMaxOutChannels, int theMaxInChannels);

    /**
     * Sets a target latency of the soundcard. Implementations should try to
     * provide a latency close to what is set in theLatency. You should call 
     * getLatency to retrieve the actual latency the soundcard will provide.
     *
     * @param theLatency latency in milliseconds
     */
    void setLatency(double theLatency);
private:
    bool         _myIsInitialized;
    std::string  _myCardName;
    std::string  _myDevName;
    int          _myMaxOutChannels;
    int          _myMaxInChannels;

    // TODO: This in/out repetition is stupid. We should have different 
    // classes for input and output sound cards.
    bool         _myIsOutOpen;
    bool         _myIsInOpen;
    int          _myNumOutChannels;
    int          _myNumInChannels;
    double       _myLatency;
    int          _mySampleRate;
    int          _mySampleBits;

    // AC3     
    AVCodec*     _myAC3Codec;
    AVCodecContext* _myAC3Context;
};

std::ostream& operator << (std::ostream& strm, const SoundCard& theCard);


}
#endif
