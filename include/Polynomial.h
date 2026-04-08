#pragma once

#include "AlgebraUtils.h"
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <vector>

template<typename T>
class Polynomial {
private:
    std::unique_ptr<Sequence<T>> coefficients;

public:
    Polynomial() : coefficients(algebra_detail::MakeSequence<T>(1, T())) {}

    Polynomial(std::initializer_list<T> values)
        : coefficients(algebra_detail::MakeSequence<T>(std::vector<T>(values))) {
        Normalize();
    }

    explicit Polynomial(const Sequence<T>& source)
        : coefficients(algebra_detail::CloneSequence(source)) {
        Normalize();
    }

    Polynomial(const Polynomial<T>& other)
        : coefficients(algebra_detail::CloneSequence(*other.coefficients)) {}

    Polynomial(Polynomial<T>&&) noexcept = default;

    Polynomial<T>& operator=(const Polynomial<T>& other) {
        if (this != &other) {
            coefficients = algebra_detail::CloneSequence(*other.coefficients);
        }
        return *this;
    }

    Polynomial<T>& operator=(Polynomial<T>&&) noexcept = default;

    int Degree() const {
        return coefficients->GetLength() - 1;
    }

    T GetCoefficient(int power) const {
        if (power < 0) {
            throw std::invalid_argument("Power must be non-negative");
        }
        if (power >= coefficients->GetLength()) {
            return T();
        }
        return coefficients->Get(power);
    }

    Polynomial<T> Add(const Polynomial<T>& other) const {
        const int maxDegree = Degree() > other.Degree() ? Degree() : other.Degree();
        std::vector<T> resultData(maxDegree + 1, T());

        for (int power = 0; power <= maxDegree; power++) {
            resultData[power] = GetCoefficient(power) + other.GetCoefficient(power);
        }
        return Polynomial<T>(resultData);
    }

    Polynomial<T> MultiplyByScalar(const T& scalar) const {
        std::vector<T> resultData(coefficients->GetLength(), T());
        for (int i = 0; i < coefficients->GetLength(); i++) {
            resultData[i] = coefficients->Get(i) * scalar;
        }
        return Polynomial<T>(resultData);
    }

    Polynomial<T> Multiply(const Polynomial<T>& other) const {
        std::vector<T> resultData(Degree() + other.Degree() + 1, T());

        for (int left = 0; left <= Degree(); left++) {
            for (int right = 0; right <= other.Degree(); right++) {
                resultData[left + right] =
                    resultData[left + right] + GetCoefficient(left) * other.GetCoefficient(right);
            }
        }
        return Polynomial<T>(resultData);
    }

    T Evaluate(const T& x) const {
        T result = T();
        for (int power = Degree(); power >= 0; power--) {
            result = result * x + coefficients->Get(power);
        }
        return result;
    }

    Polynomial<T> Compose(const Polynomial<T>& inner) const {
        Polynomial<T> result({T()});
        Polynomial<T> currentPower({static_cast<T>(1)});

        for (int power = 0; power <= Degree(); power++) {
            Polynomial<T> term = currentPower.MultiplyByScalar(GetCoefficient(power));
            result = result.Add(term);
            currentPower = currentPower.Multiply(inner);
        }
        return result;
    }

    Polynomial<T> operator+(const Polynomial<T>& other) const {
        return Add(other);
    }

    Polynomial<T> operator*(const Polynomial<T>& other) const {
        return Multiply(other);
    }

    bool operator==(const Polynomial<T>& other) const {
        if (Degree() != other.Degree()) {
            return false;
        }
        for (int i = 0; i <= Degree(); i++) {
            if (!(GetCoefficient(i) == other.GetCoefficient(i))) {
                return false;
            }
        }
        return true;
    }

    std::string ToString() const {
        std::ostringstream out;
        bool first = true;

        for (int power = Degree(); power >= 0; power--) {
            const T coefficient = GetCoefficient(power);
            if (algebra_detail::IsZeroValue(coefficient)) {
                continue;
            }

            if (!first) {
                out << " + ";
            }

            out << coefficient;
            if (power >= 1) {
                out << "*x";
            }
            if (power >= 2) {
                out << "^" << power;
            }

            first = false;
        }

        if (first) {
            out << T();
        }
        return out.str();
    }

    void Print(std::ostream& out = std::cout) const {
        out << ToString();
    }

private:
    explicit Polynomial(const std::vector<T>& values)
        : coefficients(algebra_detail::MakeSequence<T>(values)) {
        Normalize();
    }

    void Normalize() {
        std::vector<T> normalized;
        normalized.reserve(coefficients->GetLength());

        for (int i = 0; i < coefficients->GetLength(); i++) {
            normalized.push_back(coefficients->Get(i));
        }

        while (normalized.size() > 1 && algebra_detail::IsZeroValue(normalized.back())) {
            normalized.pop_back();
        }

        if (normalized.empty()) {
            normalized.push_back(T());
        }

        coefficients = algebra_detail::MakeSequence<T>(normalized);
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const Polynomial<T>& polynomial) {
    polynomial.Print(out);
    return out;
}
