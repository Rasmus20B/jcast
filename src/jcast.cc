module;

#include <print>

export module jcast;

import util;

export import collada;
export import asset;

export namespace jcast {

  auto add(int a, int b) -> int {
    return a + b;
  }

  Mesh load_mesh(std::string_view path) {
    Mesh m;
    std::println(stderr, "[=] Loading mesh from file: {}", path);
    if(path.ends_with(".dae")) {
      m = collada::load_asset(path).mesh;
    }
    return m;
  }
}
