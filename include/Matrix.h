#pragma once

#include "AlgebraUtils.h"
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <vector>

template<typename T>
class Matrix {
private:
    int rows;
    int cols;
    std::unique_ptr<Sequence<T>> data;

public:
    Matrix() : rows(0), cols(0), data(algebra_detail::MakeSequence<T>()) {}

    Matrix(int rows, int cols, const T& initialValue = T()) : rows(rows), cols(cols) {
        if (rows < 0 || cols < 0) {
            throw std::invalid_argument("Matrix dimensions must be non-negative");
        }
        data = algebra_detail::MakeSequence<T>(rows * cols, initialValue);
    }

    Matrix(std::initializer_list<std::initializer_list<T>> values) {
        rows = static_cast<int>(values.size());
        cols = rows == 0 ? 0 : static_cast<int>(values.begin()->size());
        data = algebra_detail::MakeSequence<T>();

        for (const auto& row : values) {
            if (static_cast<int>(row.size()) != cols) {
                throw std::invalid_argument("All rows must have the same number of columns");
            }
            for (const T& value : row) {
                data->Append(value);
            }
        }
    }

    Matrix(const Matrix<T>& other)
        : rows(other.rows), cols(other.cols), data(algebra_detail::CloneSequence(*other.data)) {}

    Matrix(Matrix<T>&&) noexcept = default;

    Matrix<T>& operator=(const Matrix<T>& other) {
        if (this != &other) {
            rows = other.rows;
            cols = other.cols;
            data = algebra_detail::CloneSequence(*other.data);
        }
        return *this;
    }

    Matrix<T>& operator=(Matrix<T>&&) noexcept = default;

    int Rows() const {
        return rows;
    }

    int Cols() const {
        return cols;
    }

    bool IsSquare() const {
        return rows == cols;
    }

    T Get(int row, int col) const {
        return data->Get(FlattenIndex(row, col));
    }

    void Set(int row, int col, const T& value) {
        data = algebra_detail::ReplaceAt(*data, FlattenIndex(row, col), value);
    }

    Matrix<T> Add(const Matrix<T>& other) const {
        EnsureSameShape(other);

        std::vector<T> resultData;
        resultData.reserve(rows * cols);
        for (int i = 0; i < rows * cols; i++) {
            resultData.push_back(data->Get(i) + other.data->Get(i));
        }
        return Matrix<T>(rows, cols, resultData);
    }

    Matrix<T> MultiplyByScalar(const T& scalar) const {
        std::vector<T> resultData;
        resultData.reserve(rows * cols);
        for (int i = 0; i < rows * cols; i++) {
            resultData.push_back(data->Get(i) * scalar);
        }
        return Matrix<T>(rows, cols, resultData);
    }

    double Norm() const {
        long double sum = 0.0L;
        for (int i = 0; i < rows * cols; i++) {
            sum += algebra_detail::SquaredMagnitude(data->Get(i));
        }
        return std::sqrt(static_cast<double>(sum));
    }

    void SwapRows(int firstRow, int secondRow) {
        ValidateRow(firstRow);
        ValidateRow(secondRow);

        for (int col = 0; col < cols; col++) {
            T temporary = Get(firstRow, col);
            Set(firstRow, col, Get(secondRow, col));
            Set(secondRow, col, temporary);
        }
    }

    void SwapColumns(int firstCol, int secondCol) {
        ValidateColumn(firstCol);
        ValidateColumn(secondCol);

        for (int row = 0; row < rows; row++) {
            T temporary = Get(row, firstCol);
            Set(row, firstCol, Get(row, secondCol));
            Set(row, secondCol, temporary);
        }
    }

    void MultiplyRow(int row, const T& scalar) {
        ValidateRow(row);
        for (int col = 0; col < cols; col++) {
            Set(row, col, Get(row, col) * scalar);
        }
    }

    void MultiplyColumn(int col, const T& scalar) {
        ValidateColumn(col);
        for (int row = 0; row < rows; row++) {
            Set(row, col, Get(row, col) * scalar);
        }
    }

    void AddRowMultiple(int sourceRow, int targetRow, const T& scalar) {
        ValidateRow(sourceRow);
        ValidateRow(targetRow);
        for (int col = 0; col < cols; col++) {
            Set(targetRow, col, Get(targetRow, col) + Get(sourceRow, col) * scalar);
        }
    }

    void AddColumnMultiple(int sourceCol, int targetCol, const T& scalar) {
        ValidateColumn(sourceCol);
        ValidateColumn(targetCol);
        for (int row = 0; row < rows; row++) {
            Set(row, targetCol, Get(row, targetCol) + Get(row, sourceCol) * scalar);
        }
    }

    Matrix<T> operator+(const Matrix<T>& other) const {
        return Add(other);
    }

    Matrix<T> operator*(const T& scalar) const {
        return MultiplyByScalar(scalar);
    }

    bool operator==(const Matrix<T>& other) const {
        if (rows != other.rows || cols != other.cols) {
            return false;
        }
        for (int i = 0; i < rows * cols; i++) {
            if (!(data->Get(i) == other.data->Get(i))) {
                return false;
            }
        }
        return true;
    }

    void Print(std::ostream& out = std::cout) const {
        out << "[";
        for (int row = 0; row < rows; row++) {
            out << "[";
            for (int col = 0; col < cols; col++) {
                out << Get(row, col);
                if (col + 1 < cols) {
                    out << ", ";
                }
            }
            out << "]";
            if (row + 1 < rows) {
                out << ", ";
            }
        }
        out << "]";
    }

private:
    Matrix(int rows, int cols, const std::vector<T>& values) : rows(rows), cols(cols) {
        if (rows < 0 || cols < 0) {
            throw std::invalid_argument("Matrix dimensions must be non-negative");
        }
        if (static_cast<int>(values.size()) != rows * cols) {
            throw std::invalid_argument("Matrix data size does not match dimensions");
        }
        data = algebra_detail::MakeSequence<T>(values);
    }

    int FlattenIndex(int row, int col) const {
        ValidateRow(row);
        ValidateColumn(col);
        return row * cols + col;
    }

    void ValidateRow(int row) const {
        if (row < 0 || row >= rows) {
            throw IndexOutOfRangeException(row, rows);
        }
    }

    void ValidateColumn(int col) const {
        if (col < 0 || col >= cols) {
            throw IndexOutOfRangeException(col, cols);
        }
    }

    void EnsureSameShape(const Matrix<T>& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrices must have the same shape");
        }
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& matrix) {
    matrix.Print(out);
    return out;
}
