//
//  LiveStreaming.h
//  PianoTranscriptionGUI
//
//  Created by Xinyuan Lai on 2/8/14.
//
//

#ifndef __PianoTranscriptionGUI__LiveStreaming__
#define __PianoTranscriptionGUI__LiveStreaming__

#include <iostream>

#include "NMF.h"

#include "../JuceLibraryCode/JuceHeader.h"

#ifndef RECORD_SIZE
#define RECORD_SIZE 2205
#endif

#ifndef HOP_SIZE
#define HOP_SIZE 1024
#endif

class LiveStreaming: public AudioIODeviceCallback
{
public:
    LiveStreaming(AudioDeviceManager& deviceManager, NMF* nmf_, float* transcription_);
    ~LiveStreaming();
    
    void audioDeviceIOCallback (const float **inputChannelData, int numInputChannels, float **outputChannelData, int numOutputChannels, int numSamples);
    void audioDeviceAboutToStart (AudioIODevice* device);;
    void audioDeviceStopped();
    
    void loadBuffer();
    
private:
    AudioDeviceManager& deviceManager;
    TimeSliceThread liveStreamingThread;
    AudioSampleBuffer sampleBuffer = AudioSampleBuffer(1, RECORD_SIZE);
    AudioSampleBuffer calculateBuffer = AudioSampleBuffer(1,RECORD_SIZE);
    AudioSampleBuffer tempBuffer   = AudioSampleBuffer(1, RECORD_SIZE);
    
    NMF* nmf;
    float* transcription;
    float* nmfBuffer;
    
    bool bufferReady;
    bool streamingAlive;
    int bufferIndex;
};

#endif /* defined(__PianoTranscriptionGUI__LiveStreaming__) */
