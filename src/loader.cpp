#include "loader.h"
#include <elfio/elfio.hpp>
#include <map>
#include <string>
#include <vector>

ElfLoader::ElfLoader(std::string filename) {
  this->filename = filename;
  codeSections = {};
}

int ElfLoader::LoadFile() {
  if (!reader.load(filename)) {
    std::cerr << "Couldn't load file" << filename << std::endl;
    return -1;
  }

  ELFIO::Elf_Half elfType = reader.get_type();
  if (reader.get_machine() !=
      ELFIO::EM_BPF) { // 247 incase it is not in the header
    std::cerr << "Not an executable";
    return -1;
  }

  for (int i = 0; i < reader.sections.size(); i++) {
    ELFIO::section *s = reader.sections[i];
    if (s->get_type() == ELFIO::SHT_PROGBITS) {
      codeSections.push_back(s);
    }

    if (s->get_type() == ELFIO::SHT_SYMTAB) {
      symTab = s;
    }
  }

  return 0;
}

std::vector<ELFIO::section *> ElfLoader::GetCodeSections() {
  return codeSections;
}

ELFIO::elfio &ElfLoader::GetReader() { return reader; }

ELFIO::section *ElfLoader::GetSymTab() { return symTab; }
