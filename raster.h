#pragma once
#include "params.h"
#include "image.h"
#include "toon.h"

struct Vertex { vec3 pos; vec3 nrm; };

struct Transforms { mat4 M, V, P, MVP; };

inline mat4 mulm(const mat4&A,const mat4&B){
    mat4 R{}; for(int c=0;c<4;c++) for(int r=0;r<4;r++){ R(r,c)=0; for(int k=0;k<4;k++) R(r,c)+=A(r,k)*B(k,c);} return R;
}

inline void raster_mesh(const std::vector<Vertex>& verts,
                        const std::vector<int>& indices,
                        const Transforms& T,
                        ImageRGB& img,
                        std::vector<float>& depth)
{
    auto to_clip = [&](const vec3& p)->vec4{
        return mul(T.MVP, vec4(p.x,p.y,p.z,1));
    };
    auto edge = [](float ax,float ay,float bx,float by,float px,float py){
        return (px-ax)*(by-ay) - (py-ay)*(bx-ax);
    };
    int W=img.w, H=img.h;

    for (size_t t=0; t<indices.size(); t+=3){
        int i0 = indices[t+0], i1 = indices[t+1], i2 = indices[t+2];
        vec3 p0 = verts[i0].pos, p1 = verts[i1].pos, p2 = verts[i2].pos;
        vec3 n0 = verts[i0].nrm, n1 = verts[i1].nrm, n2 = verts[i2].nrm;

        vec4 c0 = to_clip(p0), c1 = to_clip(p1), c2 = to_clip(p2);
        auto inside = [](const vec4& c){ return (c.z + c.w) >= 0 && (c.w - c.z) >= 0; };
        if (!inside(c0) && !inside(c1) && !inside(c2)) continue;

        float iw0=1.f/c0.w, iw1=1.f/c1.w, iw2=1.f/c2.w;
        float x0=(c0.x*iw0*0.5f+0.5f)*W, y0=(c0.y*iw0*0.5f+0.5f)*H;
        float x1=(c1.x*iw1*0.5f+0.5f)*W, y1=(c1.y*iw1*0.5f+0.5f)*H;
        float x2=(c2.x*iw2*0.5f+0.5f)*W, y2=(c2.y*iw2*0.5f+0.5f)*H;

        int minx = std::max(0, (int)std::floor(std::min({x0,x1,x2})));
        int maxx = std::min(W-1, (int)std::ceil (std::max({x0,x1,x2})) - 1);
        int miny = std::max(0, (int)std::floor(std::min({y0,y1,y2})));
        int maxy = std::min(H-1, (int)std::ceil (std::max({y0,y1,y2})) - 1);

        float area = edge(x0,y0,x1,y1,x2,y2);
        if (area == 0) continue;

        vec3 Vv = vec3(0,0,1);

        for (int y=miny; y<=maxy; ++y){
            for (int x=minx; x<=maxx; ++x){
                float px=x+0.5f, py=y+0.5f;
                float w0=edge(x1,y1,x2,y2,px,py), w1=edge(x2,y2,x0,y0,px,py), w2=edge(x0,y0,x1,y1,px,py);
                if (area>0 ? (w0<0||w1<0||w2<0) : (w0>0||w1>0||w2>0)) continue;

                float l0=w0/area, l1=w1/area, l2=w2/area;

                float invw = l0*iw0 + l1*iw1 + l2*iw2;
                float w = 1.0f / invw;

                float zndc = (l0*(c0.z*iw0) + l1*(c1.z*iw1) + l2*(c2.z*iw2)) * w;
                float z01  = zndc*0.5f + 0.5f;
                int idx = y*W + x;
                if (z01 >= depth[idx]) continue;
                depth[idx] = z01;

                vec3 n = (n0*(l0*iw0) + n1*(l1*iw1) + n2*(l2*iw2)) * w;
                n = normalize(n);

                vec3 color = toon_shade(n, Vv, LIGHT_DIR);
                auto* p = img.px(x,y);
                p[0] = (uint8_t)(clamp01f(color.x)*255);
                p[1] = (uint8_t)(clamp01f(color.y)*255);
                p[2] = (uint8_t)(clamp01f(color.z)*255);
            }
        }
    }
}

inline void post_edges(ImageRGB& img, const std::vector<float>& depth){
    int W=img.w, H=img.h;
    auto at=[&](int x,int y){return y*W+x;};
    const int dx[4]={1,-1,0,0};
    const int dy[4]={0,0,1,-1};
    for (int y=1;y<H-1;y++){
        for (int x=1;x<W-1;x++){
            int idx=at(x,y);
            float zc=depth[idx];
            if (!std::isfinite(zc)) continue;
            float eps = DEPTH_EPS * (zc*zc + 1e-6f);
            bool edge=false;
            for (int k=0;k<4;k++){
                float zn = depth[at(x+dx[k], y+dy[k])];
                if (!std::isfinite(zn)) continue;
                if (std::fabs(zn - zc) > eps){ edge=true; break; }
            }
            if (edge){
                auto* p = img.px(x,y);
                p[0] = (uint8_t)((1.0f-EDGE_STRENGTH)*p[0]);
                p[1] = (uint8_t)((1.0f-EDGE_STRENGTH)*p[1]);
                p[2] = (uint8_t)((1.0f-EDGE_STRENGTH)*p[2]);
            }
        }
    }
}
