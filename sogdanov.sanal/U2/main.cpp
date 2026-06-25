#include <iostream>
#include <fstream>
#include <string>
#include "../common/HashTable.hpp"
#include "../common/Vector.hpp"

namespace sogdanov {

  struct Meeting {
    size_t otherId;
    size_t duration;
  };

  struct PersonRecord {
    size_t id;
    bool isAnon;
    std::string description;
    Vector< Meeting > meetings;
  };

  void sortMeetings(Vector< Meeting >& vec) {
    for (size_t i = 0; i < vec.size; ++i) {
      for (size_t j = 0; j < vec.size - 1 - i; ++j) {
        if (vec.data[j].otherId > vec.data[j + 1].otherId ||
           (vec.data[j].otherId == vec.data[j + 1].otherId && vec.data[j].duration > vec.data[j + 1].duration)) {
          Meeting temp = vec.data[j];
          vec.data[j] = vec.data[j + 1];
          vec.data[j + 1] = temp;
        }
      }
    }
  }

  void sortSizeT(Vector< size_t >& vec) {
    for (size_t i = 0; i < vec.size; ++i) {
      for (size_t j = 0; j < vec.size - 1 - i; ++j) {
        if (vec.data[j] > vec.data[j + 1]) {
          size_t temp = vec.data[j];
          vec.data[j] = vec.data[j + 1];
          vec.data[j + 1] = temp;
        }
      }
    }
  }

  PersonRecord* getOrCreatePerson(HashTable< size_t, size_t >& indexMap, Vector< PersonRecord* >& registry, const size_t id) {
    size_t index = 0;
    for (size_t i = 0; i < indexMap.capacity; ++i) {
      if (indexMap.occupied[i] && indexMap.keys[i] == id) {
        return registry.data[indexMap.values[i]];
      }
    }

    PersonRecord* const newPerson = new PersonRecord;
    newPerson->id = id;
    newPerson->isAnon = true;
    newPerson->description = "";
    initVector(newPerson->meetings);

    pushBack(registry, newPerson);
    insertHash(indexMap, id, registry.size - 1);
    return newPerson;
  }

}

int main(int argc, char* argv[]) {
  using namespace sogdanov;

  std::string inFilename;
  std::string dataFilename;
  bool hasIn = false;
  bool hasData = false;

  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg.substr(0, 3) == "in:") {
      if (hasIn) {
        std::cerr << "Duplicate in file\n";
        return 1;
      }
      inFilename = arg.substr(3);
      hasIn = true;
    } else if (arg.substr(0, 5) == "data:") {
      if (hasData) {
        std::cerr << "Duplicate data file\n";
        return 1;
      }
      dataFilename = arg.substr(5);
      hasData = true;
    } else {
      std::cerr << "Invalid argument format\n";
      return 1;
    }
  }

  if (!hasData) {
    std::cerr << "Missing data file\n";
    return 1;
  }

  HashTable< size_t, size_t > indexMap;
  initHashTable(indexMap, 128);
  Vector< PersonRecord* > registry;
  initVector(registry);

  if (hasIn) {
    std::ifstream fin(inFilename);
    if (!fin.is_open()) {
      std::cerr << "Cannot open in file\n";
      return 2;
    }
    std::string line;
    while (std::getline(fin, line)) {
      if (line.empty()) {
        continue;
      }
      size_t charsProcessed = 0;
      size_t id = 0;
      try {
        id = std::stoull(line, &charsProcessed);
      } catch (...) {
        continue;
      }
      std::string info = line.substr(charsProcessed);
      const size_t start = info.find_first_not_of(" \t");
      if (start != std::string::npos) {
        PersonRecord* const p = getOrCreatePerson(indexMap, registry, id);
        p->description = info.substr(start);
        p->isAnon = false;
      }
    }
  }

  std::ifstream fdata(dataFilename);
  if (!fdata.is_open()) {
    std::cerr << "Cannot open data file\n";
    return 2;
  }

  size_t id1 = 0;
  size_t id2 = 0;
  size_t duration = 0;
  while (fdata >> id1 >> id2 >> duration) {
    if (fdata.fail()) {
      std::cerr << "Meeting data error\n";
      return 3;
    }
    if (id1 == id2) {
      continue;
    }
    PersonRecord* const p1 = getOrCreatePerson(indexMap, registry, id1);
    PersonRecord* const p2 = getOrCreatePerson(indexMap, registry, id2);
    Meeting m1;
    m1.otherId = id2;
    m1.duration = duration;
    pushBack(p1->meetings, m1);

    Meeting m2;
    m2.otherId = id1;
    m2.duration = duration;
    pushBack(p2->meetings, m2);
  }
  if (!fdata.eof() && fdata.fail()) {
      std::cerr << "Meeting data error\n";
      return 3;
  }

  std::string cmd;
  while (std::cin >> cmd) {
    if (cmd == "anons") {
      Vector< size_t > anons;
      initVector(anons);
      for (size_t i = 0; i < registry.size; ++i) {
        if (registry.data[i] != nullptr && registry.data[i]->isAnon) {
          pushBack(anons, registry.data[i]->id);
        }
      }
      sortSizeT(anons);
      for (size_t i = 0; i < anons.size; ++i) {
        std::cout << anons.data[i] << "\n";
      }
      destroyVector(anons);
    } else if (cmd == "deanon") {
      size_t anonId = 0;
      size_t targetId = 0;
      if (!(std::cin >> anonId >> targetId)) {
        std::cout << "<INVALID COMMAND>\n";
        std::cin.clear();
        continue;
      }
      PersonRecord* pAnon = nullptr;
      PersonRecord* pTarget = nullptr;
      for (size_t i = 0; i < registry.size; ++i) {
        if (registry.data[i] != nullptr) {
          if (registry.data[i]->id == anonId) pAnon = registry.data[i];
          if (registry.data[i]->id == targetId) pTarget = registry.data[i];
        }
      }

      if (!pAnon || !pTarget || !pAnon->isAnon || pTarget->isAnon) {
        std::cout << "<INVALID COMMAND>\n";
        continue;
      }

      for (size_t i = 0; i < pAnon->meetings.size; ++i) {
        const size_t other = pAnon->meetings.data[i].otherId;
        const size_t dur = pAnon->meetings.data[i].duration;
        if (other == targetId) {
          continue;
        }

        Meeting m;
        m.otherId = other;
        m.duration = dur;
        pushBack(pTarget->meetings, m);

        for (size_t j = 0; j < registry.size; ++j) {
          if (registry.data[j] != nullptr && registry.data[j]->id == other) {
            for (size_t k = 0; k < registry.data[j]->meetings.size; ++k) {
              if (registry.data[j]->meetings.data[k].otherId == anonId) {
                registry.data[j]->meetings.data[k].otherId = targetId;
              }
            }
          }
        }
      }
      for (size_t i = 0; i < pTarget->meetings.size; ) {
        if (pTarget->meetings.data[i].otherId == targetId || pTarget->meetings.data[i].otherId == anonId) {
          removeAt(pTarget->meetings, i);
        } else {
          ++i;
        }
      }

      for (size_t i = 0; i < registry.size; ++i) {
        if (registry.data[i] == pAnon) {
          destroyVector(registry.data[i]->meetings);
          delete registry.data[i];
          registry.data[i] = nullptr;
          break;
        }
      }

    } else if (cmd == "redesc") {
      size_t id = 0;
      if (!(std::cin >> id)) {
         std::cout << "<INVALID COMMAND>\n";
         std::cin.clear();
         continue;
      }
      char c = '\0';
      while (std::cin.get(c) && c != '"') {}
      std::string newDesc;
      while (std::cin.get(c) && c != '"') {
        newDesc += c;
      }

      PersonRecord* p = nullptr;
      for (size_t i = 0; i < registry.size; ++i) {
        if (registry.data[i] != nullptr && registry.data[i]->id == id) {
          p = registry.data[i];
          break;
        }
      }

      if (!p) {
        std::cout << "<INVALID COMMAND>\n";
      } else {
        p->description = newDesc;
        p->isAnon = false;
      }

    } else if (cmd == "desc") {
      size_t id = 0;
      if (!(std::cin >> id)) {
         std::cout << "<INVALID COMMAND>\n";
         std::cin.clear();
         continue;
      }
      PersonRecord* p = nullptr;
      for (size_t i = 0; i < registry.size; ++i) {
        if (registry.data[i] != nullptr && registry.data[i]->id == id) {
          p = registry.data[i];
          break;
        }
      }
      if (!p) {
        std::cout << "<INVALID COMMAND>\n";
      } else if (p->isAnon) {
        std::cout << "<ANON>\n";
      } else {
        std::cout << p->description << "\n";
      }

    } else if (cmd == "meets" || cmd == "less" || cmd == "greater") {
      size_t timeLimit = 0;
      size_t id = 0;
      if (cmd == "less" || cmd == "greater") {
        if (!(std::cin >> timeLimit >> id)) {
           std::cout << "<INVALID COMMAND>\n";
           std::cin.clear();
           continue;
        }
      } else {
        if (!(std::cin >> id)) {
           std::cout << "<INVALID COMMAND>\n";
           std::cin.clear();
           continue;
        }
      }

      PersonRecord* p = nullptr;
      for (size_t i = 0; i < registry.size; ++i) {
        if (registry.data[i] != nullptr && registry.data[i]->id == id) {
          p = registry.data[i];
          break;
        }
      }

      if (!p) {
        std::cout << "<INVALID COMMAND>\n";
        continue;
      }

      Vector< Meeting > filtered;
      initVector(filtered);
      for (size_t i = 0; i < p->meetings.size; ++i) {
        const size_t dur = p->meetings.data[i].duration;
        if (cmd == "meets" || (cmd == "less" && dur < timeLimit) || (cmd == "greater" && dur > timeLimit)) {
          pushBack(filtered, p->meetings.data[i]);
        }
      }
      sortMeetings(filtered);
      for (size_t i = 0; i < filtered.size; ++i) {
        std::cout << filtered.data[i].otherId << " " << filtered.data[i].duration << "\n";
      }
      destroyVector(filtered);

    } else if (cmd == "commons") {
      size_t id1 = 0;
      size_t id2 = 0;
      if (!(std::cin >> id1 >> id2)) {
         std::cout << "<INVALID COMMAND>\n";
         std::cin.clear();
         continue;
      }
      PersonRecord* p1 = nullptr;
      PersonRecord* p2 = nullptr;
      for (size_t i = 0; i < registry.size; ++i) {
        if (registry.data[i] != nullptr) {
          if (registry.data[i]->id == id1) p1 = registry.data[i];
          if (registry.data[i]->id == id2) p2 = registry.data[i];
        }
      }

      if (!p1 || !p2) {
        std::cout << "<INVALID COMMAND>\n";
        continue;
      }

      Vector< size_t > commonIds;
      initVector(commonIds);

      for (size_t i = 0; i < p1->meetings.size; ++i) {
        for (size_t j = 0; j < p2->meetings.size; ++j) {
          if (p1->meetings.data[i].otherId == p2->meetings.data[j].otherId) {
            bool exists = false;
            for (size_t k = 0; k < commonIds.size; ++k) {
              if (commonIds.data[k] == p1->meetings.data[i].otherId) {
                exists = true;
                break;
              }
            }
            if (!exists) {
              pushBack(commonIds, p1->meetings.data[i].otherId);
            }
          }
        }
      }
      sortSizeT(commonIds);
      for (size_t i = 0; i < commonIds.size; ++i) {
        std::cout << commonIds.data[i] << "\n";
      }
      destroyVector(commonIds);

    } else if (cmd == "out-persons") {
      std::string outFilename;
      if (!(std::cin >> outFilename)) {
         std::cout << "<INVALID COMMAND>\n";
         std::cin.clear();
         continue;
      }
      std::ofstream fout(outFilename);
      if (!fout.is_open()) {
        std::cout << "<INVALID COMMAND>\n";
        continue;
      }
      for (size_t i = 0; i < registry.size; ++i) {
        if (registry.data[i] != nullptr && !registry.data[i]->isAnon) {
          fout << registry.data[i]->id << " " << registry.data[i]->description << "\n";
        }
      }
    } else {
      std::cout << "<INVALID COMMAND>\n";
    }
  }

  for (size_t i = 0; i < registry.size; ++i) {
    if (registry.data[i] != nullptr) {
      destroyVector(registry.data[i]->meetings);
      delete registry.data[i];
    }
  }
  destroyVector(registry);
  destroyHashTable(indexMap);

}
