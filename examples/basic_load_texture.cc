
import jcast;

#include <print>
#include <mdspan>

#include "raylib.h"

auto main() -> int {
  jcast::Mesh m1 = jcast::load_mesh("../examples/cube.dae");

  std::vector<float> verts;
  std::vector<float> coords;
  for(auto i: m1.vertices) {
    verts.push_back(i.position.x);
    verts.push_back(i.position.y);
    verts.push_back(i.position.z);
    coords.push_back(i.texcoords.x);
    coords.push_back(i.texcoords.y);
  }


  Vector2 windowDimensions = { 1920, 1080 };
  InitWindow(windowDimensions.x, windowDimensions.y, "p2");

  Mesh real;
  real.vertexCount = m1.vertices.size() * 3;
  real.triangleCount = m1.vertices.size();

  real.vertices = (float*)MemAlloc(real.vertexCount * 3 * sizeof(float));
  real.texcoords = (float*)MemAlloc(real.vertexCount * 2 * sizeof(float));
  real.normals = (float*)MemAlloc(real.vertexCount * 3 * sizeof(float));

  int i = 0;
  int j = 0;
  int k = 0;
  for(auto v: m1.vertices) {
    real.vertices[i++] = v.position.x;
    real.vertices[i++] = v.position.y;
    real.vertices[i++] = v.position.z;
    real.normals[j++] = v.normal.x;
    real.normals[j++] = v.normal.y;
    real.normals[j++] = v.normal.z;
    real.texcoords[k++] = v.texcoords.y;
    real.texcoords[k++] = v.texcoords.x;
  }

  Camera camera = { { 0.0f, 0.0f, 5.0f }, { 0.0f, 2.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 60.0f, CAMERA_PERSPECTIVE };

  UploadMesh(&real, false);
  Model m = LoadModelFromMesh(real);
  DisableCursor();
  while(!WindowShouldClose()) {
    UpdateCameraPro(&camera, {
        (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) * 0.01f -
        (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))*0.01f,    
        (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))*0.01f -   // Move right-left
        (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))*0.01f,
        0.0f 
        },
        (Vector3){
                GetMouseDelta().x*0.1f,                            // Rotation: yaw
                GetMouseDelta().y*0.1f,                            // Rotation: pitch
                0.0f                                                // Rotation: roll
            },
        GetMouseWheelMove() * 2.0f
        );

    BeginDrawing();
      ClearBackground(RAYWHITE);

      BeginMode3D(camera);

        DrawGrid(10, 1.0f);
        DrawModel(m, {0.f, 0.f, 0.f}, 1.0f, GRAY);

        auto mesh = m.meshes[0];
        auto vertices = std::mdspan(mesh.vertices, mesh.triangleCount, 9);
        for(int i = 0; i < vertices.extent(0); ++i) {
          DrawLine3D({vertices[i, 0], vertices[i, 1], vertices[i, 2]}, {vertices[i, 3], vertices[i, 4],vertices[i, 5]}, DARKBLUE);
          DrawLine3D({vertices[i, 0], vertices[i, 1], vertices[i, 2]}, {vertices[i, 6], vertices[i, 7],vertices[i, 8]}, DARKBLUE);
          DrawLine3D({vertices[i, 3], vertices[i, 4], vertices[i, 5]}, {vertices[i, 6], vertices[i, 7],vertices[i, 8]}, DARKBLUE);
        }
        // DrawGrid(10, 1.f);
        // DrawBoundingBox(GetModelBoundingBox(m), RED);
      EndMode3D();

      DrawFPS(10, 10);
    EndDrawing();
  }

  CloseWindow();

}
