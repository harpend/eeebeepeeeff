#include <vector>
#include <elfio/elfio.hpp>

class Disassembler {
public:
  Disassembler(std::vector<ELFIO::section *> sections);
  int Disassemble();

private:
  std::vector<ELFIO::section *> sections;
};