#include <stdio.h>
#include <stdlib.h>
#include "EasyBMP.h"
#include "bmpwrapper.h"

using namespace std;

int main(int argc, char* argv[]){
    if( argc != 5 ){
        cout << "Usage: compress <new_width> <new_height> <input_filename> <output_filename>"<< '\n';
        return 1;
    }
    int nw = atoi(argv[1]), nh = atoi(argv[2]), dw, dh, dmax, dmin;
    bool t=true;

    BMP* inputBmp = new BMP();
    inputBmp->ReadFromFile(argv[3]);
    BMPWrapper* input = new BMPWrapper(inputBmp, t);

    dw = max(0, input->TellWidth() - nw);
    dh = max(0, input->TellHeight() - nh);
    if(dw<dh) t=false;
    dmax = max(dw,dh);
    dmin = min(dw,dh);

    //compression
    int error=0;
    for(int j=0; j<dmax; j++) {
        input->simpleResize(t);
        error += dmin;
        if(2*error >= dmax) {
            input->simpleResize(!t);
            error -= dmax;
        }
    }
    //output
    input->getBMP()->WriteToFile(argv[4]);
    return 0;
}
