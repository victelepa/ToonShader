#pragma once
#include <vector>
#include <fstream>
#include <cstdint>

/// @brief RGB 图片结构体
///
/// RGB image structure
struct ImageRGB {
    /// @brief 图片宽度
    ///
    /// Image width
    int w=0;

    /// @brief 图片高度
    ///
    /// Image height
    int h=0;

    /// @brief 像素数据，按行优先存储，每个像素3字节，顺序为R、G、B
    ///
    /// Pixel data, stored in row-major order, 3 bytes per pixel in R, G, B order
    std::vector<uint8_t> data;

    /// @brief 图片构造函数 默认为黑色的空图像
    ///
    /// Image constructor, defaults to a black empty image
    /// @param W 图片宽度 / Image width
    /// @param H 图片高度 / Image height
    ImageRGB(int W,int H):w(W),h(H),data(3*W*H,0){}

    /// @brief 获取特定位置的像素
    ///
    /// Get pixel pointer at specific position
    /// @param x 像素横坐标 / Pixel x coordinate
    /// @param y 像素纵坐标 / Pixel y coordinate
    /// @return 指向像素数据的指针 / Pointer to pixel data
    inline uint8_t* px(int x,int y){ return &data[3*(y*w + x)]; }

    /// @brief 将图片保存为 PPM 格式文件
    ///
    /// Save image as a PPM format file
    /// @param path 文件路径 / File path
    bool save_ppm(const char* path) const {
        std::ofstream f(path, std::ios::binary);
        if(!f) return false;
        f << "P6\n" << w << " " << h << "\n255\n";
        f.write(reinterpret_cast<const char*>(data.data()), data.size());
        return true;
    }
};
