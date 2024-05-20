#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "geometry.h"
#include <algorithm>
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(p0.x-p1.x)<std::abs(p0.y-p1.y)) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x>p1.x) {
        std::swap(p0, p1);
    }

    for (int x=p0.x; x<=p1.x; x++) {
        float t = (x-p0.x)/(float)(p1.x-p0.x);
        int y = p0.y*(1.-t) + p1.y*t;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}

float cross(Vec2i a,Vec2i b){
    return a.x * b.y - a.y * b.x;
}

// P点是否在三角形内
bool isInsideTriangle(Vec2i *tri, Vec2i P){
    Vec2i A = tri[0];
    Vec2i B = tri[1];
    Vec2i C = tri[2];
    
    Vec2i AB = B - A;
    Vec2i BC = C - B;
    Vec2i CA = A - C;

    Vec2i AP = P - A;
    Vec2i BP = P - B;
    Vec2i CP = P - C;

    float cross1 = cross(AB,AP);
    float cross2 = cross(BC,BP);
    float cross3 = cross(CA,CP);

    return (cross1 >= 0 && cross2 >=0 && cross3 >= 0) || (cross1 <= 0 && cross2 <= 0 && cross3 <= 0);
}

void triangle(Vec2i *pts, TGAImage &image, TGAColor color) { 
    // sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!) 
    Vec2i bboxmin(image.get_width()-1,  image.get_height()-1); 
    Vec2i bboxmax(0, 0); 
    Vec2i clamp(image.get_width()-1, image.get_height()-1); 
    
    for (int i=0; i<3; i++) { 
        bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
        bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));

        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
    } 
    Vec2i P; 
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) { 
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) { 
            if (!isInsideTriangle(pts,P)) continue; 
            image.set(P.x, P.y, color); 
        } 
    } 
}

int main(int argc, char** argv) {
    TGAImage frame(width, height, TGAImage::RGB); 
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }
    for (int i=0; i<model->nfaces(); i++) { 
        std::vector<int> face = model->face(i); 
        Vec2i screen_coords[3]; 
        for (int j=0; j<3; j++) { 
            Vec3f world_coords = model->vert(face[j]); 
            screen_coords[j] = Vec2i((world_coords.x+1.)*width/2., (world_coords.y+1.)*height/2.); 
        } 
        triangle(screen_coords, frame, TGAColor(rand()%255, rand()%255, rand()%255, 255)); 
    }
    frame.flip_vertically();
    frame.write_tga_file("framebuffer.tga");
    return 0; 
}