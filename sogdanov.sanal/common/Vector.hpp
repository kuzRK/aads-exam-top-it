#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cstddef>

namespace sogdanov {

  template< class T >
  struct Vector {
    T* data;
    size_t capacity;
    size_t size;
  };

  template< class T >
  void initVector(Vector< T >& vec, const size_t initialCapacity = 2) {
    vec.capacity = initialCapacity;
    vec.size = 0;
    vec.data = new T[initialCapacity];
  }

  template< class T >
  void destroyVector(Vector< T >& vec) {
    delete[] vec.data;
    vec.data = nullptr;
    vec.capacity = 0;
    vec.size = 0;
  }

  template< class T >
  void pushBack(Vector< T >& vec, const T& val) {
    if (vec.size == vec.capacity) {
      const size_t newCap = vec.capacity * 2;
      T* const newData = new T[newCap];
      for (size_t i = 0; i < vec.size; ++i) {
        newData[i] = vec.data[i];
      }
      delete[] vec.data;
      vec.data = newData;
      vec.capacity = newCap;
    }
    vec.data[vec.size++] = val;
  }

  template< class T >
  void removeAt(Vector< T >& vec, const size_t index) {
    if (index >= vec.size) {
      return;
    }
    for (size_t i = index; i < vec.size - 1; ++i) {
      vec.data[i] = vec.data[i + 1];
    }
    vec.size--;
  }

}

#endif

