//
//  NMF.cpp
//  PianoTranscriptionGUI
//
//  Created by Xinyuan Lai on 2/3/14.
//
//

#include "NMF.h"

#include "NMF.h"
#include <iostream>
#include <fstream>
#include <cmath>

//using std::cout;
//using std::endl;

NMF::NMF(): SAMPLE_RATE(44100), DOWNSAMPLE_RATE(3), FFT_ORDER(10), FFT_SIZE(1024), N_NOTES(88), BETA(0.5), ITERATE_LIMIT(100), CONVERGE_THRESHOLD(0.01), ACTIVATION_TRESHOLD(0.15)
{
    
    W = new float*[FFT_SIZE/2];
    for (int i=0; i<FFT_SIZE/2; i++) {
        W[i] = new float[N_NOTES];
    }
    
    W_h = new float[FFT_SIZE/2];
    
    W_v_eT = new float*[FFT_SIZE/2];
    for (int i=0; i<FFT_SIZE/2; i++) {
        W_v_eT[i] = new float[N_NOTES];
    }
    
    numerator   = new float[N_NOTES];
    denominator = new float[N_NOTES];
    
    fftBuffer = new float[FFT_SIZE];
    v         = new float[FFT_SIZE/2];
    
    // read in dictionary from file
    std::ifstream fin;
    std::string line;
    fin.open("dictionary.txt");
    if (!fin.is_open())
        return;
    
    for (int col = 0; col<N_NOTES; col++) {
        for (int row = 0;row<FFT_SIZE/2; row++) {
            getline(fin, line);
            W[row][col] = std::stof(line);
        }
    }
    
    
//    std::fstream fout;
//    fout.open("WW.txt", std::ios::out);
//    for (int j = 0; j<N_NOTES; j++)
//    {
//        for (int i = 0; i<FFT_SIZE/2;i++) {
//            fout<<W[i][j]<<std::endl;
//        }
//    }
//    fout.close();
    
    
    fft = new SplitRadixFFT(FFT_ORDER);
}


NMF::~NMF()
{
    for (int i=0; i<FFT_SIZE/2; i++) {
        delete W[i];
    }
    delete []W;
    
    for (int i=0; i<FFT_SIZE/2; i++) {
        delete W_v_eT[i];
    }
    delete [] W_v_eT;
    
    delete [] W_h;
    
    delete [] numerator;
    delete [] denominator;
    
    delete [] fftBuffer;
    delete [] v;
    
    delete fft;
}


void NMF::Process(float *audioSamples, float *transcription, int nSamples)
{
    antiAlias(audioSamples, nSamples);
    addHammWindow(audioSamples, nSamples);
    downsampleFillBuffer(audioSamples, nSamples);
    
    
    fft->XForm(fftBuffer);
    

    for (int i = 0; i<FFT_SIZE/2; i++) {
        v[i] = abs(fftBuffer[i]);
//        std::cout<<"v"<<i<<": "<<v[i]<<std::endl;
    }
    
    factorize(transcription);
}

// low pass anti-aliasing filter
void NMF::antiAlias(float *audioSamples, int nSamples)
{
    float Q = 2;
    float K = tan(M_PI/DOWNSAMPLE_RATE);
    
    float b0 = K*K*Q/(K*K*Q+K+Q);
    float b1 = 2*K*K*Q/(K*K*Q+K+Q);
    float b2 = K*K*Q/(K*K*Q+K+Q);
    float a1 = 2*Q*(K*K-1)/(K*K*Q+K+Q);
    float a2 = (K*K*Q-K+Q)/(K*K*Q+K+Q);
    
    float xh  = 0;
    float xh1 = 0;
    float xh2 = 0;
    
    for (int i = 0; i<nSamples; i++) {
        xh = audioSamples[i] - a1*xh1 - a2*xh2;
        audioSamples[i] = b0*xh + b1*xh1 + b2*xh2;
        
        xh2 = xh1;
        xh1 = xh;
    }
    
    
}

void NMF::addHammWindow(float *audioSamples, int nSamples)
{
    for (int i = 0; i<nSamples; i++) {
        audioSamples[i] *= 0.54 - 0.46*cos( 2*M_PI*( (float)i/(nSamples-1) )  );
    }
}

void NMF::downsampleFillBuffer(float *audioSamples, int nSamples)
{
    // clear buffer
    for (int i=0; i<FFT_SIZE; i++) {
        fftBuffer[i] = 0;
    }
    
    int i = 0;
    while (i*DOWNSAMPLE_RATE < nSamples) {
        fftBuffer[i] = audioSamples[i*DOWNSAMPLE_RATE];
        i++;
    }
}




void NMF::factorize(float* h) //length(v)=FFT_SIZE/2; length(h)=N_NOTES;
{
    int count = 0;
    
    // initialize h
    for (int j = 0; j<N_NOTES; j++) {
        h[j] = 1;
    }
    
    // W.*(v*e')
    for (int i = 0; i<FFT_SIZE/2; i++) {
        for (int j = 0; j<N_NOTES; j++) {
            W_v_eT[i][j] = W[i][j]*v[i];
        }
    }
    
//    for (int i = 0; i<FFT_SIZE/2; i++) {
//        for (int j = 0; j<N_NOTES; j++) {
//            cout<<W_v_eT[i][j]<<"\t";
//        }
//        cout<<endl<<endl;
//    }
    
    // h = h.* ( ( W_v_eT'* (W*h).^(beta-2) ) ./ ( W'*(W*h).^(beta-1)) );
    while (getBetaDivergence(h) > CONVERGE_THRESHOLD && count++ < ITERATE_LIMIT) {
        
        // W*h
        for (int i=0; i<FFT_SIZE/2; i++) {
            W_h[i] = 0;
            for (int j=0; j<N_NOTES; j++) {
                W_h[i] += W[i][j]*h[j];
            }
        }
        
        
        
        // ( W_v_eT'* (W*h).^(beta-2)
        for (int j = 0; j<N_NOTES; j++) {
            numerator[j] = 0;
            for (int i = 0; i<FFT_SIZE/2; i++) {
                numerator[j] += W_v_eT[i][j] * pow(W_h[i], BETA-2.0);
            }
        }
        
        // ( W'*(W*h).^(beta-1))
        for (int j = 0; j<N_NOTES; j++) {
            denominator[j] = 0;
            for (int i = 0; i<FFT_SIZE/2; i++) {
                denominator[j] += W[i][j] * pow(W_h[i], BETA-1.0);
            }
        }
        
        // h = h.* ( ( W_v_eT'* (W*h).^(beta-2) ) ./ ( W'*(W*h).^(beta-1)) );
        for (int j = 0; j<N_NOTES; j++) {
            h[j] = h[j]*numerator[j]/denominator[j];
        }
        
//        for (int j = 0; j<N_NOTES; j++) {
//            std::cout<<h[j]<<"\t";
//        }
//        
//        std::cout<<std::endl<<std::endl;
        
    }
    
//    for (int j = 0; j<N_NOTES; j++) {
//        std::cout<<h[j]<<"\t";
//    }
    
    // normalization and dichotomize
    float max = 0;
    for (int j = 0; j<N_NOTES; j++) {
        if (h[j]>max) {
            max = h[j];
        }
    }
    for (int j = 0; j<N_NOTES; j++) {
        h[j] /= max;
    }
    for (int j = 0; j<N_NOTES; j++) {
        if (h[j] > ACTIVATION_TRESHOLD)
            h[j] = 1;
        else
            h[j] = 0;
    }
    
}



float NMF::getBetaDivergence(float *h)
{
    float db = 0;
    
    // W*h
    for (int i=0; i<FFT_SIZE/2; i++) {
        W_h[i] = 0;
        for (int j=0; j<N_NOTES; j++) {
            W_h[i] += W[i][j]*h[j];
        }
    }
    
    // db = sum (x./y - log(x./y) - 1);
    if (BETA == 0) {
        for (int i=0; i<FFT_SIZE/2; i++) {
            if (W_h[i]!=0) {
                db += v[i]/W_h[i] - log(v[i]/W_h[i]) - 1;
            }
        }
    }
    
    
    // db = sum( x.*log(x./y) + y - x );
    else if (BETA == 1){
        for (int i=0; i<FFT_SIZE/2; i++) {
            if (W_h[i]!=0) {
                db += v[i]*log(v[i]/W_h[i]) + W_h[i] - v[i];
            }
        }
    }
    
    
    // db = sum( 1/(beta*(beta-1)) * ( x.^beta + (beta-1)*y.^beta - beta.*x.*y.^(beta-1) ) );
    else {
        for (int i=0; i<FFT_SIZE/2; i++) {
            db += 1/(BETA*(BETA-1)) * ( pow(v[i], BETA) + (BETA-1)*pow(W_h[i], BETA) - BETA*v[i]*pow(W_h[i], BETA-1) );
        }
    }
    return db;
}