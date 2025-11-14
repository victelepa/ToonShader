# ToonOBJ — Standalone CPU OBJ Rasterizer with Toon Shading

This **self-contained** C++17 project loads a simple **OBJ** mesh (no external libs),
rasterizes it on the CPU with:
- **Toon diffuse banding** (configurable band count)
- **Hard-edge specular highlight** (threshold + width)
- **View-dependent silhouette** via |N·V| darkening
- **Depth edge post-process** (darkens contact/crease lines)

Outputs `output.ppm`.

## Build & Run

### macOS / Linux
```bash
make
./toonobj assets/cube.obj
```

### Windows (MSVC)
```bat
build.bat
toonobj.exe assets\cube.obj
```

You should see `output.ppm`. Open with Preview/Photos, etc.

## Features
- Minimal **OBJ** loader: supports `v`, `vn`, `f` (triangles/quads; quads are triangulated). Ignores `vt` and materials.
- Recomputes **vertex normals** if `vn` are missing (area-weighted).
- Simple **camera** and **model transform** (scale/rotate/translate) with perspective projection.
- CPU **triangle rasterizer** with barycentric tests, depth buffer, perspective-correct interpolation.
- **Toon shading** in the fragment stage; **|N·V|** silhouette darkening.
- **Post-process** depth edge darkening (4-neighborhood).

## Quick tweaks
Open `params.h`:
- `DIFFUSE_BANDS` (e.g., 3–5)
- `SPEC_THRESH`, `SPEC_WIDTH`, `SPEC_STRENGTH`
- `NV_THRESH` (silhouette)
- `EDGE_STRENGTH`, `DEPTH_EPS`
- Camera FOV and image size
- Model transform block in `main.cpp`

## Roadmap
- Add normal-edge detector (neighboring normals difference)
- Add texture-based color ramps
- Add simple MTL color import
