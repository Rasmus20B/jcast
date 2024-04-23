module;

#include <print>
#include <ranges>
#include <utility>
#include <unordered_map>
#include <string_view>
#include <string>
#include <vector>
#include <ranges>

export module xml:parser;

import :lexer;
import util;

namespace xml {

  using Attribute = std::pair<std::string, std::string>;

  export struct DOM_Node {
    std::unordered_map<std::string, std::string> attrs;
    std::vector<u32> children;
    std::string text;
    std::string name;
    u32 parent;
  };

  export struct DOM {

    template<typename T>
    DOM_Node& operator[](const T idx);
    DOM_Node& operator[](const std::string idx) { return nodes[lookup[idx]]; }
    DOM_Node& operator[](const std::string_view idx) { return nodes[lookup[idx.data()]]; }
    DOM_Node& operator[](const u32 idx) { return nodes[idx]; }

    void push_back(const std::string id) {
      nodes.push_back({
          .name = id
          });
      lookup[id] = count;
      count++;
    }

    std::unordered_map<std::string, u32> lookup;
    std::vector<DOM_Node> nodes;
    u32 count = 0;
  };

  export auto parse(const std::string_view code) -> DOM {

    auto tokens = lex(code);
    DOM d;
    std::string cur{};
    u16 depth = 0;
    auto not_slash = [](const char c) {
      return !(c == '/');
    };
    for(auto &t: tokens) {
      switch(t.type) {
        case TokenType::START:
          {
            auto par = cur;
            cur += "/" + t.value;
            if(par.size()) {
              // std::println("{} par: {}, {}", depth, par, cur);
              d[par].children.push_back(d.count);
              d[cur].parent = d.lookup[par];
            }
            d.push_back(cur);
            depth++;
          }
          break;
        case TokenType::END:
          {
            if(depth) {
              cur = std::string_view(cur) 
                | std::views::reverse
                | std::views::drop_while(not_slash)
                | std::views::drop(1)
                | std::views::reverse
                | std::ranges::to<std::string>();
            }
            depth--;
          }
          break;
        case TokenType::TEXT:
          {
            d[cur].text = t.value;
          }
          break;
        case TokenType::ATTR:
          {
            d[cur].attrs.insert({t.attribute, t.value});
          }
        default:
          break;
      }
    }
    return d;
  }

  export void print_dom(DOM& d) {
    for(auto node : d.lookup) {
      std::println("{}: ", node.first);
      for(auto i : d[node.first].attrs) {
        std::print("    ");
        std::println("{}: {} ", i.first, i.second);
      }
      std::print("    ");
      std::println("TEXT: {}", d[node.first].text);
      std::println("");
    }
  }

}
