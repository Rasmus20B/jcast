module;

#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <print>
#include <ranges>
#include <string_view>

#include <mdspan>

import asset;


export module collada;

import xml;
import asset;

import util;


namespace jcast {
  namespace collada {

    struct Source {
      std::string name;
      std::vector<f32> array;
      u32 count;
      u32 stride;
    };

    struct Input {
      std::string semantic;
      std::string source_name;
      u32 offset;
    };

    struct Triangles {
      u32 count;
      std::vector<u32> prims;
      std::vector<Input> inputs;
    };

    struct Vertices {
      std::string name;
      Input position;
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
          in.semantic = ptr.attrs["semantic"];
          in.source_name = ptr.attrs["source"];
          in.offset = std::stoi(remove_quotes(ptr.attrs["offset"]));
          tmp.inputs.emplace_back(in);
        } else if(ptr.name.ends_with("/p")) {
          std::stringstream ss(ptr.text);
          u32 num;
          while(ss >> num) tmp.prims.push_back(num);
        }   
      }

      return tmp;
    }

    Vertices get_vertices(xml::DOM& dom, xml::DOM_Node ptr) {
      Vertices tmp;
      tmp.name = ptr.attrs["id"];
      for(auto c: ptr.children) {
        ptr = dom[c];
        if(ptr.name.ends_with("/input")) {
          Input in;
          in.semantic = ptr.attrs["semantic"];
          in.source_name = ptr.attrs["source"];
          tmp.position = in;
        }
      }
      return tmp;
    }

    Mesh get_mesh(std::vector<Source>& sources, 
                  std::vector<Triangles>& triangles, 
                  std::vector<Vertices>& vertices) {
      auto count = triangles[0].prims.size();

      auto v_stride = sources[0].stride;
      auto v_count = sources[0].count;
      auto v_span = std::mdspan(sources[0].array.data(), v_count, v_stride);

      auto n_stride = sources[1].stride;
      auto n_count = sources[1].count;
      auto n_span = std::mdspan(sources[1].array.data(), n_count, n_stride);

      auto t_stride = sources[2].stride;
      auto t_count = sources[2].count;
      auto t_span = std::mdspan(sources[2].array.data(), t_count, t_stride);

      std::vector<Vertex> verts;

      for(int i = 0; i < count; i+=3) {
        auto idx = triangles[0].prims[i];
        Vertex v;
        v.position.x = v_span[idx, 0];
        v.position.y = v_span[idx, 1];
        v.position.z = v_span[idx, 2];

        idx = triangles[0].prims[i+1];
        v.normal.x = n_span[idx, 0];
        v.normal.y = n_span[idx, 1];
        v.normal.z = n_span[idx, 2];

        idx = triangles[0].prims[i+2];
        v.texcoords.x = t_span[idx, 0];
        v.texcoords.y = t_span[idx, 1];
        verts.emplace_back(v);
      }

      Mesh m;
      m.vertices = verts;

      return m;
    }

    std::vector<Mesh> get_meshes(xml::DOM& dom) {
      /* ROOT directory for where meshes are stored in Collada. */
      std::string_view name = "/COLLADA/library_geometries";
      auto idx = dom[name];

      std::vector<Mesh> meshes;

      for(auto &c: idx.children) {
        Mesh tmp;
        auto ptr = dom[c];
        auto mesh_name = ptr.attrs["id"];
        
        ptr = dom[dom[c].children[0]];
        std::vector<Source> sources;
        std::vector<Triangles> triangles;
        std::vector<Vertices> vertices;

        for(auto &s: ptr.children) {
          auto s_ptr = dom[s];
          if(s_ptr.name.ends_with("/source")) {
            auto tmp = get_source(dom, s_ptr);
            sources.emplace_back(tmp);
          } else if(s_ptr.name.ends_with("/triangles")) {
            auto tmp = get_triangle(dom, s_ptr);
            triangles.emplace_back(tmp);
          } else if(s_ptr.name.ends_with("/vertices")) {
            auto tmp = get_vertices(dom, s_ptr);
            vertices.emplace_back(tmp);
          }
        }
        auto mesh = get_mesh(sources, triangles, vertices);
        meshes.emplace_back(mesh);
      }
      return meshes;
    }

    export Asset load_asset(const std::filesystem::path& p) {
      std::println(stderr, "[=] Loading Asset from file: {}", p.string());
      std::ifstream file{p};

      if(!file) {
        std::println(stderr, "[-] Unable to open file: {}", p.string());
      }
      std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

      auto dom = xml::parse(data);
      auto meshes = get_meshes(dom);

      Asset ret;
      ret.meshes = meshes;
      return ret;
    }
  }
}
