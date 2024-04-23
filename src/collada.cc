module;

#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <print>
#include <ranges>
#include <string_view>

import asset;


export module collada;

import xml;
import asset;

import util;


namespace jcast {

  struct Source {
    std::string name;
    std::vector<f32> array;
    u32 count;
    u32 stride;
  };

  struct Input {
    std::string source_name;
    u32 offset;
  };

  struct Triangles {
    u32 count;
    std::vector<u32> prims;
    std::vector<Input> inputs;
  };

  std::string remove_quotes(std::string s) {
    return std::ranges::to<std::string>(std::string_view(s)
      | std::views::drop_while([](const char c){ return c == '\"'; })
      | std::views::take_while([](const char c){ return c != '\"'; }));
  }

  Source get_source(xml::DOM& dom, xml::DOM_Node ptr) {
    Source tmp;
    tmp.name = ptr.attrs["id"];
    for(auto c: ptr.children) {
      auto child = dom[c];
      if(child.name.ends_with("/float_array")) {
        std::stringstream ss(child.text);
        f32 num;
        while(ss >> num) tmp.array.push_back(num);
      } else if(child.name.ends_with("/technique_common")) {
        auto ptr = dom[c];
        for(auto tech: ptr.children) {
          if(dom[tech].name.ends_with("/accessor")) {
            ptr = dom[tech];
            auto s_count = remove_quotes(ptr.attrs["count"]);
            auto s_stride = remove_quotes(ptr.attrs["stride"]);
            tmp.count = std::stoi(s_count);
            tmp.stride = std::stoi(s_stride);
          }
        }
      }
    }
    return tmp;
  }

  Triangles get_triangle(xml::DOM& dom, xml::DOM_Node ptr) {
    Triangles tmp;
    tmp.count = std::stoi(remove_quotes(ptr.attrs["count"]));
    for(auto c: ptr.children) {
      ptr = dom[c];
      if(ptr.name.ends_with("/input")) {
        Input in;
        in.source_name = ptr.attrs["source"];

      } else if(ptr.name.ends_with("/p")) {
        std::stringstream ss(ptr.text);
        u32 num;
        while(ss >> num) tmp.prims.push_back(num);
      }
    }

    return tmp;
  }


  std::vector<Mesh> get_meshes(xml::DOM& dom) {
    /* ROOT directory for where meshes are stored in Collada. */
    std::string_view name = "/COLLADA/library_geometries";
    auto idx = dom[name];

    std::vector<Mesh> meshes;

    std::println("Geometries\n=========");
    for(auto &c: idx.children) {
      // std::println("{}: {}, {}", dom[c].name, dom[c].attrs[1].first, dom[c].attrs[1].second);
      Mesh tmp;
      auto ptr = dom[c];
      auto mesh_name = ptr.attrs["id"];
      
      ptr = dom[dom[c].children[0]];
      std::vector<Source> sources;
      std::vector<Triangles> triangles;

      for(auto &s: ptr.children) {
        auto s_ptr = dom[s];
        if(s_ptr.name.ends_with("/source")) {
          auto tmp = get_source(dom, s_ptr);
          sources.emplace_back(tmp);
        } else if(s_ptr.name.ends_with("/triangles")) {
          auto tmp = get_triangle(dom, s_ptr);
          triangles.emplace_back(tmp);
        }
      }

      std::println("Sources\n========");
      for(auto s: sources) {
        std::println("{}: count : {}, stride : {}", s.name, s.count, s.stride);
        for(auto n: s.array) {
          std::print("{}, ", n);
        }
        std::println("");
      }

      std::println("Triangles\n========");
      for(auto t: triangles) {
        std::println("count: {}", t.count);
        for(auto n: t.prims) {
          std::print("{}, ", n);
        }
        std::println("");
      }
    }
    return meshes;
  }

  namespace collada {
    export Asset load_asset(const std::filesystem::path& p) {
      std::println(stderr, "[=] Loading Asset from file: {}", p.string());
      std::ifstream file{p};

      if(!file) {
        std::println(stderr, "[-] Unable to open file: {}", p.string());
      }
      std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

      auto dom = xml::parse(data);
      auto meshes = get_meshes(dom);

      if(!meshes.size()) {
        meshes.push_back({});
      }

      Asset ret;
      ret.meshes = meshes;
      return ret;
    }
  }
}
