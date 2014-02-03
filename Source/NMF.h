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
#include <iostream>

#ifndef FFT_SIZE
#define FFT_SIZE          4096
#endif
#ifndef DOWN_SAMPLE_RATE
#define DOWN_SAMPLE_RATE  2
#endif
#ifndef N_NOTES
#define N_NOTES           88
#endif

class NMF
{
public:
    NMF();
    NMF(const char* filename);
    ~NMF();
    void factorize(float* v, float* h);
    float betaDivergence(float beta, float* v, float* h);
    
private:
    float **W;
    float cnvgThreshold;
    float beta;
    float noteThreshold;
//    static const int FFT_SIZE = 4096;
//    static const int DOWN_SAMPLE_RATE = 2;
//    static const int N_NOTES = 88;
//    static const int ITERATE_LIMIT = 100;
};

#endif /* defined(__PianoTranscriptionGUI__NMF__) */
