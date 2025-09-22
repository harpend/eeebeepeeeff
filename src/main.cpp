#include "disassembler.h"
#include <elfio/elfio.hpp>
#include <iostream>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: ./eeebeepeeeff [filename]" << std::endl;
    return 1;
  }

  ElfLoader *loader = new ElfLoader(std::string(argv[1]));
  loader->LoadFile();
  std::vector<ELFIO::section *> sections = loader->GetCodeSections();
  Disassembler *dis = new Disassembler(sections);
  dis->Disassemble(loader);
  return 0;
}
