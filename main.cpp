#include <cstdio>
#include <vector>
#include <iostream>
#include "params.h"
#include "image.h"
#include "obj.h"
#include "raster.h"

int main(int argc, char** argv){
    if (argc<2){
        std::cerr << "Usage: " << argv[0] << " <model.obj>\n";
        std::cerr << "Falling back to assets/cube.obj\n";
    }
    std::string path = (argc>=2)? argv[1] : "assets/cube.obj";

    Mesh mesh;
    if (!load_obj(path, mesh)){
        std::cerr << "Failed to load OBJ: " << path << "\n";
        return 1;
    }
    std::cout << "Loaded " << mesh.positions.size() << " vertices, " << mesh.tris.size() << " triangles\n";

    std::vector<Vertex> verts(mesh.positions.size());
    for (size_t i=0;i<mesh.positions.size();i++){
        verts[i].pos = mesh.positions[i];
        verts[i].nrm = (i<mesh.normals.size())? mesh.normals[i] : vec3(0,1,0);
    }
    std::vector<int> indices; indices.reserve(mesh.tris.size()*3);
    for (auto &t : mesh.tris){ indices.push_back(t.v0); indices.push_back(t.v1); indices.push_back(t.v2); }

    mat4 V = identity(); V(2,3) = -3.0f;
    mat4 P = perspective(FOV_Y, IMG_W/(float)IMG_H, Z_NEAR, Z_FAR);

    float angle = 0.7f;
    mat4 M = identity();
    // M = translate(vec3(0,0,0)) * rotate_y(angle) * scale(vec3(1,1,1)); // if you implement operator*
    auto mulm_local = [](const mat4&A,const mat4&B){
        mat4 R{}; for(int c=0;c<4;c++) for(int r=0;r<4;r++){ R(r,c)=0; for(int k=0;k<4;k++) R(r,c)+=A(r,k)*B(k,c);} return R;
    };
    // build transform by hand
    mat4 S = scale(vec3(0.8f,0.8f,0.8f));
    mat4 R = rotate_y(angle);
    mat4 T = translate(vec3(0,0,0));
    M = mulm_local(T, mulm_local(R, S));

    mat4 MV = mulm_local(V, M);
    mat4 MVP = mulm_local(P, MV);

    Transforms Tfs{}; Tfs.M=M; Tfs.V=V; Tfs.P=P; Tfs.MVP=MVP;

    ImageRGB img(IMG_W, IMG_H);
    std::vector<float> depth(IMG_W*IMG_H, std::numeric_limits<float>::infinity());

    for (int y=0;y<IMG_H;y++){
        for (int x=0;x<IMG_W;x++){
            auto* p = img.px(x,y);
            p[0]=180; p[1]=210; p[2]=240;
        }
    }

    raster_mesh(verts, indices, Tfs, img, depth);
    post_edges(img, depth);

    img.save_ppm("output.ppm");
    std::cout << "Wrote output.ppm ("<<IMG_W<<"x"<<IMG_H<<")\n";
    return 0;
}
