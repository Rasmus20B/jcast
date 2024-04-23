
import jcast;
import util;

#include <print>
#include <mdspan>
#include <raylib.h>

auto main() -> i32 {
  jcast::Mesh m1 = jcast::load_mesh("../examples/cube.dae");
  jcast::Mesh m2 = jcast::load_mesh("../examples/cube_weird.dae");

  std::vector<f32> verts;
  std::vector<f32> coords;
  for(auto i: m2.vertices) {
    verts.push_back(i.position.x);
    verts.push_back(i.position.y);
    verts.push_back(i.position.z);
    coords.push_back(i.texcoords.x);
    coords.push_back(i.texcoords.y);
  }


  vec2 windowDimensions = { 1080, 768 };
  InitWindow(windowDimensions.x, windowDimensions.y, "p2");
  Model work = LoadModel("../examples/cube.glb");

  Mesh real;
  real.vertexCount = m2.vertices.size() * 3;
  real.triangleCount = m2.vertices.size();

  std::println("size : {}", m1.vertices.size());

  real.vertices = (float*)MemAlloc(real.vertexCount * 3 * sizeof(f32));
  real.texcoords = (float*)MemAlloc(real.vertexCount * 2 * sizeof(f32));
  real.normals = (float*)MemAlloc(real.vertexCount * 3 * sizeof(f32));

  int i = 0;
  int j = 0;
  int k = 0;
  for(auto v: m2.vertices) {
    real.vertices[i++] = v.position.x;
    real.vertices[i++] = v.position.y;
    real.vertices[i++] = v.position.z;
    real.normals[j++] = v.normal.x;
    real.normals[j++] = v.normal.y;
    real.normals[j++] = v.normal.z;
    real.texcoords[k++] = v.texcoords.y;
    real.texcoords[k++] = v.texcoords.x;
  }

  Camera camera = { { 5.0f, 5.0f, 5.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };
  UploadMesh(&real, false);
  Model m = LoadModelFromMesh(real);
  while(!WindowShouldClose()) {

    UpdateCamera(&camera, CAMERA_ORBITAL);

    if(IsKeyDown(KEY_W)) {
      camera.position.z -= .005;
    } else if(IsKeyDown(KEY_S)) {
      camera.position.z += .005;
    } else if(IsKeyDown(KEY_A)) {
      camera.position.x -= .005;
    } else if(IsKeyDown(KEY_D)) {
      camera.position.x += .005;
    }

    BeginDrawing();
      ClearBackground(RAYWHITE);

      BeginMode3D(camera);

        DrawGrid(0, 1.0f);
        DrawModel(m, {0.f, 0.f, 0.f}, 1.0f, BLACK);
        DrawModel(work, {4.f, 0.f, 0.f}, 1.0f, BLACK);
        DrawBoundingBox(GetModelBoundingBox(m), RED);

      EndMode3D();

      DrawFPS(10, 10);
    EndDrawing();
  }

  CloseWindow();

}
