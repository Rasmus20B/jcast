module;

#include <string>
#include <fstream>
#include <filesystem>
#include <print>

import asset;


export module collada;

import xml;
import asset;


namespace jcast {
  namespace collada {

    export Asset load_asset(const std::filesystem::path& p) {
      std::println(stderr, "[=] Loading Asset from file: {}", p.string());
      std::ifstream file{p};

      if(!file) {
        std::println(stderr, "[-] Unable to open file: {}", p.string());
      }
      std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

      auto dom = xml::parse(data);
      xml::print_dom(dom);

    }

  }
}
