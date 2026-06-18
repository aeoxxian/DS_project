#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <iostream>
#include <cstdlib>

template <typename T>
class DynamicArray {
private:
    T*  data;
    int sz;
    int cap;

    void resize(int newCap) {
        T* newData = new T[newCap];
        for (int i = 0; i < sz; ++i) newData[i] = data[i];
        delete[] data;
        data = newData;
        cap  = newCap;
    }

public:
    DynamicArray() : data(nullptr), sz(0), cap(0) {}
    ~DynamicArray() { delete[] data; }

    DynamicArray(const DynamicArray& other) : data(nullptr), sz(0), cap(0) {
        for (int i = 0; i < other.sz; ++i) pushBack(other.data[i]);
    }
    DynamicArray& operator=(const DynamicArray& other) {
        if (this == &other) return *this;
        delete[] data; data = nullptr; sz = 0; cap = 0;
        for (int i = 0; i < other.sz; ++i) pushBack(other.data[i]);
        return *this;
    }

    int  size()    const { return sz; }
    bool isEmpty() const { return sz == 0; }

    void pushBack(const T& value) {
        if (sz == cap) resize(cap == 0 ? 4 : cap * 2);
        data[sz++] = value;
    }

    bool removeAt(int index) {
        if (index < 0 || index >= sz) return false;
        for (int i = index; i < sz - 1; ++i) data[i] = data[i + 1];
        --sz;
        return true;
    }

    T& operator[](int index) {
        if (sz == 0) { std::cerr << "[DynamicArray] operator[] on empty array\n"; std::exit(1); }
        if (index < 0) index = 0;
        if (index >= sz) index = sz - 1;
        return data[index];
    }
    const T& operator[](int index) const {
        if (sz == 0) { std::cerr << "[DynamicArray] operator[] on empty array\n"; std::exit(1); }
        if (index < 0) index = 0;
        if (index >= sz) index = sz - 1;
        return data[index];
    }

    void clear() { sz = 0; }
};

#endif
