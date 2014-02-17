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
    bufferIndex = 0;
    
    nmf = nmf_;
    transcription = transcription_;
    nmfBuffer = new float[RECORD_SIZE];
    h = new float[nmf->getNumNotes()];
    
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
        memcpy(transcription, h, nmf->getNumNotes()*sizeof(float));
//        for (int j = 0; j<nmf->getNumNotes(); j++) {
//            transcription[j] = h[j];
//        }
        
//        for (int j = 0; j<88; j++)
//            std::cout<<transcription[j]<<"\t";
//        std::cout<<std::endl;
        
//        std::cout<<"Process complete"<<std::endl;
//        for (int j = 0; j<88; j++) {
//            std::cout<<h[j]<<"\t";
//        }
//        std::cout<<std::endl<<std::endl;
        
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
    
    audioSourcePlayer.audioDeviceIOCallback (inputChannelData, totalNumInputChannels, outputChannelData, totalNumOutputChannels, numSamples);
    
}

void AudioFileSource::loadBuffer()
{
    float* calBuffer = calculateBuffer.getSampleData(0);
    for (int i = 0; i<RECORD_SIZE; i++) {
        nmfBuffer[i] = calBuffer[i];
    }
}

void AudioFileSource::audioDeviceAboutToStart(AudioIODevice* device)
{
    audioSourcePlayer.audioDeviceAboutToStart (device);
}

void AudioFileSource::audioDeviceStopped()
{
    audioSourcePlayer.audioDeviceStopped();
}