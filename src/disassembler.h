#include "loader.h"
#include <elfio/elfio.hpp>
#include <vector>

typedef struct {
  ELFIO::Elf64_Word type;
  ELFIO::Elf64_Word key_size;
  ELFIO::Elf64_Word value_size;
  ELFIO::Elf64_Word max_entries;
  ELFIO::Elf64_Word inner_map_idx;
  unsigned char platform_specific_data[];
} Elf64_BpfMapDefinition;

typedef struct {
  std::string name;
  ELFIO::Elf64_Addr value;
  ELFIO::Elf_Xword size;
} Symbol_Name_Addr;

class Disassembler {
public:
  Disassembler(std::vector<ELFIO::section *> sections);
  int Disassemble(ElfLoader *loader);

private:
  std::vector<ELFIO::section *> sections;
};
