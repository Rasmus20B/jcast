module;

#include <vector>
#include <utility>
#include <cstdint>
#include <span>
#include <ranges>
#include <print>
#include <string_view>

export module jcast.xml:lexer;

import jcast.util;

using namespace jcast::util;
namespace xml {
  export enum class TokenType : u8 {
    STARTDOC,
    ENDDOC,
    START,
    END,
    TEXT,
    ATTR,
    NAMESPACE,
    COMMENT,
    PROCINST
  };

  constexpr std::array toknames = {
    "startdoc",
    "enddoc",
    "start",
    "end",
    "text",
    "attr",
    "namespace",
    "comment",
    "procinst"
  };

  struct Token {
    const TokenType type = TokenType::ATTR;
    const std::string attribute;
    const std::string value;
  };

  export auto lex(const std::string_view code) -> std::vector<Token>{

    std::vector<Token> tokens;
    std::string buffer;

    u16 scope = 0;
    u32 ptr = 0;

    while(ptr < code.size()) {
      buffer += code[ptr];
      if(code[ptr] == '<') {
        if(code[ptr+1] == '?') {
          std::string inst;
          while(code[ptr] != ' ') {
            inst += code[ptr++];
          }
          tokens.push_back(Token {
              .type = TokenType::PROCINST,
              .value = inst
              });

          while(!inst.ends_with("?>")) {
            inst += code[ptr++];
          }
          buffer.clear();

        } else if(std::string_view(&code[ptr], 3) == "!--") {
          ptr++;
          std::string comment;
          while(!comment.ends_with("-->")) {
            comment += code[ptr++];
          }
          tokens.push_back(Token {
              .type = TokenType::COMMENT,
              .value = comment
              });
          buffer.clear();
        }else if(code[ptr+1] == '/') {
          std::string tag;
          while(code[ptr++] != '>') {
            tag += code[ptr];
          }
          tokens.push_back(Token {
              .type = TokenType::END,
              .value = tag
              });
        } else {
          ptr++;
          std::string elem;
          while(code[ptr] != ' ' && code[ptr] != '>') {
            elem += code[ptr++];
          }

          if(code[ptr-1] == '/') elem.pop_back();
          tokens.push_back(Token {
              .type = TokenType::START,
              .value = elem
              });
          buffer.clear();

          if(code[ptr] == '>') {
            if(code[ptr-1] == '/') {
              tokens.push_back( Token {
                  .type = TokenType::END,
                  .value = elem
                  });
            } else {
              std::string text;
              ptr++;
              while(code[ptr] != '<') {
                if(code[ptr] != '<') {
                  text += code[ptr];
                }
                ptr++;
              }

              if(text.size()) {
                tokens.push_back(Token {
                    .type = TokenType::TEXT,
                    .value = text
                  });
              }
            }
            ptr--;
          }
          scope++;
        }
      } else if(std::isalpha(code[ptr])) {
        if(scope) {
          std::string name;
          while(code[ptr] != '=') {
            name += code[ptr++];
          }
          ptr++;
          std::string val;
          while(code[ptr] != ' ' && code[ptr] != '>') {
            val += code[ptr++];
          }

          auto end = val.ends_with('/');
          if(end) {
            val.pop_back();
          }

          tokens.push_back(Token {
              .type = TokenType::ATTR,
              .attribute = name,
              .value = val
              });

          if(code[ptr] == ' ') {
            tokens.push_back(Token {
                .type = TokenType::NAMESPACE
                });
          } else if(end) {
            tokens.push_back( Token {
                .type = TokenType::END,
                });
          } else if(code[ptr] == '>') {
            std::string text;
            ptr++;
            while(code[ptr] != '<') {
              text += code[ptr++];
            }

            auto size_no_space = (text)
              | std::views::drop_while([](const char c) {
                  return iswspace(c);
                  });

            if(size_no_space.size()) {
              tokens.push_back(Token {
                  .type = TokenType::TEXT,
                  .value = text
                  });
            }
            continue;
          }
        } 
      } else if(code[ptr] == '>') {
        scope--;
      }
      ptr++;

    }
    return tokens;
  }

};


