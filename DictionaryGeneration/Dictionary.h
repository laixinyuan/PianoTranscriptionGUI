//
//  Dictionary.h
//  PianoTranscriptionGUI
//
//  Created by Xinyuan Lai on 2/2/14.
//
//

#ifndef __PianoTranscriptionGUI__Dictionary__
#define __PianoTranscriptionGUI__Dictionary__

#include <iostream>
#include <complex>
#include "../JuceLibraryCode/JuceHeader.h"
#include "../3rdPartyLibs/kiss_fft/kiss_fft.h"

using std::complex;

class Dictionary
{
public:
    Dictionary(std::string samplePath,int fftSize, int nNotes, int downsampleRate, float secsPerBlock);
    ~Dictionary();

    void createDictionary(std::string outputDir);
    void antiAlias(float* audioBuffer, int numSamples);
    float hammWin(int nthPoint, int winLenth);

    
private:
    float **W;
    const int FFT_SIZE;
    const int N_NOTES;
    const int DOWNSAMPLE_RATE;
    const float SECS_PER_BLOCK;
    int numSamplesToRead;
    int numSamplesPerBlock;
    
    complex<float>* audio;
    complex<float>* spectrum;
    
    std::string sampleFileFolder;
};

#endif /* defined(__PianoTranscriptionGUI__Dictionary__) */
