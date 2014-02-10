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

NMF::NMF(): FFT_SIZE(1024), N_NOTES(88), BETA(0.5), ITERATE_LIMIT(100), CONVERGE_THRESHOLD(0.01), ACTIVATION_TRESHOLD(0.15)
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
}


float NMF::getBetaDivergence(float *v, float *h)
{
    float db = 0;
    
    // W*h
    for (int i=0; i<FFT_SIZE/2; i++) {
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


void NMF::factorize(float* v, float* h) //length(v)=FFT_SIZE/2; length(h)=N_NOTES;
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
    
    // h = h.* ( ( W_v_eT'* (W*h).^(beta-2) ) ./ ( W'*(W*h).^(beta-1)) );
    while (getBetaDivergence(v, h) > CONVERGE_THRESHOLD && count++ < ITERATE_LIMIT) {
        
        // W*h
        for (int i=0; i<FFT_SIZE/2; i++) {
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
                denominator[j] = W[i][j] * pow(W_h[i], BETA-1.0);
            }
        }
        
        // h = h.* ( ( W_v_eT'* (W*h).^(beta-2) ) ./ ( W'*(W*h).^(beta-1)) );
        for (int j = 0; j<N_NOTES; j++) {
            h[j] = h[j]*numerator[j]/denominator[j];
        }
        
    }
    
    //normalization
    float max = 0;
    for (int j = 0; j<N_NOTES; j++) {
        if (h[j]>max) {
            max = h[j];
        }
    }
    for (int j = 0; j<N_NOTES; j++) {
        h[j] /= max;
    }
    
}