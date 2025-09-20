#include <vector>
#include <elfio/elfio.hpp>
#include <string>

class ElfLoader {
public:
  ElfLoader(std::string filename);
  int LoadFile();
  std::vector<ELFIO::section *> GetCodeSections();

private:
  std::string filename;
  std::vector<ELFIO::section *> codeSections;
  ELFIO::elfio reader;
};