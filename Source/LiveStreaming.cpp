//
//  LiveStreaming.cpp
//  PianoTranscriptionGUI
//
//  Created by Xinyuan Lai on 2/8/14.
//
//

#include "LiveStreaming.h"

LiveStreaming::LiveStreaming(AudioDeviceManager& deviceManager_, NMF* nmf_, float* transcription_):deviceManager(deviceManager_), liveStreamingThread("real time IO")
{
    AudioDeviceManager::AudioDeviceSetup config;
    deviceManager.getAudioDeviceSetup(config);
    config.bufferSize = HOP_SIZE;
    
    deviceManager.setAudioDeviceSetup(config, true);
    deviceManager.addAudioCallback(this);
    streamingAlive = true;
    bufferReady = false;
    
    nmf = nmf_;
    transcription = transcription_;
    N_NOTES = nmf->getNumNotes();
    nmfBuffer = new float[RECORD_SIZE];
    h = new float[N_NOTES];
    hp = new float[N_NOTES];
    hpp = new float[N_NOTES];

}

LiveStreaming::~LiveStreaming()
{
    deviceManager.removeAudioCallback(this);
    streamingAlive = false;
    nmf = nullptr;
    transcription = nullptr;
    delete [] nmfBuffer;
    delete [] h;
    delete [] hp;
    delete [] hpp;
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
        nmf->Process(nmfBuffer, h, RECORD_SIZE);
        
        // median filter
        for (int j = 0; j<N_NOTES; j++) {
            transcription[j] = getMedian(h[j], hp[j], hpp[j]);
        }
        
        memcpy(hpp, hp, N_NOTES*sizeof(float));
        memcpy(hp,  h,  N_NOTES*sizeof(float));

        
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

float LiveStreaming::getMedian(float a, float b, float c)
{
    if ( (a - b) * (c - a) >= 0 ) // a >= b and a <= c OR a <= b and a >= c
        return a;
    else if ( (b - a) * (c - b) >= 0 ) // b >= a and b <= c OR b <= a and b >= c
        return b;
    else
        return c;
}