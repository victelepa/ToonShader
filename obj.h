#pragma once
#include "params.h"
#include <string>
#include <sstream>
#include <fstream>

struct Mesh {
    std::vector<vec3> positions;
    std::vector<vec3> normals;
    struct Tri { int v0,v1,v2; int n0,n1,n2; };
    std::vector<Tri> tris;
};

inline bool load_obj(const std::string& path, Mesh& out) {
    std::ifstream in(path);
    if(!in) return false;
    std::string line;
    std::vector<vec3> temp_pos; temp_pos.reserve(1000);
    std::vector<vec3> temp_nrm; temp_nrm.reserve(1000);

    auto parse_vi = [](const std::string& tok, int& vi, int& ni){
        vi = ni = -1;
        int s1 = tok.find('/');
        if (s1 == std::string::npos){ vi = std::stoi(tok); return; }
        int s2 = tok.find('/', s1+1);
        vi = std::stoi(tok.substr(0, s1));
        if (s2 != std::string::npos){
            std::string tail = tok.substr(s2+1);
            if (!tail.empty()) ni = std::stoi(tail);
        }
    };

    while(std::getline(in,line)){
        if(line.empty() || line[0]=='#') continue;
        std::istringstream ss(line);
        std::string key; ss>>key;
        if (key=="v"){
            float x,y,z; ss>>x>>y>>z; temp_pos.emplace_back(x,y,z);
        } else if (key=="vn"){
            float x,y,z; ss>>x>>y>>z; temp_nrm.emplace_back(x,y,z);
        } else if (key=="f"){
            std::vector<int> vIdx, nIdx; std::string tok;
            while (ss>>tok){
                int vi=-1, ni=-1; parse_vi(tok, vi, ni);
                if (vi<0) continue;
                vIdx.push_back(vi-1);
                nIdx.push_back(ni>0?ni-1:-1);
            }
            if (vIdx.size()<3) continue;
            for(size_t i=1;i+1<vIdx.size();i++){
                Mesh::Tri t{};
                t.v0=vIdx[0]; t.v1=vIdx[i]; t.v2=vIdx[i+1];
                t.n0=nIdx[0]; t.n1=nIdx[i]; t.n2=nIdx[i+1];
                out.tris.push_back(t);
            }
        }
    }
    out.positions = std::move(temp_pos);

    bool has_n=true;
    for (auto &tri: out.tris){
        if (tri.n0<0 || tri.n1<0 || tri.n2<0){ has_n=false; break; }
    }
    if (!has_n){
        out.normals.assign(out.positions.size(), vec3(0,0,0));
        for (auto &tri: out.tris){
            vec3 p0 = out.positions[tri.v0];
            vec3 p1 = out.positions[tri.v1];
            vec3 p2 = out.positions[tri.v2];
            vec3 fn = cross(p1-p0, p2-p0);
            out.normals[tri.v0] = out.normals[tri.v0] + fn;
            out.normals[tri.v1] = out.normals[tri.v1] + fn;
            out.normals[tri.v2] = out.normals[tri.v2] + fn;
            tri.n0 = tri.v0; tri.n1 = tri.v1; tri.n2 = tri.v2;
        }
        for (auto &n: out.normals) n = normalize(n);
    } else {
        out.normals = temp_nrm;
    }
    return true;
}
