#include "loader.h"
#include <elfio/elfio.hpp>
#include <iomanip>
#include <iostream>
#include <map>
#include <stdio.h>
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
    } else if (s->get_type() == ELFIO::SHT_SYMTAB) {
      symTab = s;
    } else if (s->get_type() == ELFIO::SHT_RELA ||
               s->get_type() == ELFIO::SHT_REL) {
      relSections.push_back(s);
    }
  }

  return 0;
}

std::map<ELFIO::Elf64_Addr, std::string>
ElfLoader::GetRelocationMap(ELFIO::section *s) {
  std::map<ELFIO::Elf64_Addr, std::string> relMap;
  for (ELFIO::section *relsec : relSections) {
    auto infosecidx = relsec->get_info();
    auto symsecidx = relsec->get_link();
    if (infosecidx == s->get_index()) {
      const ELFIO::relocation_section_accessor rsa(reader, relsec);
      for (int i = 0; i < rsa.get_entries_num(); i++) {
        ELFIO::Elf64_Addr offset;
        ELFIO::Elf64_Addr symbolValue;
        std::string symbolName;
        ELFIO::Elf_Word type;
        ELFIO::Elf_Sxword addend;
        ELFIO::Elf_Sxword calcValue;

        rsa.get_entry(i, offset, symbolValue, symbolName, type, addend,
                      calcValue);
        relMap[offset] = symbolName;
      }

      break;
    }
  }

  return relMap;
}

std::vector<ELFIO::section *> ElfLoader::GetCodeSections() {
  return codeSections;
}

ELFIO::elfio &ElfLoader::GetReader() { return reader; }

ELFIO::section *ElfLoader::GetSymTab() { return symTab; }
