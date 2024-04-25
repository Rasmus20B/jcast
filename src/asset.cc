module;

#include <vector> 

export module asset;

import jcast.util;


namespace jcast {
  using namespace jcast::util;

  export struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texcoords;
  };

  export struct Mesh {
    std::string_view name;
    std::vector<Vertex> vertices;
  };

  export struct Asset {
    std::vector<Mesh> meshes;
  };

}
