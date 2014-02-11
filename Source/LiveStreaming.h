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

class LiveStreaming: public AudioIODeviceCallback
{
public:
    LiveStreaming(AudioDeviceManager& deviceManager);
    ~LiveStreaming();
    
    void audioDeviceIOCallback (const float **inputChannelData, int numInputChannels, float **outputChannelData, int numOutputChannels, int numSamples);
    void audioDeviceAboutToStart (AudioIODevice* device);;
    void audioDeviceStopped();
    
private:
    AudioDeviceManager& deviceManager;                          // global device manager
    AudioSourcePlayer audioSourcePlayer;
    AudioFormatReaderSource* fileSource;
    AudioTransportSource transportSource;
    AudioFormatManager formatManager;
    AudioSampleBuffer sampleBuffer;
    TimeSliceThread liveStreamingThread;
    
    NMF nmf;
    
    bool bufferReady;
    bool streamingAlive;
};

#endif /* defined(__PianoTranscriptionGUI__LiveStreaming__) */
