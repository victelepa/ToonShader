#pragma once
#include "params.h"

inline vec3 toon_shade(const vec3& N0, const vec3& V0, const vec3& L0) {
    vec3 N = normalize(N0);
    vec3 V = normalize(V0);
    vec3 L = normalize(L0);

    float ndotl = std::max(0.f, dot(N, L));
    float diff = ndotl;
    if (DIFFUSE_BANDS > 1) {
        diff = std::floor(diff * DIFFUSE_BANDS) / (float)(DIFFUSE_BANDS - 1);
        diff = clamp01f(diff);
    }
    vec3 base = BASE_COLOR * (0.4f + 0.6f*diff);

    vec3 H = normalize(L + V);
    float blinn = std::max(0.f, dot(N, H));
    float spec  = std::pow(blinn, 64.0f);
    float s = (SPEC_WIDTH <= 1e-6f) ? (spec >= SPEC_THRESH ? 1.f : 0.f)
                                    : clamp01f((spec - SPEC_THRESH) / std::max(1e-6f, SPEC_WIDTH));
    vec3 color = base + (HI_COLOR * (s * SPEC_STRENGTH));

    float nv = std::fabs(dot(N, V));
    float ink = 1.f - clamp01f((nv - NV_THRESH) / 0.05f);
    color = color * (1.f - 0.9f*ink);
    return clamp01(color);
}
