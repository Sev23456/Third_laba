#pragma once
#include <memory>
#include "IEnumerator.h"

template<typename T>
class IEnumerable {
public:
    virtual ~IEnumerable() = default;
    virtual std::unique_ptr<IEnumerator<T>> GetEnumerator() const = 0;
};
