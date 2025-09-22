#include <capstone/capstone.h>
#include <elfio/elfio.hpp>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "disassembler.h"

Disassembler::Disassembler(std::vector<ELFIO::section *> sections) {
  this->sections = sections;
}

int Disassembler::Disassemble() {
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
      std::cout << s->get_name() << ":" << std::endl;
      n = cs_disasm(dis, reinterpret_cast<const uint8_t*>(data), size, 0, 0, &insns);
      if (n <= 0) {
        std::cerr << "\tFAILED" << std::endl;
        continue;
      }

      for (size_t i = 0; i < n; i++) {
        std::cout << "\t";
        for (size_t j = 0; j < 8; j++) {
          std::cout << "0x" << std::setw(2) << std::setfill('0') << std::hex << (int)insns[i].bytes[j] << " ";
        }

        std::cout << insns[i].mnemonic << " " << insns[i].op_str << std::endl;
      }
    }
  }

  return 0;
}
