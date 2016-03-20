#include "bmpwrapper.h"

int diff(RGBApixel a, RGBApixel b){
    return abs(a.Blue-b.Blue) + abs(a.Green-b.Green) + abs(a.Red-b.Red);
}

int ind_of_min(int* s, int n){
    int m = s[0], ind = 0;
    for(int i = 0; i < n; i++){
        if(s[i] < m){
            m = s[i];
            ind = i;
        }
    }
    return ind;
}

int ind_of_min_neigh(int* s, int n, int j){
    if(j == 0) return(s[0]<s[1]?0:1);
    if(j == n-1) return(s[n-1]<s[n-2]?(n-1):(n-2));
    int m = MIN3(s[j-1], s[j], s[j+1]);
    return (s[j-1] == m)?(j-1) : (s[j] == m)?j : (j+1);
}

BMPWrapper::BMPWrapper(BMP* bmp, bool d) {
    this->bmp=bmp;
    this->d=d;
    this->computeEnergy();
}

BMPWrapper::~BMPWrapper(){
    for(int i=0; i<start_w; i++) delete[] energy[i];
    delete bmp;
    delete[] energy;
}

int BMPWrapper::TellWidth() {
    if(d) return bmp->TellWidth();
    else return bmp->TellHeight();
}
int BMPWrapper::TellHeight() {
    if(!d) return bmp->TellWidth();
    else return bmp->TellHeight();
}
int BMPWrapper::TellBitDepth() {
    return bmp->TellBitDepth();
}

void BMPWrapper::simpleResize(int direction) {
    d=direction;
    int w = TellWidth(), h = TellHeight();

    //energyMap[heigh][width] - minimal energy of way
    int** energyMap = new int* [h];
    for(int i = 0; i < h; i++)
        energyMap[i] = new int[w];

    //Compute map
    for(int j = 0; j < w; j++) energyMap[0][j] = energy[0][j];
    for(int i = 1; i < h; i++){
        energyMap[i][0] = getEnergy(0, i) + std::min(energyMap[i-1][0], energyMap[i-1][1]);
        for(int j = 1; j < w-1; j++)
             energyMap[i][j] = getEnergy(j, i) + MIN3(energyMap[i-1][j-1], energyMap[i-1][j], energyMap[i-1][j+1]);
        energyMap[i][w-1] = getEnergy(w-1, i) + std::min(energyMap[i-1][w-2], energyMap[i-1][w-1]);
    }

    //Find the vertical chain with minimal energy
    int* chain = new int[h];
    chain[h-1] = ind_of_min(energyMap[h-1], w);
    for(int i = h-2; i >= 0; i--){
        chain[i] = ind_of_min_neigh(energyMap[i], w, chain[i+1]);
    }

    for(int i=0; i<h; i++) delete[] energyMap[i];
    delete[] energyMap;
    deleteChain(chain);
}

BMP* BMPWrapper::getBMP() {
    return bmp;
}

void BMPWrapper::setPixel(int i, int j, RGBApixel p){
    if(d)  bmp->SetPixel(i, j, p);
    else  bmp->SetPixel(j, i, p);
}

void BMPWrapper::setEnergy(int i, int j, int val){
    if(d) energy[i][j] = val;
    else energy[j][i] = val;
}

void BMPWrapper::setSize(int i, int j){
    if(d) bmp->SetSize(i, j);
    else bmp->SetSize(j, i);
}

void BMPWrapper::setBitDepth(int a){
    bmp->SetBitDepth(a);
}

RGBApixel BMPWrapper::getPixel(int i, int j){
    if(d) return bmp->GetPixel(i, j);
    else return bmp->GetPixel(j, i);
}

int BMPWrapper::getEnergy(int i, int j){
    if(d) return energy[i][j];
    else return energy[j][i];
}

void BMPWrapper::deleteChain(int* chain){
    int w = TellWidth(), h = TellHeight(), bd = TellBitDepth();

    BMP* ni = new BMP();
    if(d)ni->SetSize(w-1, h);
    else ni->SetSize(h, w-1);
    ni->SetBitDepth(bd);

    //Chain carving
    RGBApixel pixel;
    for(int i = 0; i<h; i++){
        for(int j = 0; j < chain[i]; j++){
             pixel = getPixel(j, i);
             if(d)ni->SetPixel(j, i, pixel);
             else ni->SetPixel(i, j, pixel);
        }
        for(int j = chain[i]+1; j < w; j++){
             pixel = getPixel(j, i);
             if(d)ni->SetPixel(j-1, i, pixel);
             else ni->SetPixel(i, j-1, pixel);
        }
    }
    delete bmp;
    bmp = ni;
    recomputeEnergy(chain);
    delete[] chain;
}

void BMPWrapper::computeEnergy(){
    start_w = TellWidth();
    energy = new int* [TellWidth()];
    for(int i = 0; i < TellWidth(); i++)
        energy[i] = new int [TellHeight()];

    for(int i = 0; i < TellWidth(); i++){
        for(int j = 0; j < TellHeight(); j++){
            int t = 0;
            RGBApixel pixel = getPixel(i, j);
            if(i>0) t+= diff(pixel, getPixel(i-1,j));
            if(i<TellWidth()-1) t+= diff(pixel, getPixel(i+1,j));
            if(j>0) t+= diff(pixel, getPixel(i, j-1));
            if(j<TellHeight()-1) t+= diff(pixel, getPixel(i, j+1));
            setEnergy(i, j, t);
        }
    }
}

void BMPWrapper::recomputeEnergy(int* chain){
    for(int j = 0; j < TellHeight(); j++){
        int i, t;
        //Shift neighbours (carve the chain)
        for(int k = chain[j]+1; k<TellWidth(); k++)
            setEnergy(k, j, getEnergy(k+1, j));
        //Recompute energy near the chain
        for(int r = 0; r<2; r++){
            i = chain[j]-r;
            if(i > -1 && i < TellWidth()){
                t = 0;
                RGBApixel pixel = getPixel(i, j);
                if(i>0) t+= diff(pixel, getPixel(i-1,j));
                if(i<TellWidth()-1) t+= diff(pixel, getPixel(i+1,j));
                if(j>0) t+= diff(pixel, getPixel(i, j-1));
                if(j<TellHeight()-1) t+= diff(pixel, getPixel(i, j+1));
                setEnergy(i, j, t);
            }
        }
    }
}


