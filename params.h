#pragma once
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <string>
#include <limits>

struct vec2 { float x,y; vec2():x(0),y(0){} vec2(float X,float Y):x(X),y(Y){} };
struct vec3 { float x,y,z; vec3():x(0),y(0),z(0){} vec3(float X,float Y,float Z):x(X),y(Y),z(Z){} };
struct vec4 { float x,y,z,w; vec4():x(0),y(0),z(0),w(1){} vec4(float X,float Y,float Z,float W):x(X),y(Y),z(Z),w(W){} };

inline vec3 operator+(const vec3&a,const vec3&b){return vec3(a.x+b.x,a.y+b.y,a.z+b.z);}
inline vec3 operator-(const vec3&a,const vec3&b){return vec3(a.x-b.x,a.y-b.y,a.z-b.z);}
inline vec3 operator*(const vec3&a,float s){return vec3(a.x*s,a.y*s,a.z*s);}
inline vec3 operator*(float s,const vec3&a){return vec3(a.x*s,a.y*s,a.z*s);}
inline vec3 operator/(const vec3&a,float s){return vec3(a.x/s,a.y/s,a.z/s);}
inline float dot(const vec3&a,const vec3&b){return a.x*b.x+a.y*b.y+a.z*b.z;}
inline vec3 cross(const vec3&a,const vec3&b){return vec3(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);}
inline float length(const vec3&a){return std::sqrt(std::max(0.f, dot(a,a)));}
inline vec3 normalize(const vec3&a){ float L=length(a); return (L>1e-8f)? a*(1.f/L) : vec3(0,0,1); }
inline float clamp01f(float v){return std::min(1.f,std::max(0.f,v));}
inline vec3 clamp01(const vec3&c){return vec3(clamp01f(c.x),clamp01f(c.y),clamp01f(c.z));}

struct mat4 {
    float m[16];
    float& operator()(int r,int c){return m[c*4+r];}
    const float& operator()(int r,int c) const {return m[c*4+r];}
};

inline vec4 mul(const mat4& M, const vec4& v){
    vec4 r;
    r.x = M(0,0)*v.x + M(0,1)*v.y + M(0,2)*v.z + M(0,3)*v.w;
    r.y = M(1,0)*v.x + M(1,1)*v.y + M(1,2)*v.z + M(1,3)*v.w;
    r.z = M(2,0)*v.x + M(2,1)*v.y + M(2,2)*v.z + M(2,3)*v.w;
    r.w = M(3,0)*v.x + M(3,1)*v.y + M(3,2)*v.z + M(3,3)*v.w;
    return r;
}

inline mat4 identity(){ mat4 M{}; for(int i=0;i<16;i++) M.m[i]=0; M(0,0)=M(1,1)=M(2,2)=M(3,3)=1; return M; }
inline mat4 translate(const vec3& t){ mat4 M=identity(); M(0,3)=t.x; M(1,3)=t.y; M(2,3)=t.z; return M; }
inline mat4 scale(const vec3& s){ mat4 M=identity(); M(0,0)=s.x; M(1,1)=s.y; M(2,2)=s.z; return M; }
inline mat4 rotate_y(float a){ mat4 M=identity(); float c=std::cos(a), s=std::sin(a); M(0,0)= c; M(0,2)= s; M(2,0)=-s; M(2,2)= c; return M; }
inline mat4 perspective(float fovy_deg, float aspect, float znear, float zfar){
    float f = 1.0f / std::tan((fovy_deg*3.14159265f/180.f)*0.5f);
    mat4 M{}; for(int i=0;i<16;i++) M.m[i]=0;
    M(0,0)=f/aspect; M(1,1)=f;
    M(2,2)=(zfar+znear)/(znear-zfar);
    M(2,3)=(2*zfar*znear)/(znear-zfar);
    M(3,2)=-1.0f;
    return M;
}

static constexpr int   IMG_W = 800;
static constexpr int   IMG_H = 600;
static constexpr float FOV_Y = 60.0f;
static constexpr float Z_NEAR = 0.1f;
static constexpr float Z_FAR  = 100.f;

static constexpr int   DIFFUSE_BANDS  = 4;
static constexpr float SPEC_THRESH    = 0.6f;
static constexpr float SPEC_WIDTH     = 0.05f;
static constexpr float SPEC_STRENGTH  = 0.9f;
static constexpr float NV_THRESH      = 0.25f;

static constexpr float EDGE_STRENGTH  = 0.9f;
static constexpr float DEPTH_EPS      = 1e-3f;

static const vec3 BASE_COLOR  = vec3(0.95f,0.85f,0.35f);
static const vec3 HI_COLOR    = vec3(1.00f,1.00f,0.95f);

static const vec3 LIGHT_DIR = normalize(vec3(0.3f, 0.6f, 0.8f));
