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
  void initHashTable(HashTable< Key, Value >& table, const size_t initialCapacity = 16);

  template< class Key, class Value >
  void destroyHashTable(HashTable< Key, Value >& table);

  template< class Key, class Value >
  bool insertHash(HashTable< Key, Value >& table, const Key& key, const Value& value);

}

#include "HashTable.cpp"

#endif
