# CSCI 580 Final Project — Toon Shader (CPU Ray Tracer)

**Toon Shading with Ramp, Diffuse, Hard-Edge Specular, Rim Light, and Dual Outlines**

## Team
- Haoran Jia — Haoranji@usc.edu     banded specular + rim light tuning, integration/support
- Shengjie Lin — slin0570@usc.edu     rim integration, cleanup/documentation
- Weibo Xu — weiboxu@usc.edu     ramp diffuse, depth buffer + depth edge support
- Raymond Zhang — yanzhezh@usc.edu     toon shader core, silhouette integration

---

## Overview
This project extends the CSCI 580 ray tracing homework framework with a stylized toon shading model.  
We replace the original shading, adjust several modules, and add a depth-based post-processing step since the original pipeline does not provide a depth buffer.

---

## Features
- Toon diffuse with **color ramp** (colors + positions)
- **Hard-Edge Specular** with two thresholds
- **Rim Light** (Fresnel-like, view-dependent)
- **Outlines**
  - **Silhouette** using \|N·V\|
  - **Depth Edge** using Sobel on a generated depth buffer
- OBJ mesh support + basic primitives (e.g., spheres)

---

## Pipeline (High-Level)
1. Set up camera and allocate buffers  
2. Parse OBJ into triangles (retain simple primitives)  
3. Fire primary rays → intersection → toon shading  
4. Fill fragment buffer  
5. Apply depth-based outline post-process in place  

---

## Key Parameters
Configured in `main.cpp` via `ToonParams`:
- Ramp: `rampColors`, `rampPositions`
- Specular: `specularThreshold1/2`, `specColorA/B`, material `shininess`
- Rim: `enableRim`, `rimColor`, `rimIntensity`, `rimPower`, `rimThreshold`
- Outlines: `silhouetteThreshold`, `outlineColor`, `enableDepthEdges`, `depthEdgeThreshold`

---

<img width="1740" height="908" alt="image" src="https://github.com/user-attachments/assets/bbca865b-a70f-42fe-8ea1-b2d03e3dd7c1" />

---

## Build & Run
```bash
clang++ -std=gnu++17 -O2 src/*.cpp -o toon
./toon
