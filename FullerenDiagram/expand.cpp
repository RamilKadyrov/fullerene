#include <algorithm>
#include <vector>
#include <iostream>
#include <random>
#include <limits>

#include "fullereneTypes.h" 
#include "paintFullerene.h"
#include "expandFullerene.h" 

inline double sqr(double x)
{
    return x * x;
}

Result Expand::expand(std::vector<Vertex>& graph, const double rm, const size_t nSide)
{
    std::vector<double> f, v; 
    double residue, residueOld;
    bool quit;
    
    if ((nSide != 5) && (nSide != 6))
    {
        LOG_ERROR( L"Last face contain incorrect number of edges:" << nSide );
        return Result::FAIL;
    }

    const size_t graphSize = graph.size();
    const size_t sizeN = graphSize - nSide;
    const size_t sizeXYN = sizeN * 2;
    const size_t mtrxSize = 4 * graphSize * graphSize;

    sizeXY = 2 * graphSize;

    wp.resize(mtrxSize);
    wp1.resize(mtrxSize);
    wm.resize(mtrxSize);
    f.resize(sizeXY);
    v.reserve(sizeXY);

    for (size_t i = 0; i < graphSize; ++i)
    {
        v.push_back(static_cast<double>(graph[i].x));
        v.push_back(static_cast<double>(graph[i].y));
    }
 
    quit = false;
    for (size_t l = 0; l < enlarge * iter && !quit; ++l)
    {
        for (size_t i = 0; i < nSide; ++i)
        {
            auto dv = static_cast<double>(enlarge * iter - l) / static_cast<double>(enlarge * iter + 1 - l);
            auto i2 = i * 2;
            v[sizeXYN + i2] = (v[sizeXYN + i2] - rm) * dv + rm;
            v[sizeXYN + i2 + 1] = (v[sizeXYN + i2 + 1] - rm) * dv + rm;
        }
        int k = 5;
        residueOld = std::numeric_limits<double>::max();
        LOG( L"Expanding the graph..." );
        do {
            bool overlap = true;
            while (overlap)
            {
                overlap = false;
                for (size_t i = 0; i < sizeN && !overlap; ++i)
                {
                    f[i * 2] = 0;
                    f[i * 2 + 1] = 0;
                    for (size_t edge = 0; edge < 3 && !overlap; ++edge)
                    {
                        auto j = graph[i].e[edge] * 2;
                        auto res = sqrt(sqr(v[j] - v[i * 2]) + sqr(v[j + 1] - v[i * 2 + 1]));
                        auto fL = res - L0;
                        if (res != 0)
                        {
                            f[i * 2] += fL * (v[j] - v[i * 2]) / res;
                            f[i * 2 + 1] += fL * (v[j + 1] - v[i * 2 + 1]) / res;
                        }
                        else
                        {
                            overlap = true;
                            auto rnd = (double)rand() / RAND_MAX - 0.5;
                            v[i * 2] = v[i * 2] + rnd;
                            v[i * 2] = v[i * 2 + 1] - rnd;
                        }
                    }

                }
            }
            std::fill(wp.begin(), wp.end(), 0.0);
            //derivative matrix calculation
            for (size_t i = 0; i < sizeN; ++i)
            {
                auto i2 = i * 2;
                auto ia2 = i2 * sizeXY;
                for (size_t edge = 0; edge < 3; ++edge)
                {
                    auto j = graph[i].e[edge] * 2;
                    auto fL = sqr(v[j] - v[i2]) + sqr(v[j + 1] - v[i2 + 1]);
                    auto res = L0 / (sqrt(fL) * fL);
                    auto fX = sqr(v[j + 1] - v[i2 + 1]) * res - 1.0;
                    auto fY = sqr(v[j] - v[i2]) * res - 1.0;
                    auto fXY = (v[j + 1] - v[i2 + 1]) * (v[i2] - v[j]) * res;
                    
                    wp[ia2 + i2] += fX;
                    wp[ia2 + sizeXY + i2 + 1] += fY;
                    wp[ia2 + i2 + 1] += fXY;
                    wp[ia2 + sizeXY + i2] += fXY;
                    // xi
                    wp[ia2 + j] = -fX;
                    wp[ia2 + sizeXY + j] = -fXY;
                    // yi
                    wp[ia2 + sizeXY + j + 1] = -fY;
                    wp[ia2 + j + 1] = -fXY;
                }
            }
            //inverse matrix
            if (k > 3)
            {
                LOG( L"Inverse matrix calculation. Size: " << sizeXYN << " x " << sizeXYN);
                inverseMatrixOp(sizeXYN - 1);
                k = 0;
            }
            ++k;
            for (size_t i = 0; i < sizeXYN; ++i)
            {
                auto ia = sizeXY * i;
                for (size_t j = 0; j < sizeXYN; ++j)
                {
                    v[i] -= wp1[ia + j] * f[j];
                }
            }

            residue = 0.0;
            for (size_t i = 0; i < sizeXYN; ++i)
            {
                residue += abs(f[i]);
            }
            if (residue > residueOld)
            {
                residueOld = std::numeric_limits<double>::max();
                k = 5;
            }
            else
            {
                residueOld = residue;
            }
            LOG( L"Residue = " << residue );
        } while (residue > epsilon);
        LOG( L"Iteration number: "<< l );
        quit = true;
        for (size_t i = 0; i < sizeN; ++i)
        {
            if (sqr(v[i * 2] - rm) + sqr(v[i * 2 + 1] - rm) >= sqr(rm * 0.9))
            {
                quit = false;
                break;
            }
        }

    }
    
    for (size_t i = 0; i < graphSize; ++i)
    {
        graph[i].x = static_cast<int>(v[i * 2]);
        graph[i].y = static_cast<int>(v[i * 2 + 1]);
    }
    LOG( L"The graph is expanded" );
    return Result::OK;
}

void Expand::dive(size_t n, bool p)
{
    if (1 == n)
    {
        auto det1 = 1.0 / (wp[0] * wp[sizeXY + 1] - wp[1] * wp[sizeXY]);
        if (p)
        {
            wp1[0]       = wp[sizeXY + 1] * det1;
            wp1[sizeXY + 1] = wp[0] * det1;
            wp1[sizeXY]     = -wp[1] * det1;
            wp1[1]       = -wp[sizeXY] * det1;
        }
        else
        {
            wm[0]       = wp[sizeXY + 1] * det1;
            wm[sizeXY + 1] = wp[0] * det1;
            wm[sizeXY]     = -wp[1] * det1;
            wm[1]       = -wp[sizeXY] * det1;
        }
    }
    else
    {
        //const size_t n_1 = n - 1;
        dive(n - 1, !p);
        auto an = sizeXY * n;
        if (p)
            //итог в wp1----------------------------
        {
            //вычисляем  - w * A_1
            for (size_t i = 0; i < n; ++i)
            {
                auto tEx = 0.0;
                for (size_t j = 0; j < n; ++j)
                {
                    tEx -= wp[an + j] * wm[sizeXY * j + i];
                }
                wp1[an + i] = tEx;
            }
            //beta
            auto det = wp[an + n];
            for (size_t j = 0; j < n; ++j)
            {
                det += wp1[an + j] * wp[sizeXY * j + n];
            }
            auto det1 = 1.0 / det;
            wp1[an + n] = det1;

            // - beta * A_1 * v
            for (size_t i = 0; i < n; ++i)
            {
                auto tEx = 0.0;
                auto ai = sizeXY * i;
                for (size_t j = 0; j < n; ++j)
                {
                    tEx = tEx - wm[ai + j] * wp[sizeXY *j + n];
                }
                tEx *= det1;
                wp1[ai + n] = tEx;
                // A_1 + beta * A_1 * v * w * A_1
                for(size_t j = 0; j < n; ++j)
                {
                    wp1[ai + j] = wm[ai + j] + wp1[an + j] * tEx;
                }
            }
            //вычисляем  - beta * w * A_1
            for (size_t i = 0; i < n; ++i)
            {
                wp1[an + i] *= det1;
            }
        }
        //итог в wm----------------------------
        else
        {
            //вычисляем  - w * A_1
            for (size_t i = 0; i < n; ++i)
            {
                auto tEx = 0.0;
                for (size_t j = 0; j < n; ++j)
                {
                    tEx -= wp[an + j] * wp1[sizeXY * j + i];
                }
                wm[an + i] = tEx;
            }
            //beta
            auto det = wp[an + n];
            for (size_t j = 0; j < n; ++j)
            {
                det += wm[an + j] * wp[sizeXY * j + n];
            }
            auto det1 = 1.0 / det;
            wm[an + n] = det1;
            // - beta * A_1 * v
            for (size_t i = 0; i < n; ++i)
            {
                auto tEx = 0.0;
                auto ai = sizeXY * i;
                for (size_t j = 0; j < n; ++j)
                {
                    tEx -= wp1[ai + j] * wp[sizeXY * j + n];
                }
                tEx *= det1;
                wm[ai + n] = tEx;
                // A_1 + beta * A_1 * v * w * A_1
                for (size_t j = 0; j < n; ++j)
                {
                    wm[ai + j] = wp1[ai + j] + wm[an + j] * tEx;
                }
            }
            //вычисляем  - beta * w * A_1
            for (size_t i = 0; i < n; ++i)
            {
                wm[an + i] *= det1;
            }
        }
    }
    //Log('Вынырнули с ' + IntToStr(n) + 'уровня');
}

void Expand::inverseMatrixOp(size_t n)
{
    dive(n, true);
}
