#include <iostream>
#include <fstream>
#include <string>
#include "../common/DynamicArray.hpp"
#include "../common/HashTable.hpp"

int main(int argc, char** argv)
{
  using namespace sogdanov;

  if (argc > 3) {
    std::cerr << "Invalid arguments\n";
    return 0;
  }

  std::string inFile;
  std::string outFile;
  bool hasIn = false;
  bool hasOut = false;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.find("in:") == 0) {
      if (hasIn) {
        std::cerr << "Invalid arguments\n";
        return 1;
      }
      inFile = arg.substr(3);
      hasIn = true;
    } else if (arg.find("out:") == 0) {
      if (hasOut) {
        std::cerr << "Invalid arguments\n";
        return 1;
      }
      outFile = arg.substr(4);
      hasOut = true;
    } else {
      std::cerr << "Invalid arguments\n";
      return 1;
    }
  }

  std::istream* input = &std::cin;
  std::ifstream fin;

  if (hasIn) {
    fin.open(inFile);
    if (!fin.is_open()) {
      std::cerr << "Cannot open file\n";
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
    if (line.empty() || line.find_first_not_of(" \t\r") == std::string::npos) {
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

  std::cerr << successCount << " " << ignoreCount << "\n";

  if (hasOut) {
    std::ofstream fout(outFile);
    if (!fout.is_open()) {
      std::cerr << "Cannot open file\n";
      destroyArray(persons);
      destroyHashTable(seenIds);
      return 2;
    }
    for (size_t i = 0; i < persons.size; ++i) {
      fout << persons.data[i].id << " " << persons.data[i].info << "\n";
    }
    std::cout << "in file " << outFile << "\n";
    for (size_t i = 0; i < persons.size; ++i) {
      std::cout << persons.data[i].id << " " << persons.data[i].info << "\n";
    }
    fout.close();
  } else {
    for (size_t i = 0; i < persons.size; ++i) {
      std::cout << persons.data[i].id << " " << persons.data[i].info << "\n";
    }
  }

  destroyArray(persons);
  destroyHashTable(seenIds);

  return 0;
}
