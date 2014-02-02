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
#include "kiss_fft.h"
#include "kiss_fftr.h"

class Dictionary
{
public:
    Dictionary();
    ~Dictionary();
    
private:
    float **W;
    const int FFT_SIZE;
    const int DOWNSAMPLE_RATE;
    const float SECS_PER_BLOCK;
    const float HOP_IN_SECS;
};

#endif /* defined(__PianoTranscriptionGUI__Dictionary__) */
