//=============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//=============================================================================

#include "CoreAudioPump.h"

#include <asl/Logger.h>
//#include <asl/string_functions.h>
//#include <asl/numeric_functions.h>
#include <asl/Exception.h>

//#include <exception>
//#include <sstream>
//#include <string.h>

using namespace std;

namespace asl {

DEFINE_EXCEPTION(CoreAudioException, Exception);
    
#define checkStatus(x) if (x) { \
    throw CoreAudioException((char*)(&x), PLUS_FILE_LINE); \
}

void PrintStreamDesc (AudioStreamBasicDescription *inDesc)
{
    if (!inDesc) {
        printf ("Can't print a NULL desc!\n");
        return;
    }

    printf ("- - - - - - - - - - - - - - - - - - - -\n");
    printf ("  Sample Rate:%f\n", inDesc->mSampleRate);
    printf ("  Format ID:%s\n", (char*)&inDesc->mFormatID);
    printf ("  Format Flags:%lX\n", inDesc->mFormatFlags);
    printf ("  Bytes per Packet:%ld\n", inDesc->mBytesPerPacket);
    printf ("  Frames per Packet:%ld\n", inDesc->mFramesPerPacket);
    printf ("  Bytes per Frame:%ld\n", inDesc->mBytesPerFrame);
    printf ("  Channels per Frame:%ld\n", inDesc->mChannelsPerFrame);
    printf ("  Bits per Channel:%ld\n", inDesc->mBitsPerChannel);
    printf ("- - - - - - - - - - - - - - - - - - - -\n");
}


CoreAudioPump::CoreAudioPump () 
    : Pump(SF_F32, 0), _curFrame(0)
{
    AC_INFO << "CoreAudioPump::CoreAudioPump";
   
    setDeviceName("CoreAudio Sound Device");
    
    setCardName("Dummy Sound Card"); // TODO

    _myOutputBuffer.init(512, getNumOutputChannels(), getNativeSampleRate());

    dumpState();
    AudioTimeSource::run();
    _myRunning = true;
    // setup coreaudio callbacks
    OSStatus err = noErr;
        
    err = setupAudioUnit();
    checkStatus(err);    

    err = matchAUFormats(&_myOutputDesc);
    checkStatus(err);

    //err = getFileInfo(&fileRef, &musicFileID, &fileASBD, fileName);
    //checkStatus(err);    

    //err = MakeAUConverter(&musicFileID, &converter,&fileASBD, &_myOutputDesc );
    //checkStatus(err);

    err = setupCallbacks();
    checkStatus(err);

    //printf("\n\nOutput format of Audio is:\n");
    PrintStreamDesc(&_myOutputDesc);
    err =AudioOutputUnitStart(_myOutputUnit);

    checkStatus(err);
    AC_DEBUG << "Play has started";
}

CoreAudioPump::~CoreAudioPump () {
    AC_INFO << "CoreAudioPump::~CoreAudioPump";
    AudioTimeSource::stop();
    _myRunning = false;
    // teardown coreaudio callbacks
    cleanUp();
}

Time CoreAudioPump::getCurrentTime () {
    return Time();
}

OSStatus 
CoreAudioPump::setupAudioUnit(){
    OSStatus err;

    //An Audio Unit is a OS component
    //The component description must be setup, then used to 
    //initialize an AudioUnit
    ComponentDescription desc;  


    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_DefaultOutput;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;


    Component comp = FindNextComponent(NULL, &desc);  //Finds an component that meets the desc spec's
    if (comp == NULL) {
        throw CoreAudioException("could not find matching audio component", PLUS_FILE_LINE);
    }

    err = OpenAComponent(comp, &_myOutputUnit);  //gains access to the services provided by the component
    checkStatus(err);
    // if (err)  exit (-1);

    // Initialize AudioUnit 
    err = AudioUnitInitialize(_myOutputUnit);

    return err;


}

OSStatus 
CoreAudioPump::fileRenderProc(void             *inRefCon, 
        AudioUnitRenderActionFlags    *inActionFlags,
        const AudioTimeStamp       *inTimeStamp, 
        UInt32                                  inBusNumber,
        UInt32                                  inNumFrames, 
        AudioBufferList                         *ioData)
{
    CoreAudioPump * mySelf = reinterpret_cast<CoreAudioPump*>(inRefCon);
    return mySelf->pump(inActionFlags, inTimeStamp, inBusNumber, inNumFrames, ioData);
}
    
OSStatus 
CoreAudioPump::pump(AudioUnitRenderActionFlags    *inActionFlags,
        const AudioTimeStamp       *inTimeStamp, 
        UInt32                                  inBusNumber,
        UInt32                                  inNumFrames, 
        AudioBufferList                         *ioData)
{
    OSStatus err= noErr;
    mix(_myOutputBuffer, inNumFrames);
    int myCurFrame = 0;
    int curBuffer = 0;
    while (myCurFrame != inNumFrames) {
        ::AudioBuffer & myCoreAudioBuffer = ioData->mBuffers[curBuffer];
        int myFramesToWriteInBuffer = min(inNumFrames-myCurFrame, 
                myCoreAudioBuffer.mDataByteSize / _myOutputDesc.mBytesPerFrame);
        _myOutputBuffer.copyToRawMem(myCoreAudioBuffer.mData, 
                myCurFrame*_myOutputDesc.mBytesPerFrame, 
                myFramesToWriteInBuffer*_myOutputDesc.mBytesPerFrame);
        myCurFrame += myFramesToWriteInBuffer;
    }
    
#if 0    
    void *inInputDataProcUserData=NULL;
    AudioStreamPacketDescription* outPacketDescription =NULL;
    err = AudioConverterFillComplexBuffer(converter, ACComplexInputProc ,inInputDataProcUserData , &inNumFrames, ioData, outPacketDescription);

    /*Parameters for AudioConverterFillComplexBuffer()

      converter - the converter being used

      ACComplexInputProc() - input procedure to supply data to the Audio Converter

      inInputDataProcUserData - Used to hold any data that needs to be passed on.  Not needed in this example.

      inNumFrames - The amount of requested data.  On output, this
      number is the amount actually received.

      ioData - Buffer of the converted data recieved on return

      outPacketDescription - contains the format of the returned data.  Not used in this example.
     */

    checkStatus(err);
#endif
    return err;
}

OSStatus
CoreAudioPump::matchAUFormats (AudioStreamBasicDescription *theDesc)
{
    UInt32 size = sizeof (AudioStreamBasicDescription);
    memset(theDesc, 0, size);
    ::Boolean             outWritable;                            

    //Gets the size of the Stream Format Property and if it is writable
    AudioUnitGetPropertyInfo(_myOutputUnit,  
            kAudioUnitProperty_StreamFormat,
            kAudioUnitScope_Output, 
            0, 
            &size, 
            &outWritable);
    //Get the current stream format of the output
    OSStatus result = AudioUnitGetProperty (_myOutputUnit,
            kAudioUnitProperty_StreamFormat,
            kAudioUnitScope_Output,
            0,
            theDesc,
            &size);
    checkStatus(result); 
    theDesc->mSampleRate = getNativeSampleRate();
    //Set the stream format of the output to match the input
    result = AudioUnitSetProperty (_myOutputUnit,
            kAudioUnitProperty_StreamFormat,
            kAudioUnitScope_Input,
            0, // output
            theDesc,
            size);


    return result;
} 

OSStatus
CoreAudioPump::setupCallbacks() {
    OSStatus err= noErr;

    memset(&renderCallback, 0, sizeof(AURenderCallbackStruct));

    renderCallback.inputProc = &fileRenderProc;
    renderCallback.inputProcRefCon = this;

    //Sets the callback for the Audio Unit 
    err = AudioUnitSetProperty (_myOutputUnit, 
            kAudioUnitProperty_SetRenderCallback, 
            kAudioUnitScope_Input, 
            0,
            &renderCallback, 
            sizeof(AURenderCallbackStruct));

    checkStatus(err);

    return err;
}

void 
CoreAudioPump::cleanUp(){
    AC_DEBUG << "finished playing";

    AudioOutputUnitStop(_myOutputUnit);//you must stop the audio unit
    AudioUnitUninitialize (_myOutputUnit);
    CloseComponent(_myOutputUnit);
}

#if 0
void CoreAudioPump::pump()
{
    static Time lastTime;
    
    msleep(unsigned(1000*getLatency()));
    Time curTime;
    double TimeSinceLastPump = curTime-lastTime;
    unsigned numFramesToDeliver = unsigned(TimeSinceLastPump*getNativeSampleRate());
    lastTime = curTime;
    AC_TRACE << "CoreAudioPump::pump: numFramesToDeliver=" << numFramesToDeliver;

    mix(_myOutputBuffer, numFramesToDeliver);
}
#endif
}

