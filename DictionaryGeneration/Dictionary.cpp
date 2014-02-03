//
//  Dictionary.cpp
//  PianoTranscriptionGUI
//
//  Created by Xinyuan Lai on 2/2/14.
//
//

#include "Dictionary.h"
#include <cmath>
#include <fstream>


Dictionary::Dictionary(std::string samplePath,int fftSize, int nNotes, int downsampleRate, float secsPerBlock) :
    FFT_SIZE(fftSize),
    N_NOTES(nNotes),
    DOWNSAMPLE_RATE(downsampleRate),
    SECS_PER_BLOCK(secsPerBlock)
{
    sampleFileFolder = samplePath;
    
    W = new float*[FFT_SIZE/2];
    for (int i=0; i<FFT_SIZE/2; i++) {
        W[i] = new float[N_NOTES];
    }
    
    audio = new complex<float>[FFT_SIZE];
    spectrum = new complex<float>[FFT_SIZE];
}

Dictionary::~Dictionary()
{
    delete [] audio;
    delete [] spectrum;
    
    for (int i=0; i<FFT_SIZE/2; i++) {
        delete W[i];
    }
    delete [] W;
}

void Dictionary::createDictionary(std::string outputDir)
{
    //open output file
    std::fstream outputFile;
    outputFile.open(outputDir+"W.txt", std::ios::out);
    if (!outputFile.is_open()) {
        std::cout << "Text file open error!";
        return;
    }
    
    //prepare to read file
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    kiss_fft_cfg fwd= kiss_fft_alloc(FFT_SIZE,0,NULL,NULL);
    
    // HARD CODING HERE. REQUIRES MODIFICATION
    float fs = 44100;
    int nChannels = 2;
    numSamplesToRead = round(fs*SECS_PER_BLOCK);
    numSamplesPerBlock = numSamplesToRead/DOWNSAMPLE_RATE;
    
    AudioSampleBuffer buffer(nChannels, numSamplesToRead);
    float* audioBuffer = new float[numSamplesToRead];
    
    for (int track = 21; track<=108; track++) {
        
        // ANY BETTER WAYS TO DEAL WITH FILENAMES??
        std::string strFileDir = sampleFileFolder + "MAPS_ISOL_NO_M_S0_M"+std::to_string(track)+"_AkPnBcht.wav";
        File audioFileS0(strFileDir);
        
        AudioFormatReader* reader;
        
        if (audioFileS0.exists()) {
            reader = formatManager.createReaderFor(audioFileS0);
        }
        else {
            strFileDir = sampleFileFolder + "MAPS_ISOL_NO_M_S1_M"+std::to_string(track)+"_AkPnBcht.wav";
            File audioFileS1(strFileDir);
            reader = formatManager.createReaderFor(audioFileS1);
        }
        
        reader->read(&buffer, 0, numSamplesToRead, fs, true, true);
        
        float* leftBuffer = buffer.getSampleData(0);
        float* rightBuffer = buffer.getSampleData(1);
        
        for (int i=0; i<numSamplesToRead; i++) {
            audioBuffer[i] = (leftBuffer[i]+rightBuffer[i])/2;
        }
        
        antiAlias(audioBuffer, numSamplesToRead);
        
        
        for (int i=0; i<numSamplesPerBlock; i++) {
            audio[i].real( audioBuffer[i*DOWNSAMPLE_RATE]*hammWin(i, numSamplesPerBlock) );
            audio[i].imag(0);
        }
        for (int i = numSamplesPerBlock; i<FFT_SIZE; i++) {
            audio[i].real(0);
            audio[i].imag(0);
        }
        
        kiss_fft(fwd, (kiss_fft_cpx*) audio, (kiss_fft_cpx*)spectrum);
        for (int i = 0; i<FFT_SIZE/2; i++) {
            W[i][track-21] = abs(spectrum[i]);       //HARD CODING HERE!
        }
        
        delete reader;
        
    }
    delete [] audioBuffer;
    
    for(int i = 0; i< FFT_SIZE/2; i++){
        for (int j = 0; j<N_NOTES; j++) {
//            fwrite(&W[i][j], sizeof(float), 1, file);
            outputFile << W[i][j] << "\t";
        }
        outputFile<<std::endl;
    }
    
    outputFile.close();
    
}

void Dictionary::antiAlias(float *audioBuffer, int numSamples)
{
    float Q = 2;
    float K = tan(M_PI/DOWNSAMPLE_RATE);
    
    float b0 = K*K*Q/(K*K*Q+K+Q);
    float b1 = 2*K*K*Q/(K*K*Q+K+Q);
    float b2 = K*K*Q/(K*K*Q+K+Q);
    float a1 = 2*Q*(K*K-1)/(K*K*Q+K+Q);
    float a2 = (K*K*Q-K+Q)/(K*K*Q+K+Q);
    
    float* xh = new float[numSamples];
    
    xh[0] = audioBuffer[0];
    audioBuffer[0] = b0*xh[0];
    
    xh[1] = audioBuffer[1]-a1*xh[0];
    audioBuffer[1] = b0*xh[1] + b1*xh[0];
    
    for (int i = 2; i<numSamples; i++) {
        xh[i] = audioBuffer[i] - a1*xh[i-1] - a2*xh[i-2];
        audioBuffer[i] = b0*xh[i] + b1*xh[i-1] + b2*xh[i-2];
    }
    
    delete [] xh;
}

float Dictionary::hammWin(int nthPoint, int winLenth)
{
    return 0.54 - 0.46*cos( 2*M_PI*( (float)nthPoint/(winLenth-1) )  );
}