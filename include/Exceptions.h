#pragma once
#include <exception>
#include <string>

class IndexOutOfRangeException : public std::exception {
private:
    std::string message;
public:
    IndexOutOfRangeException(int index, int size) {
        message = "Index out of range: " + std::to_string(index) + 
                  " (size: " + std::to_string(size) + ")";
    }
    const char* what() const noexcept override {
        return message.c_str();
    }
};

class EmptySequenceException : public std::exception {
private:
    std::string message = "Sequence is empty";
public:
    const char* what() const noexcept override {
        return message.c_str();
    }
};