#include <elfio/elfio.hpp>
#include <map>
#include <string>
#include <vector>

class ElfLoader {
public:
  ElfLoader(std::string filename);
  int LoadFile();
  std::vector<ELFIO::section *> GetCodeSections();
  ELFIO::elfio &GetReader();
  ELFIO::section *GetSymTab();
  std::map<ELFIO::Elf64_Addr, std::string> GetRelocationMap(ELFIO::section *s);

private:
  std::string filename;
  std::vector<ELFIO::section *> codeSections;
  std::vector<ELFIO::section *> relSections;
  ELFIO::elfio reader;
  ELFIO::section *symTab;
};
