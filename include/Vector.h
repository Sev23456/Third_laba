#pragma once

#include "AlgebraUtils.h"
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <vector>

template<typename T>
class Vector {
private:
    std::unique_ptr<Sequence<T>> coordinates;

public:
    Vector() : coordinates(algebra_detail::MakeSequence<T>()) {}

    explicit Vector(int size, const T& initialValue = T())
        : coordinates(algebra_detail::MakeSequence<T>(size, initialValue)) {}

    Vector(std::initializer_list<T> values)
        : coordinates(algebra_detail::MakeSequence<T>(std::vector<T>(values))) {}

    explicit Vector(const Sequence<T>& source)
        : coordinates(algebra_detail::CloneSequence(source)) {}

    Vector(const Vector<T>& other)
        : coordinates(algebra_detail::CloneSequence(*other.coordinates)) {}

    Vector(Vector<T>&&) noexcept = default;

    Vector<T>& operator=(const Vector<T>& other) {
        if (this != &other) {
            coordinates = algebra_detail::CloneSequence(*other.coordinates);
        }
        return *this;
    }

    Vector<T>& operator=(Vector<T>&&) noexcept = default;

    int Size() const {
        return coordinates->GetLength();
    }

    T Get(int index) const {
        return coordinates->Get(index);
    }

    void Set(int index, const T& value) {
        coordinates = algebra_detail::ReplaceAt(*coordinates, index, value);
    }

    const Sequence<T>& AsSequence() const {
        return *coordinates;
    }

    Vector<T> Add(const Vector<T>& other) const {
        EnsureSameSize(other);

        std::vector<T> resultData;
        resultData.reserve(Size());
        for (int i = 0; i < Size(); i++) {
            resultData.push_back(Get(i) + other.Get(i));
        }
        return Vector<T>(resultData);
    }

    Vector<T> MultiplyByScalar(const T& scalar) const {
        std::vector<T> resultData;
        resultData.reserve(Size());
        for (int i = 0; i < Size(); i++) {
            resultData.push_back(Get(i) * scalar);
        }
        return Vector<T>(resultData);
    }

    T Dot(const Vector<T>& other) const {
        EnsureSameSize(other);

        T result = T();
        for (int i = 0; i < Size(); i++) {
            result = result + Get(i) * other.Get(i);
        }
        return result;
    }

    double Norm() const {
        long double sum = 0.0L;
        for (int i = 0; i < Size(); i++) {
            sum += algebra_detail::SquaredMagnitude(Get(i));
        }
        return std::sqrt(static_cast<double>(sum));
    }

    Vector<T> operator+(const Vector<T>& other) const {
        return Add(other);
    }

    Vector<T> operator*(const T& scalar) const {
        return MultiplyByScalar(scalar);
    }

    bool operator==(const Vector<T>& other) const {
        if (Size() != other.Size()) {
            return false;
        }
        for (int i = 0; i < Size(); i++) {
            if (!(Get(i) == other.Get(i))) {
                return false;
            }
        }
        return true;
    }

    void Print(std::ostream& out = std::cout) const {
        out << "[";
        for (int i = 0; i < Size(); i++) {
            out << Get(i);
            if (i + 1 < Size()) {
                out << ", ";
            }
        }
        out << "]";
    }

private:
    explicit Vector(const std::vector<T>& values)
        : coordinates(algebra_detail::MakeSequence<T>(values)) {}

    void EnsureSameSize(const Vector<T>& other) const {
        if (Size() != other.Size()) {
            throw std::invalid_argument("Vectors must have the same size");
        }
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const Vector<T>& vector) {
    vector.Print(out);
    return out;
}
