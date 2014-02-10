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

class NMF
{
public:
    NMF();
    ~NMF();
    void factorize(float* v, float* h);
    float getBetaDivergence(float* v, float* h);
    
    
private:
    float **W;
    const int FFT_SIZE;
    const int N_NOTES;
    const float BETA;
    const int ITERATE_LIMIT;
    const float CONVERGE_THRESHOLD;
    const float ACTIVATION_TRESHOLD;
    
    float*  W_h;          // W*h, intermediate variable for beta divergence calculation, 512*1
    float** W_v_eT;       // 512*88
    float*  numerator;    // 88*1
    float*  denominator;  // 88*1
};

#endif /* defined(__PianoTranscriptionGUI__NMF__) */
