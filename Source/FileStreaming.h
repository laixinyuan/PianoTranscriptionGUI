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


class AudioFileSource : public AudioIODeviceCallback
{
public:
    AudioFileSource(AudioDeviceManager& deviceManager);
    ~AudioFileSource();
    void audioDeviceIOCallback(const float** inputChannelData,
							   int totalNumInputChannels,
							   float** outputChannelData,
							   int totalNumOutputChannels,
							   int numSamples);
    void audioDeviceAboutToStart (AudioIODevice* device);
    void audioDeviceStopped();
    void setFile(File audioFile);
    
    
private:
    
    AudioDeviceManager& deviceManager;                          // global device manager
    AudioSourcePlayer audioSourcePlayer;
    AudioFormatReaderSource* fileSource;
    AudioTransportSource transportSource;
    AudioFormatManager formatManager;
    TimeSliceThread playingThread;
    
    AudioSampleBuffer sampleBuffer = AudioSampleBuffer(1,1024); //the buffer is for store;
    AudioSampleBuffer calculateBuffer = AudioSampleBuffer(1,1024); //the buffer is throwing to the pitchtail
    AudioSampleBuffer tempBuffer = AudioSampleBuffer(1,1024); // this buffer is for sliding buffer window
    bool bufferReady;
    int bufferIndex;
    
};


#endif /* defined(__PianoTranscriptionGUI__FileStreaming__) */
