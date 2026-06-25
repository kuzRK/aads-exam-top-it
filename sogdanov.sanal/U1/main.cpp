#include <iostream>
#include <fstream>
#include <string>
#include "../common/DynamicArray.hpp"
#include "../common/HashTable.hpp"

int main(int argc, char** argv)
{
  using namespace sogdanov;

  std::string inFile;
  std::string outFile;
  bool hasIn = false;
  bool hasOut = false;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.find("in:") == 0) {
      if (hasIn) {
        std::cerr << "Duplicate in argument\n";
        return 1;
      }
      inFile = arg.substr(3);
      hasIn = true;
    } else if (arg.find("out:") == 0) {
      if (hasOut) {
        std::cerr << "Duplicate out argument\n";
        return 1;
      }
      outFile = arg.substr(4);
      hasOut = true;
    } else {
      std::cerr << "Invalid argument format\n";
      return 1;
    }
  }

  std::istream* input = &std::cin;
  std::ostream* output = &std::cout;
  std::ifstream fin;
  std::ofstream fout;

  if (hasIn) {
    fin.open(inFile);
    if (!fin.is_open()) {
      std::cerr << "Cannot open input file\n";
      return 2;
    }
    input = &fin;
  }

  HashTable< size_t, bool > seenIds;
  initHashTable(seenIds);
  DynamicArray persons;
  initArray(persons);

  size_t successCount = 0;
  size_t ignoreCount = 0;
  std::string line;

  while (std::getline(*input, line)) {
    if (line.empty()) {
      ignoreCount++;
      continue;
    }

    size_t charsProcessed = 0;
    size_t id = 0;
    bool parsedId = false;

    try {
      id = std::stoull(line, &charsProcessed);
      parsedId = true;
    } catch (...) {
      ignoreCount++;
      continue;
    }

    if (parsedId) {
      std::string info = line.substr(charsProcessed);
      const size_t start = info.find_first_not_of(" \t");
      if (start != std::string::npos) {
        info = info.substr(start);
        if (insertHash(seenIds, id, true)) {
          Person p;
          p.id = id;
          p.info = info;
          pushBack(persons, p);
          successCount++;
        } else {
          ignoreCount++;
        }
      } else {
        ignoreCount++;
      }
    }
  }

  if (hasIn) {
    fin.close();
  }

  if (hasOut) {
    fout.open(outFile);
    if (!fout.is_open()) {
      std::cerr << "Cannot open output file\n";
      destroyArray(persons);
      destroyHashTable(seenIds);
      return 2;
    }
    output = &fout;
  }

  for (size_t i = 0; i < persons.size; ++i) {
    *output << persons.data[i].id << " " << persons.data[i].info << "\n";
  }

  std::cerr << successCount << " " << ignoreCount << "\n";

  destroyArray(persons);
  destroyHashTable(seenIds);

}

