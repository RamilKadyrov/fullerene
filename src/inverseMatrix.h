#pragma once
#include <vector>

class Matrix {
public:
    Matrix() {}
    Matrix(std::size_t _size);
    std::size_t size;
    std::vector<double> m, m_1, mt;

    std::vector<double>::iterator operator[](std::size_t n)
    {
        return m.begin() + n * size;
    }
    std::vector<double>::iterator operator()(std::size_t n)
    {
        return m_1.begin() + n * size;
    }
    void fillZero();
    void dive(std::size_t n, bool p);
    void inverse(std::size_t n);
};
