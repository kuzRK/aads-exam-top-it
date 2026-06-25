#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <cstddef>

namespace sogdanov {

  template< class Key, class Value >
  struct HashTable {
    Key* keys;
    Value* values;
    bool* occupied;
    size_t capacity;
    size_t size;
  };

  template< class Key, class Value >
  void initHashTable(HashTable< Key, Value >& table, const size_t initialCapacity = 16) {
    table.capacity = initialCapacity;
    table.size = 0;
    table.keys = new Key[initialCapacity];
    table.values = new Value[initialCapacity];
    table.occupied = new bool[initialCapacity]{false};
  }

  template< class Key, class Value >
  void destroyHashTable(HashTable< Key, Value >& table) {
    delete[] table.keys;
    delete[] table.values;
    delete[] table.occupied;
    table.keys = nullptr;
    table.values = nullptr;
    table.occupied = nullptr;
    table.capacity = 0;
    table.size = 0;
  }

  template< class Key, class Value >
  bool insertHash(HashTable< Key, Value >& table, const Key& key, const Value& value) {
    if (table.size * 2 >= table.capacity) {
      const size_t oldCapacity = table.capacity;
      Key* const oldKeys = table.keys;
      Value* const oldValues = table.values;
      bool* const oldOccupied = table.occupied;

      table.capacity *= 2;
      table.keys = new Key[table.capacity];
      table.values = new Value[table.capacity];
      table.occupied = new bool[table.capacity]{false};
      table.size = 0;

      for (size_t i = 0; i < oldCapacity; ++i) {
        if (oldOccupied[i]) {
          insertHash(table, oldKeys[i], oldValues[i]);
        }
      }
      delete[] oldKeys;
      delete[] oldValues;
      delete[] oldOccupied;
    }

    const size_t h1 = key % table.capacity;
    const size_t h2 = (key % (table.capacity - 1)) | 1;
    size_t index = h1;

    for (size_t i = 0; i < table.capacity; ++i) {
      if (!table.occupied[index]) {
        table.keys[index] = key;
        table.values[index] = value;
        table.occupied[index] = true;
        table.size++;
        return true;
      }
      if (table.keys[index] == key) {
        return false;
      }
      index = (index + h2) % table.capacity;
    }
    return false;
  }

}

#endif

