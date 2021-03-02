#ifndef STATICVECTOR_H
#define STATICVECTOR_H

#include <array>
#include <cassert>
#include <initializer_list>

template<typename T, unsigned int max_size>
class static_vector {
public:
    constexpr static_vector() : data_(), size_(0) {}

    static_vector(const std::initializer_list<T>& list) : data_(), size_(0) {
        for(const auto& t : list) push_back(t);
    }

    constexpr unsigned int size() const noexcept { return size_; }
    constexpr void push_back(const T& t) noexcept {
        assert(size_ < max_size);
        data_[size_] = t;
        ++size_;
    }
    constexpr bool empty() const noexcept { return size_ == 0; }

    constexpr const T* begin() const noexcept { return data_.data(); }
    constexpr const T* end() const noexcept { return data_.data() + size_; }

private:
    std::array<T, max_size> data_;
    unsigned int size_;
};

#endif