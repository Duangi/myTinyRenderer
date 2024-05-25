#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        } 
        else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vec2f uv;
            for(int i = 0; i < 2; i++) {
                iss >> uv[i];
            }
            tex_coords_.push_back({uv.x,1 - uv.y});
        }
        else if (!line.compare(0, 2, "f ")) {
            std::vector<int> face,tex;
            int f,t;
            int itrash;
            iss >> trash;
            while (iss >> f >> trash >> t >> trash >> itrash) {
                f--; // in wavefront obj all indices start at 1, not zero
                face.push_back(f);
                t--;
                tex.push_back(t);
            }
            faces_.push_back(face);
            tex_faces_.push_back(tex);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << " vt# " << tex_coords_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

int Model::ntex_coords() {
    return (int)tex_coords_.size();
}

int Model::ntex_faces() {
    return (int)tex_faces_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

std::vector<int> Model::tex_face(int idx){
    return tex_faces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec2f Model::tex_coord(int i){
    return tex_coords_[i];
}