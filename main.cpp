#include "tgaimage.h"
#include <algorithm>
#include "model.h"

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

const TGAColor white = TGAColor({255, 255, 255, 255});
const TGAColor red = TGAColor({0, 0, 255, 255}); // TGAColor 里面是bgra... 不是rgba...

int main(int argc, char** argv){
    TGAImage image(100, 100, TGAImage::RGB);
    line(13, 20, 80, 40, image, white); 
    line(20, 13, 40, 80, image, red); 
    line(80, 40, 13, 20, image, red);
    image.write_tga_file(
        "output.tga"
    );
    return 0;
}

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if(std::abs(x0-x1) < std::abs(y0-y1)){
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if(x0 > x1){
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    // 尽量避免一切的浮点运算
    int dx = x1 - x0;
    int dy = y1 - y0;
    // k2 = k*dx*2
    int k2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    for(int x=x0; x<=x1;x += 1){
        if(steep){
            image.set(y,x,color);
        }else{
            image.set(x,y,color);
        }
        // 通过
        error2 += k2; 
        if (error2 > dx) { 
            y += (y1>y0?1:-1); 
            error2 -= dx * 2; 
        } 
    }
}