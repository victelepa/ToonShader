#pragma once
#include "params.h"

/// @brief 卡通着色函数
///
/// Toon shading function
/// @param N0 法线向量 / Normal vector
/// @param V0 观察向量 / View vector
/// @param L0 光照向量 / Light vector
/// @return 颜色值 / Color value
inline vec3 toon_shade(const vec3& N0, const vec3& V0, const vec3& L0) {
    // 法线 / Normal
    vec3 N = normalize(N0);

    // 视线和光线方向 / View and Light directions
    vec3 V = normalize(V0);

    // 光线方向 / Light direction
    vec3 L = normalize(L0);

    // 漫反射计算 / Diffuse calculation
    float ndotl = std::max(0.f, dot(N, L));
    if (DIFFUSE_BANDS > 1) {
        ndotl = std::floor(ndotl * DIFFUSE_BANDS) / (float)(DIFFUSE_BANDS - 1);
        ndotl = clamp01f(ndotl);
    }
    vec3 base = BASE_COLOR * (0.4f + 0.6f*ndotl);

    // 高光计算 / Specular calculation

    // 半角向量 / Halfway vector
    vec3 H = normalize(L + V);

    // 法线与半角向量的点积 / Dot product of Normal and Halfway vector
    float blinn = std::max(0.f, dot(N, H));
    // 计算高光强度 / Compute specular intensity
    float spec  = std::pow(blinn, 64.0f);

    // 根据高光宽度和阈值计算高光系数 / Compute specular coefficient based on width and threshold
    float s = (SPEC_WIDTH <= 1e-6f) ? 
                //如果
                (spec >= SPEC_THRESH ? 1.f : 0.f)
                : clamp01f((spec - SPEC_THRESH) / std::max(1e-6f, SPEC_WIDTH));
    vec3 color = base + (HI_COLOR * (s * SPEC_STRENGTH));

    float nv = std::fabs(dot(N, V));
    float ink = 1.f - clamp01f((nv - NV_THRESH) / 0.05f);
    color = color * (1.f - 0.9f*ink);
    return clamp01(color);
}
