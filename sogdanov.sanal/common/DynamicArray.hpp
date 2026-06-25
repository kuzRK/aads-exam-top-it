#ifndef DYNAMICARRAY_HPP
#define DYNAMICARRAY_HPP

#include <string>
#include <cstddef>

namespace sogdanov {

  struct Person {
    size_t id;
    std::string info;
  };

  struct DynamicArray {
    Person* data;
    size_t capacity;
    size_t size;
  };

  void initArray(DynamicArray& arr);
  void pushBack(DynamicArray& arr, const Person& p);
  void destroyArray(DynamicArray& arr);

}

#endif
