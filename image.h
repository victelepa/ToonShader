#pragma once
#include <vector>
#include <fstream>
#include <cstdint>

struct ImageRGB {
    int w=0,h=0;
    std::vector<uint8_t> data;
    ImageRGB(int W,int H):w(W),h(H),data(3*W*H,0){}
    inline uint8_t* px(int x,int y){ return &data[3*(y*w + x)]; }
    bool save_ppm(const char* path) const {
        std::ofstream f(path, std::ios::binary);
        if(!f) return false;
        f << "P6\n" << w << " " << h << "\n255\n";
        f.write(reinterpret_cast<const char*>(data.data()), data.size());
        return true;
    }
};
