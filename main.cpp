#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <algorithm>
#include <iostream>

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

Vec3f cross(Vec3f a, Vec3f b){
    // n = (y1z2 - y2z1, x2z1-z2x1, x1y2 - x2y1)
    return Vec3f(a.y * b.z - b.y * a.z, b.x * a.z - b.z * a.x, a.x * b.y - b.x * a.y);
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

    float cross1 = cross(AB, AP);
    float cross2 = cross(BC, BP);
    float cross3 = cross(CA, CP);

    return (cross1 >= 0 && cross2 >=0 && cross3 >= 0) || (cross1 <= 0 && cross2 <= 0 && cross3 <= 0);
}

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    Vec3f s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void triangle(Vec3f *pts, Vec2f* tex, float *zbuffer, TGAImage &image, TGAImage &texture) {  
    
    Vec2f bboxmin(image.get_width()-1,  image.get_height()-1); 
    Vec2f bboxmax(0, 0); 
    Vec2f clamp(image.get_width()-1, image.get_height()-1); 
    
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::max(0.f,      std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    Vec3f P; 
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) { 
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) { 
            Vec3f bc_screen  = barycentric(pts[0], pts[1], pts[2], P);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            P.z = 0;
            Vec2f uv(0,0);
            for (int i=0; i<3; i++){
                P.z += pts[i][2]*bc_screen[i];
                uv = uv + tex[i] * bc_screen[i];
            } 
            if (zbuffer[int(P.x+P.y*width)]<P.z) {
                zbuffer[int(P.x+P.y*width)] = P.z;
                TGAColor color = texture.get(uv.x * texture.get_width(), uv.y * texture.get_height());
                image.set(P.x, P.y, color);
                // image.set(P.x, P.y, color);
            }
        } 
    } 
}

Vec3f world2screen(Vec3f v) {
    return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
}

int main(int argc, char** argv) {
    TGAImage frame(width, height, TGAImage::RGB); 
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

    TGAImage texture;
    if(!texture.read_tga_file("obj/african_head_diffuse.tga")){
        // std:cerr << "Error loading texture file" << std::endl;
        return -1;
    }
    // texture.flip_vertically();
    Vec3f light_dir(0,0,-1);
    float *zbuffer = new float[width*height];
    for(int i = width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());
    
    for (int i=0; i<model->nfaces(); i++) {
        
        std::vector<int> face = model->face(i);
        std::vector<int> tex_face = model->tex_face(i);
        Vec2i screen_coords[3]; 
        Vec3f world_coords[3]; 
        Vec2f tex_coords[3];
        Vec3f pts[3];
        for (int j=0; j<3; j++) { 
            Vec3f v = model->vert(face[j]); 
            screen_coords[j] = Vec2i((v.x+1.)*width/2., (v.y+1.)*height/2.); 
            world_coords[j]  = v;
            pts[j] = world2screen(model->vert(face[j]));
            tex_coords[j] = model -> tex_coord(tex_face[j]);
        } 
        // 
        Vec3f n = cross(world_coords[2]-world_coords[1], world_coords[1]-world_coords[0]); 
        n.normalize(); 
        float intensity = n*light_dir; 

        // 背面剔除
        if(intensity > 0 ) triangle(pts,tex_coords, zbuffer, frame, texture);
    }

    


    frame.flip_vertically();
    frame.write_tga_file("framebuffer.tga");
    delete[] zbuffer;
    return 0; 
}
