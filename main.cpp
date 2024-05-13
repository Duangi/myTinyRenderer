#include "tgaimage.h"
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

const TGAColor white = TGAColor({255, 255, 255, 255});
const TGAColor red = TGAColor({0, 0, 255, 255}); // TGAColor 里面是bgra... 不是rgba...

int main(int argc, char** argv){
    TGAImage image(100, 100, TGAImage::RGB);
    line(20,20,60,60,image,white);
    image.write_tga_file(
        "output.tga"
    );
    return 0;
}

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    for(float t=0.; t<1.;t += .01){
        int x = x0 + (x1-x0)*t;
        int y = y0 + (y1-y0)*t;
        image.set(x, y, color);
    }
}