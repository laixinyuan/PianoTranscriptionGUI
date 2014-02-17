//
//  LiveStreaming.cpp
//  PianoTranscriptionGUI
//
//  Created by Xinyuan Lai on 2/8/14.
//
//

#include "LiveStreaming.h"

LiveStreaming::LiveStreaming(AudioDeviceManager& deviceManager_, NMF* nmf_):deviceManager(deviceManager_), liveStreamingThread("real time IO")
{
    AudioDeviceManager::AudioDeviceSetup config;
    deviceManager.getAudioDeviceSetup(config);
    config.bufferSize = HOP_SIZE;
    
    deviceManager.setAudioDeviceSetup(config, true);
    deviceManager.addAudioCallback(this);
    streamingAlive = true;
    bufferReady = false;
    
    nmf = nmf_;
    transcription = new float[nmf->getNumNotes()];
    nmfBuffer = new float[RECORD_SIZE];

}

LiveStreaming::~LiveStreaming()
{
    deviceManager.removeAudioCallback(this);
    streamingAlive = false;
    nmf = nullptr;
//    transcription = nullptr;
    delete [] transcription;
    delete [] nmfBuffer;
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
    if (bufferReady == true)
    {
        loadBuffer();
        nmf->Process(nmfBuffer, transcription, RECORD_SIZE);
        bufferReady = false;
    }
    
    if (bufferReady == false)
    {
        sampleBuffer.copyFrom(0, 0, inputChannelData[0], numSamples);
        tempBuffer.copyFrom(0, 0, calculateBuffer, 0, numSamples, RECORD_SIZE - numSamples);
        calculateBuffer.clear();
        tempBuffer.copyFrom(0, RECORD_SIZE - numSamples, sampleBuffer, 0, 0, numSamples);
        calculateBuffer.copyFrom(0, 0, tempBuffer, 0, 0, RECORD_SIZE);
        tempBuffer.clear();
        bufferReady = true;
        sampleBuffer.clear();
    }
    
    
    for (int i = 0; i < numSamples; ++i)
        for (int j = totalNumOutputChannels; --j >= 0;)
            outputChannelData[j][i] = 0;
    
}

void LiveStreaming::loadBuffer()
{
    float* calBuffer = calculateBuffer.getSampleData(0);
    for (int i = 0; i<RECORD_SIZE; i++) {
        nmfBuffer[i] = calBuffer[i];
    }
}