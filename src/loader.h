#include <elfio/elfio.hpp>
#include <string>
#include <vector>

class ElfLoader {
public:
  ElfLoader(std::string filename);
  int LoadFile();
  std::vector<ELFIO::section *> GetCodeSections();
  ELFIO::elfio &GetReader();
  ELFIO::section *GetSymTab();

private:
  std::string filename;
  std::vector<ELFIO::section *> codeSections;
  ELFIO::elfio reader;
  ELFIO::section *symTab;
};
