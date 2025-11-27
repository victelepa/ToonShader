# README

This week, based on the discussion and inspiration from the class, we adjusted our original plan:  
Instead of modifying the GPUDriver software rasterization framework provided by the course, we implement the Toon Shader on top of our own **simple ray tracer**.

## Work completed this week

- Completed the basic ray tracer framework  
  - Built a ray-tracing rendering pipeline that supports a camera, spheres, triangles, and simple OBJ model loading.  
  - Used a single primary ray to render static scenes and provide clean input data for the Toon Shader (intersection position, normal, depth, etc.).

- Implemented the basic Toon Shader shading model  
  - Added toon-style lighting computation (ambient + diffuse + specular) in the shading stage of the ray tracer.  
  - Initially integrated toon-style diffuse and color control based on brightness banding.

- Implemented hard-edge specular highlight  
  - Discretized the traditional Phong specular term and used thresholds to obtain clear highlight bands.

- Implemented view-dependent silhouette outline  
  - Detected silhouette regions using the dot product \|N·V\| between the surface normal and the view direction.  
  - Output black strokes in near-silhouette regions to achieve stable contour lines.
 
<img width="1224" height="644" alt="image" src="https://github.com/user-attachments/assets/8192a2ae-3fdb-43e4-9eae-4027bad51820" />


## Build & Run

In the project root directory, run the following commands:

```bash
clang++ -std=gnu++17 -O2 src/*.cpp -o toon
./toon
