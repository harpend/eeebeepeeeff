#include "disassembler.h"
#include <capstone/capstone.h>
#include <elfio/elfio.hpp>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

Disassembler::Disassembler(std::vector<ELFIO::section *> sections) {
  this->sections = sections;
}

int Disassembler::Disassemble(ElfLoader *loader) {
  csh dis;
  cs_insn *insns;
  size_t n;
  if (cs_open(CS_ARCH_BPF, CS_MODE_BPF_EXTENDED, &dis) != CS_ERR_OK) {
    std::cerr << "Capstone failed" << std::endl;
    return -1;
  }

  for (ELFIO::section *s : sections) {
    const char *data = s->get_data();
    ELFIO::Elf_Xword size = s->get_size();
    if (data) {
      std::string sname = s->get_name();
      std::cout << sname << ":" << std::endl;

      if (sname == "license") {
        std::cout << "\t" << std::string(data, size) << std::endl;
      } else if (sname == "version") {
        if (size == 4) {
          char *ver = (char *)data;
          printf("\t%d.%d.%d\n", ver[2], ver[1], ver[0]);
        }
      } else if (sname == "maps") { // TODO: extract to new func
        // the number of maps is defined by the size of the section / the number
        // of symbols that point into it
        const ELFIO::symbol_section_accessor symbols(loader->GetReader(),
                                                     loader->GetSymTab());
        ELFIO::Elf_Xword sym_num = symbols.get_symbols_num();
        if (sym_num == 0) {
          std::cerr << "\tFAILED" << std::endl;
          continue;
        }

        std::vector<Symbol_Name_Addr> map_symbols;
        for (int i = 0; i < sym_num; i++) {
          std::string name;
          ELFIO::Elf64_Addr value;
          ELFIO::Elf_Xword size;
          unsigned char bind;
          unsigned char type;
          ELFIO::Elf_Half section_index;
          unsigned char other;
          symbols.get_symbol(i, name, value, size, bind, type, section_index,
                             other);
          if (s->get_index() == section_index) {
            Symbol_Name_Addr sna = {.name = name, .value = value, .size = size};
            map_symbols.push_back(sna);
          }
        }

        if (map_symbols.size() == 0) {
          std::cerr << "\tFAILED" << std::endl;
          continue;
        }

        int map_size = s->get_size() / map_symbols.size();
        if (map_size == 0) {
          std::cerr << "\tFAILED" << std::endl;
          continue;
        }

        Elf64_BpfMapDefinition *bpfmap =
            (Elf64_BpfMapDefinition *)malloc(map_size);
        for (Symbol_Name_Addr sna : map_symbols) {
          // calculate the offset from the start of maps
          // output each part of the data
          memcpy(bpfmap, data + sna.value, map_size);
          std::cout << "\t" << sna.name << "{" << std::endl;
          std::cout << "\t\ttype: " << bpfmap->type << std::endl;
          std::cout << "\t\tkey_size: " << bpfmap->key_size << std::endl;
          std::cout << "\t\tvalue_size: " << bpfmap->value_size << std::endl;
          std::cout << "\t\tmax_entries: " << bpfmap->max_entries << std::endl;
          std::cout << "\t\tinner_map_idx: " << bpfmap->inner_map_idx
                    << std::endl;
          std::cout << "\t\tplatform_specific_data: "
                    << bpfmap->platform_specific_data << std::endl;
          std::cout << "\t}" << std::endl;
        }

        free(bpfmap);
        bpfmap = nullptr;
      } else {
        n = cs_disasm(dis, reinterpret_cast<const uint8_t *>(data), size, 0, 0,
                      &insns);
        if (n <= 0) {
          std::cerr << "\tFAILED" << std::endl;
          continue;
        }

        for (size_t i = 0; i < n; i++) {
          std::cout << "\t";
          for (size_t j = 0; j < 8; j++) {
            std::cout << "0x" << std::setw(2) << std::setfill('0') << std::hex
                      << (int)insns[i].bytes[j] << " ";
          }

          std::cout << insns[i].mnemonic << " " << insns[i].op_str << std::endl;
        }
      }
    }
  }

  return 0;
}
