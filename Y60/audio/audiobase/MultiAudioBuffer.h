//=============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: MultiAudioBuffer.h,v $
//     $Author: martin $
//   $Revision: 1.2 $
//       $Date: 2005/04/21 15:07:03 $
//
//  Datastructure to store multichannel audiobuffers.
//
//=============================================================================

#ifndef INCL_MULTIAUDIOBUFFER
#define INCL_MULTIAUDIOBUFFER

#include "AudioBuffer.h"
#include "SoundException.h"

namespace AudioBase {
    typedef std::vector<AudioBuffer*> BUFFERVECTOR;
    typedef std::vector<ChannelType> CHANNELVECTOR;
    const ChannelType ChannelTypesStereo[] = {Left, Right};
    const ChannelType ChannelTypesMono[] = {Mono};
    const ChannelType ChannelTypes6[] = {Left, Right, Mono, RearLeft, RearRight, Sub};


    /**
     * @ingroup Y60audioaudiobase
     * Datastructure to store multichannel audiobuffers
     */
    class MultiAudioBuffer {
        public:
            MultiAudioBuffer(unsigned theChannelCount) :
                _myTimeStamp(0.0)
            {
                const ChannelType * myTypes = NULL;
                switch (theChannelCount) 
                {
                case 1:
                    myTypes = ChannelTypesMono;
                    break;
                case 2:
                    myTypes = ChannelTypesStereo;
                    break;
                case 6:
                    myTypes = ChannelTypes6;
                    break;
                default:
                    throw SoundException("Can't handle Channelcount" ,PLUS_FILE_LINE); 
                }
                _myBuffers.reserve(theChannelCount);
                _myTypes.reserve(theChannelCount);
                for (int i = 0; i < theChannelCount; ++i) {
                    _myBuffers.push_back(new AudioBuffer);
                    _myTypes[i] = myTypes[i];
                }
            }

            ~MultiAudioBuffer() {
                // Delete buffers
                for (BUFFERVECTOR::iterator it = _myBuffers.begin(); it != _myBuffers.end(); ++it) {
                    delete *it;
                    *it = NULL;
                }
            }

            /**
             * Stores the given sample data in the internal buffers. The
             * sampledata theData is demultiplexed and stored in separate
             * buffers for each channel
             *
             * \param theData 16 bit sample data to store in the buffers
             * \param theTimeStamp timestamp of the sample
             */
            void setData(const asl::Signed16 * theData, double theTimeStamp) {
                _myTimeStamp = theTimeStamp;
                for (unsigned i = 0; i < _myBuffers.size(); ++i)
                {
                    AudioBuffer * pBuffer = _myBuffers[i];
                    const ChannelType & type = _myTypes[i];
                    pBuffer->set16Bit(theData, type, _myBuffers.size());
                }
            }

            /**
             *  Clears all internal buffers
             */
            void clear() {
                _myTimeStamp = 0.0;
                for (BUFFERVECTOR::iterator it = _myBuffers.begin(); it != _myBuffers.end(); ++it) {
                    (*it)->clear();
                }
            }

            /**
             *
             * \return the single buffer for mono
             */
            AudioBuffer * getMono() const {
                if (_myBuffers.size() != 1) {
                    throw SoundException("Not a mono buffer", PLUS_FILE_LINE); 
                }
                else { 
                    return _myBuffers[0];
                }
            }

            /**
             *
             * \return the left channel buffer
             */
            AudioBuffer * getLeft() const {
                if (_myBuffers.size() < 2) {
                    throw SoundException("Only a mono buffer", PLUS_FILE_LINE); 
                }
                return _myBuffers[0];
            }

            /**
             *
             * \return the right channel buffer
             */
            AudioBuffer * getRight() const {
                if (_myBuffers.size() < 2) {
                    throw SoundException("Only a mono buffer", PLUS_FILE_LINE); 
                }
                return _myBuffers[1];
            }

            AudioBuffer * getChannel(ChannelType type) const {
                for (int i = 0; i < _myBuffers.size(); ++i) {
                    if (type == _myTypes[i]) {
                        return _myBuffers[i];
                    }
                }
                return NULL;
            }

            /**
             *
             * \return the timestamp for this buffer
             */
            double getTimeStamp() const {
                return _myTimeStamp;
            }

            /**
             * Sets the audio timestamp for this buffer
             * \param theTimeStamp the timestamp for this buffer
             */
            void setTimeStamp(double theTimeStamp) {
                _myTimeStamp = theTimeStamp;
            }

            /**
             *
             * \return the number of channels in this buffer
             */
            unsigned getNumChannels() const {
                return _myBuffers.size();
            }

        private:
            BUFFERVECTOR        _myBuffers;
            CHANNELVECTOR       _myTypes;
            double              _myTimeStamp;
    };
}

#endif

