#include "EasyBMP.h"
#include <stdlib.h>

class BMPWrapper {
    public:
        BMPWrapper(BMP* bmp, bool d);
        ~BMPWrapper();
        int TellWidth();
        int TellHeight();
        int TellBitDepth();
        void simpleResize(int direction);
        BMP* getBMP();
        void computeEnergy();

    private:
        int start_w;
        bool d;
        BMP* bmp;
        int** energy;

        void setPixel(int i, int j, RGBApixel p);
        int setEnergy(int i, int j, int val);
        void setSize(int i, int j);
        void setBitDepth(int a);

        RGBApixel getPixel(int i, int j);
        int getEnergy(int i, int j);

        void deleteChain(int* chain);
        void recomputeEnergy(int* chain);
};


inline int MIN3(int a, int b, int c) {
    return std::min(a,std::min(b,c));
}


