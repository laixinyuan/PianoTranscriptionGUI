//
//  FileStreaming.cpp
//  PianoTranscriptionGUI
//
//  Created by Xinyuan Lai on 2/9/14.
//
//

#include "AudioFileSource.h"

AudioFileSource::AudioFileSource(AudioDeviceManager& deviceManager_, NMF* nmf_, float* transcription_):deviceManager(deviceManager_), playingThread("file audio IO")
{
    AudioDeviceManager::AudioDeviceSetup config;
    deviceManager.getAudioDeviceSetup(config);
    config.bufferSize = HOP_SIZE;
    
    deviceManager.setAudioDeviceSetup(config, true);
    
    formatManager.registerBasicFormats();
    audioSourcePlayer.setSource(&transportSource);
    deviceManager.addAudioCallback(this);
    playingThread.startThread();
    
    bufferReady = false;
    
    nmf = nmf_;
    transcription = transcription_;
    
    N_NOTES = nmf->getNumNotes();
    
    nmfBuffer = new float[RECORD_SIZE];
    h   = new float[N_NOTES];
    hp  = new float[N_NOTES];
    hpp = new float[N_NOTES];
    
    for (int j = 0; j<N_NOTES; j++) {
        h[j] = 0;
        hp[j] = 0;
        hpp[j] = 0;
    }
}

AudioFileSource::~AudioFileSource()
{
    deviceManager.removeAudioCallback(this);
    transportSource.setSource(0);
    deleteAndZero(fileSource);
    audioSourcePlayer.setSource(0);
    
    nmf = nullptr;
    transcription = nullptr;
    delete [] nmfBuffer;
    delete [] h;
    delete [] hp;
    delete [] hpp;
}

void AudioFileSource::setFile(File audioFile)
{
    AudioFormatReader* tempReader = formatManager.createReaderFor(audioFile);
    fileSource = new AudioFormatReaderSource(tempReader,true);
    transportSource.setSource(fileSource,32768,&playingThread,tempReader->sampleRate);
    transportSource.start();
}

void AudioFileSource::audioDeviceIOCallback(const float** inputChannelData,
                                            int totalNumInputChannels,
                                            float** outputChannelData,
                                            int totalNumOutputChannels,
                                            int numSamples)
{
    //pass the output to the player
    
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
        sampleBuffer.copyFrom(0, 0, outputChannelData[0], numSamples);
        tempBuffer.copyFrom(0, 0, calculateBuffer, 0, numSamples, RECORD_SIZE - numSamples);
        calculateBuffer.clear();
        tempBuffer.copyFrom(0, RECORD_SIZE - numSamples, sampleBuffer, 0, 0, numSamples);
        calculateBuffer.copyFrom(0, 0, tempBuffer, 0, 0, RECORD_SIZE);
        tempBuffer.clear();
        bufferReady = true;
        sampleBuffer.clear();
    }
    
//    calculateBuffer.copyFrom(0, 0, outputChannelData[0], numSamples);
//    loadBuffer();
//    nmf->Process(nmfBuffer, h, RECORD_SIZE);
//    memcpy(transcription, h, N_NOTES*sizeof(float));
    
    audioSourcePlayer.audioDeviceIOCallback (inputChannelData, totalNumInputChannels, outputChannelData, totalNumOutputChannels, numSamples);
    
}

void AudioFileSource::loadBuffer()
{
    float* calBuffer = calculateBuffer.getSampleData(0);
    for (int i = 0; i<RECORD_SIZE; i++) {
        nmfBuffer[i] = calBuffer[i];
    }
}

float AudioFileSource::getMedian(float a, float b, float c)
{
    if ( (a - b) * (c - a) >= 0 ) // a >= b and a <= c OR a <= b and a >= c
        return a;
    else if ( (b - a) * (c - b) >= 0 ) // b >= a and b <= c OR b <= a and b >= c
        return b;
    else
        return c;
}

void AudioFileSource::audioDeviceAboutToStart(AudioIODevice* device)
{
    audioSourcePlayer.audioDeviceAboutToStart (device);
}

void AudioFileSource::audioDeviceStopped()
{
    audioSourcePlayer.audioDeviceStopped();
}