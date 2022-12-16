#include <iostream>
#include <stack>

#include "./ast/tojson.hpp"
#include "./utils/log.hpp"

extern int yyparse();
extern FILE *yyin;
extern std::stack<std::string> kFilenames;
extern utils::Uptr<ast::Module> kModule;

void PrintJson(const std::string &str, int depth = 0) {
  bool one_line = false, in_string = false;
  for (int i = 0; i < static_cast<int>(str.length());) {
    if (str[i] == '{' || str[i] == '[') {
      int right = str.find(str[i] == '{' ? '}' : ']', i);
      one_line = str.find(str[i], i + 1) > static_cast<size_t>(right) &&
                 str.find(',', i) > static_cast<size_t>(right);

      std::cout << str[i++];
      if (!one_line) std::cout << "\n" << std::string(++depth * 2, ' ');
    } else if (str[i] == '}' || str[i] == ']') {
      if (!one_line) std::cout << "\n" << std::string(--depth * 2, ' ');
      std::cout << str[i++];

      if (str[i] == ',') {
        std::cout << str[i++] << '\n' << std::string(depth * 2, ' ');
        i += str[i] == ' ';
      } else if (str[i] == '}') {
        one_line = false;
      }
    } else {
      std::cout << str[i];
      in_string ^= str[i] == '"';
      if (str[i++] == ',' && !in_string) {
        std::cout << '\n' << std::string(depth * 2, ' ');
        i += str[i] == ' ';
      }
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: parser file1.xl file2.xl file3.xl ..." << std::endl;
    return 0;
  }

  for (int i = 1; i < argc; ++i) {
    kFilenames.push(argv[i]);
    yyin = fopen(argv[i], "r+");
    if (yyparse() != 0) return -1;

    auto to_json = ast::ToJson();
    PrintJson(to_json(kModule.get()));
    std::cout << std::endl << std::endl;
  }

  return 0;
}
