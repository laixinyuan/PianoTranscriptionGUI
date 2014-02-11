//
//  LiveStreaming.cpp
//  PianoTranscriptionGUI
//
//  Created by Xinyuan Lai on 2/8/14.
//
//

#include "LiveStreaming.h"

LiveStreaming::LiveStreaming(AudioDeviceManager& deviceManager_):deviceManager(deviceManager_), sampleBuffer(1, 1024), liveStreamingThread("real time IO")
{
    AudioDeviceManager::AudioDeviceSetup config;
    deviceManager.getAudioDeviceSetup(config);
    config.bufferSize = 512;
    
    deviceManager.setAudioDeviceSetup(config, true);
    deviceManager.addAudioCallback(this);
    streamingAlive = true;
    bufferReady = false;
    

}

LiveStreaming::~LiveStreaming()
{
    deviceManager.removeAudioCallback(this);
    streamingAlive = false;
}

void LiveStreaming::audioDeviceAboutToStart(AudioIODevice* device)
{
    
}

void LiveStreaming::audioDeviceStopped()
{
    streamingAlive = false;
}

void LiveStreaming::audioDeviceIOCallback( const float** inputChannelData,
                                          int totalNumInputChannels,
                                          float** outputChannelData,
                                          int totalNumOutputChannels,
                                          int numSamples)
{
    
    //get sample here
//    if (bufferReady == true)
//    {
//        pitchTracking(calculateBuffer.getSampleData(0));
//        bufferReady = false;
//    }
//    
//    if (bufferReady == false)
//    {
//        sampleBuffer.copyFrom(0, 0, inputChannelData[0], numSamples);
//        tempBuffer.copyFrom(0, 0, calculateBuffer, 0, numSamples, RECORDSIZE - numSamples);
//        calculateBuffer.clear();
//        tempBuffer.copyFrom(0, RECORDSIZE - numSamples, sampleBuffer, 0, 0, numSamples);
//        calculateBuffer.copyFrom(0, 0, tempBuffer, 0, 0, RECORDSIZE);
//        tempBuffer.clear();
//        bufferReady = true;
//        sampleBuffer.clear();
//    }
    
    
    for (int i = 0; i < numSamples; ++i)
        for (int j = totalNumOutputChannels; --j >= 0;)
            outputChannelData[j][i] = 0;
    
}