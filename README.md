### Build & Run

macOS / Linux:

```bash
make
./toonobj assets/cube.obj
```

Windows (MSVC):

```bat
build.bat
toonobj.exe assets\cube.obj
```

The result will be in `output.ppm`.

### What we've done this week
- C++ CPU rasterizer scaffold with Makefile/Win batch build.
- Minimal OBJ loader (`v`, `vn`, `f`). 
- Basic math and transforms (`params.h`): vectors/matrices, translate/scale/rotate_y, perspective.
- Triangle rasterizer (`raster.h`): bounding box + edge-function fill, depth buffer, perspective-correct normal interpolation.
- Toon shading (`toon.h`): diffuse banding, hard-edge specular (threshold/width), silhouette darkening (|N·V|).
- Simple depth-based edge darkening pass (post-process).
- PPM image output (`image.h`) and sample render of `assets/cube.obj`.
- Empty directories (code, math, post, shading) are for later code refactoring.
