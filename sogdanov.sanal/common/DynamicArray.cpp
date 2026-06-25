#include "DynamicArray.hpp"

void sogdanov::initArray(DynamicArray& arr) {
  arr.data = nullptr;
  arr.capacity = 0;
  arr.size = 0;
}

void sogdanov::pushBack(DynamicArray& arr, const Person& p) {
  if (arr.size == arr.capacity) {
    const size_t newCap = (arr.capacity == 0) ? 1 : arr.capacity * 2;
    Person* const newData = new Person[newCap];
    for (size_t i = 0; i < arr.size; ++i) {
      newData[i] = arr.data[i];
    }
    delete[] arr.data;
    arr.data = newData;
    arr.capacity = newCap;
  }
  arr.data[arr.size++] = p;
}

void sogdanov::destroyArray(DynamicArray& arr) {
  delete[] arr.data;
  arr.data = nullptr;
  arr.capacity = 0;
  arr.size = 0;
}
