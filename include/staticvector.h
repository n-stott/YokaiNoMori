#ifndef STATICVECTOR_H
#define STATICVECTOR_H

#include <array>
#include <cassert>
#include <initializer_list>
#include <cstddef>

template<typename T, unsigned int max_size>
class static_vector {
public:

    using const_iterator = const T*;
    static constexpr unsigned int capacity = max_size;

    constexpr static_vector() : data_(), size_(0) {}

    constexpr static_vector(const std::initializer_list<T>& list) : data_(), size_(0) {
        for(const auto& t : list) push_back(t);
    }

    constexpr unsigned int size() const noexcept { return size_; }

    constexpr void push_back(const T& t) noexcept {
        assert(size_ < max_size);
        data_[size_] = t;
        ++size_;
    }

    template<typename... Args>
    constexpr void emplace_back(Args... args) noexcept {
        assert(size_ < max_size);
        data_[size_] = T(args...);
        ++size_;
    }

    constexpr bool empty() const noexcept { return size_ == 0; }

    constexpr void clear() noexcept { size_ = 0; }

    constexpr void reserve(size_t size) const noexcept {
        assert(size <= max_size);
    }


    constexpr const T* begin() const noexcept { return data_.data(); }
    constexpr const T* end() const noexcept { return begin() + size_; }

    constexpr T* begin() noexcept { return data_.data(); }
    constexpr T* end() noexcept { return begin() + size_; }

    constexpr const T& operator[](unsigned int i) const { return data_[i]; }
    constexpr T& operator[](unsigned int i) { return data_[i]; }

    constexpr bool operator==(const static_vector<T, max_size>& other) const noexcept {
        if(size() != other.size()) return false;
        for(size_t i = 0; i < size(); ++i) {
            if(!(data_[i] == other[i])) return false;
        }
        return true;
    }

private:
    std::array<T, max_size> data_;
    unsigned int size_;
};

#endif