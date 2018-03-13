#include "inverseMatrix.h"

Matrix::Matrix(size_t _size) :size(_size) {
    auto sq = size * size;
    m.resize(sq);
    m_1.resize(sq);
    mt.resize(sq);
}

void Matrix::fillZero()
{
    std::fill(m.begin(), m.end(), 0.0);
}

void Matrix::dive(size_t n, bool p)
{
    if (1 == n)
    {
        auto det1 = 1.0 / (m[0] * m[size + 1] - m[1] * m[size]);
        if (p)
        {
            m_1[0] = m[size + 1] * det1;
            m_1[size + 1] = m[0] * det1;
            m_1[size] = -m[1] * det1;
            m_1[1] = -m[size] * det1;
        }
        else
        {
            mt[0] = m[size + 1] * det1;
            mt[size + 1] = m[0] * det1;
            mt[size] = -m[1] * det1;
            mt[1] = -m[size] * det1;
        }
    }
    else
    {
        dive(n - 1, !p);
        auto an = size * n;
        if (p)
            //result in m_1
        {
            //calc  - w * A_1
            for (size_t i = 0; i < n; ++i)
            {
                auto tEx = 0.0;
                for (size_t j = 0; j < n; ++j)
                {
                    tEx -= m[an + j] * mt[size * j + i];
                }
                m_1[an + i] = tEx;
            }
            //beta
            auto det = m[an + n];
            for (size_t j = 0; j < n; ++j)
            {
                det += m_1[an + j] * m[size * j + n];
            }
            auto det1 = 1.0 / det;
            m_1[an + n] = det1;

            // - beta * A_1 * v
            for (size_t i = 0; i < n; ++i)
            {
                auto tEx = 0.0;
                auto ai = size * i;
                for (size_t j = 0; j < n; ++j)
                {
                    tEx = tEx - mt[ai + j] * m[size *j + n];
                }
                tEx *= det1;
                m_1[ai + n] = tEx;
                // A_1 + beta * A_1 * v * w * A_1
                for (size_t j = 0; j < n; ++j)
                {
                    m_1[ai + j] = mt[ai + j] + m_1[an + j] * tEx;
                }
            }
            //calc  - beta * w * A_1
            for (size_t i = 0; i < n; ++i)
            {
                m_1[an + i] *= det1;
            }
        }
        //result in mt
        else
        {
            //calc  - w * A_1
            for (size_t i = 0; i < n; ++i)
            {
                auto tEx = 0.0;
                for (size_t j = 0; j < n; ++j)
                {
                    tEx -= m[an + j] * m_1[size * j + i];
                }
                mt[an + i] = tEx;
            }
            //beta
            auto det = m[an + n];
            for (size_t j = 0; j < n; ++j)
            {
                det += mt[an + j] * m[size * j + n];
            }
            auto det1 = 1.0 / det;
            mt[an + n] = det1;
            // - beta * A_1 * v
            for (size_t i = 0; i < n; ++i)
            {
                auto tEx = 0.0;
                auto ai = size * i;
                for (size_t j = 0; j < n; ++j)
                {
                    tEx -= m_1[ai + j] * m[size * j + n];
                }
                tEx *= det1;
                mt[ai + n] = tEx;
                // A_1 + beta * A_1 * v * w * A_1
                for (size_t j = 0; j < n; ++j)
                {
                    mt[ai + j] = m_1[ai + j] + mt[an + j] * tEx;
                }
            }
            //calc  - beta * w * A_1
            for (size_t i = 0; i < n; ++i)
            {
                mt[an + i] *= det1;
            }
        }
    }
}

void Matrix::inverse(size_t n)
{
    dive(n - 1, true);
}
