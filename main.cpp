#include "tgaimage.h"
#include <algorithm>
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

const TGAColor white = TGAColor({255, 255, 255, 255});
const TGAColor red = TGAColor({0, 0, 255, 255}); // TGAColor 里面是bgra... 不是rgba...

int main(int argc, char** argv){
    TGAImage image(100, 100, TGAImage::RGB);
    line(13, 20, 80, 40, image, white); 
    line(20, 13, 40, 80, image, red); 
    // line(80, 40, 13, 20, image, red);
    image.write_tga_file(
        "output.tga"
    );
    return 0;
}

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    float k = (y1 - y0) / (float)(x1 - x0);
    float b = y0 - k * x0;
    for(int x=std::min(x0,x1); x<=std::max(x0,x1);x += 1){
        float y = k*x + b;
        image.set(x,y,color);
    }
}