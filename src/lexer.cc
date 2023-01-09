#include "./parser/parser.hpp"

inline std::string Fill(const std::string &text, int len, char c = ' ') {
  if (text.length() >= len) return text;
  return text + std::string(len - text.length(), c);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: parser file1.xl file2.xl file3.xl ..." << std::endl;
    return 0;
  }

  auto parser = parser::Parser();

  for (int i = 1; i < argc; ++i) {
    if (parser.Parse(argv[i]) == false) return -1;

    std::cout << "File " << parser.GetFilenames().back() << std::endl;

    for (const auto &token : parser.GetLexer()->GetTokens()) {
      std::cout << " "
                << Fill(std::string(token->GetLoc()),
                        sizeof("Line XXXX Col YY"));
      std::cout << " " << Fill(token->GetTypeName(), 16);
      std::string text;
      for (auto &c : token->GetText()) {
        if (c == '\n') {
          text += "<LF>";
        } else if (c == '\r') {
          text += "<CR>";
        } else if (c == '\t') {
          text += "<TAB>";
        } else {
          text += c;
        }
      }

      std::cout << " " << text << std::endl;
    }

    std::cout << std::endl << std::endl;
  }

  return 0;
}
