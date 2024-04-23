module;

#include <vector> 

export module asset;

import util;

namespace jcast {

  struct Vertex {
    float x, y, z;
  };

  export struct Mesh {
    std::string_view name;
    std::vector<Vertex> vertices;
  };

  export struct Asset {
    std::vector<Mesh> meshes;
  };

}
