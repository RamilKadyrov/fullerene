#pragma once
#include <vector>

class Matrix {
public:
    Matrix() {}
    Matrix(size_t _size);
    size_t size;
    std::vector<double> m, m_1, mt;

    std::vector<double>::iterator operator[](size_t n)
    {
        return m.begin() + n * size;
    }
    std::vector<double>::iterator operator()(size_t n)
    {
        return m_1.begin() + n * size;
    }
    void fillZero();
    void dive(size_t n, bool p);
    void inverse(size_t n);
};
