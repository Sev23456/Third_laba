#pragma once

#include "ArraySequence.h"
#include <complex>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace algebra_detail {

template<typename T>
std::unique_ptr<Sequence<T>> MakeSequence() {
    return std::unique_ptr<Sequence<T>>(new ArraySequence<T>());
}

template<typename T>
std::unique_ptr<Sequence<T>> MakeSequence(int count, const T& value = T()) {
    if (count < 0) {
        throw std::invalid_argument("Count must be non-negative");
    }

    std::unique_ptr<Sequence<T>> result = MakeSequence<T>();
    for (int i = 0; i < count; i++) {
        result->Append(value);
    }
    return result;
}

template<typename T>
std::unique_ptr<Sequence<T>> MakeSequence(const std::vector<T>& values) {
    std::unique_ptr<Sequence<T>> result = MakeSequence<T>();
    for (const T& value : values) {
        result->Append(value);
    }
    return result;
}

template<typename T>
std::unique_ptr<Sequence<T>> CloneSequence(const Sequence<T>& source) {
    std::unique_ptr<Sequence<T>> result = MakeSequence<T>();
    for (int i = 0; i < source.GetLength(); i++) {
        result->Append(source.Get(i));
    }
    return result;
}

template<typename T>
std::unique_ptr<Sequence<T>> ReplaceAt(const Sequence<T>& source, int index, const T& value) {
    if (index < 0 || index >= source.GetLength()) {
        throw IndexOutOfRangeException(index, source.GetLength());
    }

    std::unique_ptr<Sequence<T>> result = MakeSequence<T>();
    for (int i = 0; i < source.GetLength(); i++) {
        result->Append(i == index ? value : source.Get(i));
    }
    return result;
}

template<typename T>
long double SquaredMagnitude(const T& value) {
    if constexpr (std::is_arithmetic_v<T>) {
        long double converted = static_cast<long double>(value);
        return converted * converted;
    } else {
        return static_cast<long double>(std::norm(value));
    }
}

template<typename T>
bool IsZeroValue(const T& value) {
    return value == T();
}

}  // namespace algebra_detail
