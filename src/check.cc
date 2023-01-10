#include "./check/check.hpp"

#include <iostream>
#include <vector>

#include "./parser/parser.hpp"

std::string Fill(const std::string &text, int cnt, char c = ' ') {
  if (text.length() >= cnt) return text;
  return text + std::string(cnt - static_cast<int>(text.length()), c);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: parser file1.xl file2.xl file3.xl ..." << std::endl;
    return 0;
  }

  auto parser = parser::Parser();
  auto check = check::Check();

  for (int i = 1; i < argc; ++i) {
    if (parser.Parse(argv[i]) == false) return -1;
    if (!check(*parser.GetAst())) return -1;
  }

  auto res = check.MakePrettyResult();
  std::string symbols = "Symbols\n" + Fill("name", 40) + Fill("type", 40) +
                        Fill("block", 20) + "\n";
  for (auto &sym : res.symbols) {
    auto &name = std::get<0>(sym);
    auto &type = std::get<1>(sym);
    auto &bid = std::get<2>(sym);
    symbols += Fill(name, 40) + Fill(type, 40) + Fill(bid, 20) + "\n";
  }

  std::string codes = "Three Address Codes\n";
  for (auto &code : res.codes) {
    codes += Fill(code.op, 20) + Fill(code.left, 20) + Fill(code.right, 20) +
             Fill(code.res, 20) + Fill(code.id, 20) + "\n";
  }

  std::cout << symbols << std::endl << std::endl;
  std::cout << codes << std::endl << std::endl;
  return 0;
}
