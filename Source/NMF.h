//
//  NMF.h
//  PianoTranscriptionGUI
//
//  Created by Xinyuan Lai on 2/3/14.
//
//

#ifndef __PianoTranscriptionGUI__NMF__
#define __PianoTranscriptionGUI__NMF__

#include <iostream>

#include "../JuceLibraryCode/JuceHeader.h"

#include "rsrfft.h"

#include <iostream>

class NMF
{
public:
    NMF();
    ~NMF();
    void Process(float* audioSamples, float* transcription, int nSamples);
    int getNumNotes(){return N_NOTES;};
    
protected:
    void antiAlias(float* audioSamples, int nSamples);
    void addHammWindow(float* audioSamples, int nSamples);
    void downsampleFillBuffer(float* audioSamples, int nSamples);
    
    void factorize(float* h);
    float getBetaDivergence(float* h);
    
private:
    float SAMPLE_RATE;
    int   DOWNSAMPLE_RATE;
    int   FFT_ORDER;
    int   FFT_SIZE;
    int   N_NOTES;
    float BETA;
    int   ITERATE_LIMIT;
    float CONVERGE_THRESHOLD;
    float ACTIVATION_TRESHOLD;
    
    SplitRadixFFT* fft;
    
    float** W;            // dictionary
    float*  W_h;          // W*h, intermediate variable for beta divergence calculation, 512*1
    float** W_v_eT;       // 512*88
    float*  numerator;    // 88*1
    float*  denominator;  // 88*1
    float*  fftBuffer;    // buffer for FFT
    float*  v;            // half the FFT point. For factorization.
    
};

#endif /* defined(__PianoTranscriptionGUI__NMF__) */
