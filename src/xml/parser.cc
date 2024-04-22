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

  struct DOM_Node {
    std::vector<Attribute> attrs;
    std::vector<u32> children;
    std::string text;
    u32 parent;
  };

  struct DOM {
    DOM_Node& operator[](const std::string idx) { return nodes[lookup[idx]]; }

    void push_back(const std::string id) {
      nodes.push_back({});
      lookup[id] = count;
      count++;
    }

    std::unordered_map<std::string, u32> lookup;
    std::vector<DOM_Node> nodes;
    u32 count = 0;
  };

  export auto parse(const std::string code) -> DOM {

    auto tokens = lex(code);
    DOM d;
    std::string cur{};
    u16 depth = 0;
    for(auto &t: tokens) {
      switch(t.type) {
        case TokenType::START:
          {
            auto par = cur;
            cur = cur += "/" + t.value;
            d.push_back(cur);
            d[cur].parent = d.lookup[par];
            depth++;
          }
          break;
        case TokenType::END:
          {
            if(depth) {
              cur = std::string_view(cur) 
                | std::views::reverse
                | std::views::drop_while([](const char c) {
                    return !(c == '/');
                    })
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
            d[cur].attrs.push_back({t.attribute, t.value});
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
