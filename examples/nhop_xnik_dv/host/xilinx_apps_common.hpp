/*
 * Copyright 2021 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _XILINX_APPS_COMMON_H_
#define _XILINX_APPS_COMMON_H_

#include <cstdlib>  // std::size_t + ...
#include <cstring>
#include <utility>
#include <algorithm>
#include <iterator>
#include <cstddef>
#include <initializer_list>
#include <type_traits>  // std::is_trivially_copiable
#include <cstdint>  // std::uintptr_t


// Turn this macro on to trace memory allocations
//#define XILINX_APPS_MEM_DEBUG 1

#ifdef XILINX_APPS_MEM_DEBUG
#include <iostream>  // std::cout, std::ostream
#endif

namespace xilinx_apps {

class XAllocator {
public:
    XAllocator() : mallocFunc_(std::malloc), freeFunc_(std::free) {}
    XAllocator(const XAllocator &) = default;
    XAllocator &operator=(const XAllocator &) = default;
    void *malloc(std::size_t size) const { return mallocFunc_(size); }
    void free(void *pData) const { freeFunc_(pData); }

#ifdef XILINX_APPS_MEM_DEBUG
    void print(std::ostream &os) const {
        std::cout << "XAllocator {malloc = "
            << std::hex << reinterpret_cast<std::uintptr_t>(mallocFunc_) << std::dec;
        std::cout << ", free = "
            << std::hex << reinterpret_cast<std::uintptr_t>(freeFunc_) << std::dec
            << "}";
    }
    friend std::ostream &operator<<(std::ostream &os, const XAllocator &a) { a.print(os); return os; }
#endif
    
private:
    void * (*mallocFunc_)(std::size_t size) = nullptr;
    void (*freeFunc_)(void *pData) = nullptr;
};


/**
 * @brief Simple string class for avoiding ABI problems
 * 
 * This class provides string memory management like `std::string` but without ABI compatibility issues.
 * ABI (Application Binary Interface) problems can arise when using standard C++ classes in a coding environment
 * that uses multiple compiler versions.  For example, if you compile your application code using a different
 * version of the g++ compiler from the one used to compile this library, then when using dynamic loading,
 * standard C++ types, such as `std::string`, may not pass correctly from your code into the library.
 * This string class avoids these compatibility issues by using only plain data types internally.
 */
class XString {
public:
    XString() = default;
    ~XString() { clear(); }
    XString(const XString &other) { copyIn(other.data); }
    XString(XString &&other) { steal(std::forward<XString>(other)); }
    XString(const char *cstr) { copyIn(cstr); }
    XString(const std::string &str) { copyIn(str.c_str()); }
    XString &operator=(const XString &other) { copyIn(other.data); return *this; }
    XString &operator=(XString &&other) { steal(std::forward<XString>(other)); return *this; }
    XString &operator=(const char *cstr) { copyIn(cstr); return *this; }
    XString &operator=(const std::string &str) { copyIn(str.c_str()); return *this; }
    operator std::string() const { return std::string(c_str()); }
    operator const char *() const { return c_str(); }
    const char *c_str() const { return data == nullptr ? "" : data; }
    bool empty() const { return data == nullptr || std::strlen(data) == 0; }

    bool operator==(const XString &other) const {
        if (data == nullptr && other.data == nullptr)
            return true;
        if (data == nullptr || other.data == nullptr)
            return false;
        return std::strcmp(data, other.data) == 0;
    }

    void clear() {
        if (data != nullptr)
            allocator.free(data);
        data = nullptr;
    }

private:
    char *data = nullptr;
    XAllocator allocator = XAllocator();
    
    void copyIn(const char *other) {
        clear();
        if (other != nullptr) {
            data = static_cast<char *>(allocator.malloc(std::strlen(other) + 1));
            std::strcpy(data, other);
        }
    }
    
    void steal(XString &&other) {
        clear();
        data = other.data;
        other.data = nullptr;
    }
};


template <typename T>
class XVector {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    
    template <typename U>
    class IteratorTemplate {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = U;
        using difference_type = XVector<T>::difference_type;
        using pointer = U *;
        using reference = U &;
      
    private:            
        pointer p = nullptr;

    public:
        explicit IteratorTemplate(U *pStart = nullptr) : p(pStart) {}
        IteratorTemplate(const IteratorTemplate &other) = default;
        ~IteratorTemplate() {}
        IteratorTemplate &operator=(const IteratorTemplate &other) = default;
//        IteratorTemplate &operator=(U *pInput) { p = pInput; return *this; }
        
        U &operator*() { return *p; }
        const U &operator*() const { return *p; }
        IteratorTemplate &operator++() { ++p; return *this; }
        IteratorTemplate &operator++(int) {
            IteratorTemplate oldVal(*this);
            ++p;
            return oldVal;
        }
        IteratorTemplate &operator--() { --p; return *this; }
        IteratorTemplate &operator--(int) {
            IteratorTemplate oldVal(*this);
            --p;
            return oldVal;
        }
        bool operator==(const IteratorTemplate &other) const { return p == other.p; }
        bool operator!=(const IteratorTemplate &other) const { return !(p == other.p); }
        IteratorTemplate &operator+=(difference_type n) { p += n; return *this; }
        IteratorTemplate &operator-=(difference_type n) { p -= n; return *this; }
        IteratorTemplate &operator+(difference_type n) { return IteratorTemplate(p + n); }
        IteratorTemplate &operator-(difference_type n) { return IteratorTemplate(p - n); }
        difference_type operator-(const IteratorTemplate &other) const { return p - other.p; }
        bool operator<(const IteratorTemplate &other) const { return p < other.p; }
    };
    using iterator = IteratorTemplate<T>;
    using const_iterator = IteratorTemplate<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    
    XVector() {}
    XVector(size_type count) { resize(count); }
    XVector(size_type count, const_reference value) { resize(count, value); }
    XVector(const std::initializer_list<T> &list) {
        reserve(list.size());
        for (const_reference e : list)
            push_back(e);
    }
    XVector(const XVector &other) { copyIn(other); }
    XVector(XVector &&other) { steal(std::forward<XVector>(other)); }
    ~XVector() { reset(); }
    XVector &operator=(const XVector &other) {
        copyIn(other);
        return *this;
    }
    XVector &operator=(XVector &&other) {
        steal(std::forward<XVector>(other));
        return *this;
    }
    const_reference operator[](size_type i) const { return data_[i]; }
    reference operator[](size_type i) { return data_[i]; }
    pointer data() { return data_; }
    const_pointer data() const { return data_; }
    
    iterator begin() { return iterator(data_); }
    const_iterator begin() const { return cbegin(); }
    const_iterator cbegin() const { return const_iterator(data_); }
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return crbegin(); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }
    iterator end() { return iterator(data_ + size_); }
    const_iterator end() const { return cend(); }
    const_iterator cend() const { return const_iterator(data_ + size_); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return crend(); }
    const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }
    
    bool empty() const { return data_ == nullptr || size_ == 0; }
    size_type size() const { return size_; }

    void reserve(size_type new_cap) {
        if (new_cap <= capacity_)
            return;
        auto oldData = data_;
        data_ = static_cast<T *>(allocMem(sizeof(T) * new_cap));
        capacity_ = new_cap;
        if (size_ > 0)
            copyToRawMem(oldData, oldData + size_, data_);
        freeArray(oldData, oldData + size_);
    }

    void clear() {
        destroyElements(data_, data_ + size_);
        size_ = 0;
    }
    
    void push_back(const T &value) {
        expand();
        (void) new (data_ + size_) T(value);
        ++size_;
    }
    void push_back(T &&value) {
        expand();
        (void) new (data_ + size_) T(std::forward<T>(value));
        ++size_;
    }
    void resize(size_type count) {
        if (count > size_) {
            reserve(count);
            for (pointer pT = data_ + size_, pEnd = data_ + count; pT < pEnd; ++pT)
                (void) new (pT) T();
        }
        else
            destroyElements(data_ + count, data_ + size_);
        size_ = count;
    }
    void resize(size_type count, const_reference value) {
        if (count > size_) {
            reserve(count);
            for (pointer pT = data_ + size_, pEnd = data_ + count; pT < pEnd; ++pT)
                (void) new (pT) T(value);
        }
        else
            destroyElements(data_ + count, data_ + size_);
        size_ = count;
    }

private:
    size_type size_ = 0;
    size_type capacity_ = 0;
    pointer data_ = nullptr;
    XAllocator allocator_ = XAllocator();

    void *allocMem(size_type size) const {
#ifdef XILINX_APPS_MEM_DEBUG
        static bool isFirstTime = true;
        if (isFirstTime) {
            std::cout << "DEBUG: xilinx_apps::XVector: first malloc, " << allocator_ << std::endl;
            isFirstTime = false;
        }
        std::cout << "DEBUG xilinx_apps::XVector: malloc(" << size << ") = ";
#endif
        void *pData = allocator_.malloc(size);
#ifdef XILINX_APPS_MEM_DEBUG
        std::cout << std::hex << reinterpret_cast<std::uintptr_t>(pData) << std::dec << std::endl;
#endif
        return pData;
    }
    
    void freeMem(void *pData) const {
#ifdef XILINX_APPS_MEM_DEBUG
        static bool isFirstTime = true;
        if (isFirstTime) {
            std::cout << "DEBUG: xilinx_apps::XVector: first free, " << allocator_ << std::endl;
            isFirstTime = false;
        }
        std::cout << "DEBUG xilinx_apps::XVector: free(" << std::hex << reinterpret_cast<std::uintptr_t>(pData)
            << std::dec << ")" << std::endl;
#endif
        allocator_.free(pData);
    }
    
    static void copyToRawMem(const_pointer pSrcStart, const_pointer pSrcEnd, pointer pDest) {
//        if (std::is_trivially_copyable<value_type>::value)  C++11 feature not available in GCC 4.8.4 (TG's version)!
        if (__has_trivial_copy(value_type))  // GCC extension, not part of C++11 standard
            std::memcpy(pDest, pSrcStart, (pSrcEnd - pSrcStart) * sizeof(value_type));
        else {
            pointer pCurDest = pDest;
            for (const_pointer pCurSrc = pSrcStart; pCurSrc < pSrcEnd; ++pCurSrc, ++pCurDest)
                (void) new (pCurDest) T(*pCurSrc);
        }
    }
    
    static void destroyElements(pointer pData, const_pointer pEnd) {
        for (pointer pT = pData; pT < pEnd; ++pT)
            pT->~T();
    }
    
    void freeArray(pointer pData, const_pointer pEnd) {
        destroyElements(pData, pEnd);
        freeMem(pData);
    }
    
    void reset() {
        freeArray(data_, data_ + size_);
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }
    
    void copyIn(const XVector &other) {
        clear();
        reserve(other.size());
        if (other.size() > 0)
            copyToRawMem(other.data(), other.data() + other.size(), data_);
        size_ = other.size();
    }
    
    void steal(XVector &&other) {
        data_ = other.data_;
        capacity_ = other.capacity_;
        size_ = other.size_;
        other.data_ = nullptr;
        other.capacity_ = other.size_ = 0;
    }
    
    void expand() {
        if (size_ == capacity_) {
            size_type newCapacity = (capacity_ << 1);
            if (newCapacity == 0)
                newCapacity = 1;
            reserve(newCapacity);
        }
    }
};

}  // namespace xilinx_apps

// For legacy use
using XString = xilinx_apps::XString;

#endif
