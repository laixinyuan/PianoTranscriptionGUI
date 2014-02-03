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

NMF::NMF()
{
    //TODO throw exception of file IO
    W = new float*[FFT_SIZE/2];
    for (int i=0; i<FFT_SIZE/2; i++) {
        W[i] = new float[N_NOTES];
    }
    FILE* file = fopen("W.dat", "r");
    for(int i = 0; i<FFT_SIZE/2; i++){
        for (int j = 0; j<N_NOTES; j++) {
            fread(&W[i][j], sizeof(float), 1, file);
        }
    }
    fclose(file);
    
    cnvgThreshold = 0.0001;
    beta = 1.5;
    noteThreshold = 0.7;
}

NMF::NMF(const char* filename)
{
    //TODO throw exception of file IO
    W = new float*[FFT_SIZE/2];
    for (int i=0; i<FFT_SIZE/2; i++) {
        W[i] = new float[N_NOTES];
    }
    FILE* file = fopen(filename, "r");
    for(int i = 0; i<FFT_SIZE/2; i++){
        for (int j = 0; j<N_NOTES; j++) {
            fread(&W[i][j], sizeof(float), 1, file);
        }
    }
    fclose(file);
    
    cnvgThreshold = 0.0001;
    beta = 1.5;
    noteThreshold = 0.7;
}

NMF::~NMF()
{
    for (int i=0; i<FFT_SIZE/2; i++) {
        delete W[i];
    }
    delete []W;
}


float NMF::betaDivergence(float beta, float *v, float *h)
{
    float db = 0;
    float WH[FFT_SIZE/2];
    
    //W*h
    for (int i=0; i<FFT_SIZE/2; i++) {
        for (int j=0; j<N_NOTES; j++) {
            WH[i] += W[i][j]*h[j];
        }
    }
    
    if (beta == 0) {
        for (int i=0; i<FFT_SIZE/2; i++) {
            if (WH[i]!=0) {
                db += v[i]/WH[i] - log(v[i]/WH[i]) - 1;
            }
        }
    }
    
    else if (beta == 1){
        for (int i=0; i<FFT_SIZE/2; i++) {
            if (WH[i]!=0) {
                db += v[i]*log(v[i]/WH[i]) + WH[i]  - v[i];
            }
        }
    }
    
    else {
        for (int i=0; i<FFT_SIZE/2; i++) {
            db += 1/(beta*(beta-1)) * ( pow(v[i], beta) + (beta-1)*pow(WH[i], beta) + beta*v[i]*pow(WH[i], beta-1) );
        }
    }
    return db;
}


void NMF::factorize(float* v, float* h) //length(v)=FFT_SIZE/2; length(h)=N_NOTES;
{
    float hp[N_NOTES] = {0};
    float Wmhp[FFT_SIZE/2] = {0};
    float numerator[N_NOTES] = {0};
    float denominator[N_NOTES] = {0};
    float WTmN[N_NOTES] = {0};
    float WTmD[N_NOTES] = {0};
    int count = 0;
    
    for (int j = 0; j<N_NOTES; j++) {
        h[j] = 1;
    }
    
    while (betaDivergence(beta, v, h) > cnvgThreshold && count < ITERATE_LIMIT) {
        // assign h to hp
        for (int i = 0; i<N_NOTES; i++) {
            hp[i] = h[i];
        }
        
        //(W*hp).^(beta-2).*v and (W*hp).^(beta-1)
        for (int i=0; i<FFT_SIZE/2; i++) {
            
            //W*hp
            for (int j = 0; j<N_NOTES; j++) {
                Wmhp[i] += W[i][j]*hp[j];
            }
            
            //(W*hp).^(beta-2).*v
            numerator[i]  = pow(Wmhp[i], beta-2.0)*v[i];
            //(W*hp).^(beta-1)
            denominator[i] = pow(Wmhp[i], beta-1.0);
        }
        
        //h  = hp .* ( ( W'*( (W*hp).^(beta-2).*v) ) ./ ( W'*(W*hp).^(beta-1)) );
        for (int j = 0; j<N_NOTES; j++) {
            //W'*numerator and W'*denumonator
            for (int i = 0; i<FFT_SIZE/2; i++) {
                WTmN[j] += W[i][j]*numerator[j];
                WTmD[j] += W[i][j]*denominator[j];
            }
            
            h[j] = hp[j]*WTmN[j]/WTmD[j];
        }
        count+=1;
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