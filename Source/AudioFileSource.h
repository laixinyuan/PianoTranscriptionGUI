//
//  FileStreaming.h
//  PianoTranscriptionGUI
//
//  Created by Xinyuan Lai on 2/9/14.
//
//

#ifndef __PianoTranscriptionGUI__FileStreaming__
#define __PianoTranscriptionGUI__FileStreaming__

#include <iostream>

#include "../JuceLibraryCode/JuceHeader.h"

#include "NMF.h"

#ifndef RECORD_SIZE
#define RECORD_SIZE 2205
#endif

#ifndef HOP_SIZE
#define HOP_SIZE 1024
#endif


class AudioFileSource : public AudioIODeviceCallback
{
public:
    AudioFileSource(AudioDeviceManager& deviceManager, NMF* nmf_, float* transcription_);
    ~AudioFileSource();
    void audioDeviceIOCallback(const float** inputChannelData,
							   int totalNumInputChannels,
							   float** outputChannelData,
							   int totalNumOutputChannels,
							   int numSamples);
    void audioDeviceAboutToStart (AudioIODevice* device);
    void audioDeviceStopped();
    void setFile(File audioFile);
    
    void loadBuffer();
    
private:
    
    AudioDeviceManager& deviceManager;                          // global device manager
    AudioSourcePlayer audioSourcePlayer;
    AudioFormatReaderSource* fileSource;
    AudioTransportSource transportSource;
    AudioFormatManager formatManager;
    TimeSliceThread playingThread;
    
    AudioSampleBuffer sampleBuffer = AudioSampleBuffer(1,RECORD_SIZE); //the buffer is for store;
    AudioSampleBuffer calculateBuffer = AudioSampleBuffer(1,RECORD_SIZE); //the buffer is throwing to the pitchtail
    AudioSampleBuffer tempBuffer = AudioSampleBuffer(1,RECORD_SIZE); // this buffer is for sliding buffer window
    
    NMF* nmf;
    float* nmfBuffer;
    float* transcription;
    
    bool bufferReady;
    int bufferIndex;
    
};


#endif /* defined(__PianoTranscriptionGUI__FileStreaming__) */
