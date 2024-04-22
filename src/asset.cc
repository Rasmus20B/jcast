module;

#include <vector> 

export module asset;

import util;

namespace jcast {

  struct Vertex {
    float x, y, z;
  };

  export struct Mesh {
    std::vector<Vertex> vertices;
  };

  export struct Asset {
    Mesh mesh;
  };

}
