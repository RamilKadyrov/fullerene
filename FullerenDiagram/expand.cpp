#include <algorithm>
#include <vector>
#include <iostream>
#include <random>
#include <limits>

#include "fullereneTypes.h" 
#include "paintFullerene.h"
#include "expandFullerene.h" 
#include "inverseMatrix.h"

inline double sqr(double x)
{
    return x * x;
}

Result expand(std::vector<Vertex>& graph, const double rm, const size_t nSide)
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

    size_t sizeXY = 2 * graphSize;

    Matrix d(sizeXY);

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
                for (size_t i2 = 0, i = 0; i < sizeN && !overlap;  ++i, i2 += 2 )
                {
                    f[i2] = 0;
                    f[i2 + 1] = 0;
                    for (size_t edge = 0; edge < 3 && !overlap; ++edge)
                    {
                        auto j = graph[i].e[edge] * 2;
                        auto res = sqrt(sqr(v[j] - v[i2]) + sqr(v[j + 1] - v[i2 + 1]));
                        auto fL = res - L0;
                        if (res != 0)
                        {
                            f[i2] += fL * (v[j] - v[i2]) / res;
                            f[i2 + 1] += fL * (v[j + 1] - v[i2 + 1]) / res;
                        }
                        else
                        {
                            overlap = true;
                            auto rnd = (double)rand() / RAND_MAX - 0.5;
                            v[i2] +=  rnd;
                            v[i2 + 1] -= rnd;
                        }
                    }

                }
            }
            d.fillZero();
            //derivative matrix calculation
            for (size_t i2 = 0, i = 0; i < sizeN;  ++i, i2 += 2)
            {
                for (size_t edge = 0; edge < 3; ++edge)
                {
                    auto j = graph[i].e[edge] * 2;
                    auto fL = sqr(v[j] - v[i2]) + sqr(v[j + 1] - v[i2 + 1]);
                    auto res = L0 / (sqrt(fL) * fL);
                    auto fX = sqr(v[j + 1] - v[i2 + 1]) * res - 1.0;
                    auto fY = sqr(v[j] - v[i2]) * res - 1.0;
                    auto fXY = (v[j + 1] - v[i2 + 1]) * (v[i2] - v[j]) * res;
                    
                    d[i2][i2] += fX;
                    d[i2 + 1][ i2 + 1] += fY;
                    d[i2][i2 + 1] += fXY;
                    d[i2 + 1][i2] += fXY;
                    // xi
                    d[i2][j] = -fX;
                    d[i2 + 1][j] = -fXY;
                    // yi
                    d[i2 + 1][j + 1] = -fY;
                    d[i2][j + 1] = -fXY;
                }
            }
            //inverse matrix
            if (k > 3)
            {
                LOG( L"Inverse matrix calculation. Size: " << sizeXYN << " x " << sizeXYN);
                d.inverse(sizeXYN);
                k = 0;
            }
            ++k;
            for (size_t i = 0; i < sizeXYN; ++i)
            {
                for (size_t j = 0; j < sizeXYN; ++j)
                {
                    v[i] -= d(i)[j] * f[j];
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
        for (size_t i = 0; i < 2 * sizeN; i += 2)
        {
            if (sqr(v[i] - rm) + sqr(v[i + 1] - rm) >= sqr(rm * 0.9))
            {
                quit = false;
                break;
            }
        }
    }
    for (size_t i = 0, i2 = 0; i < graphSize; ++i, i2 += 2)
    {
        graph[i].x = static_cast<int>(v[i2]);
        graph[i].y = static_cast<int>(v[i2 + 1]);
    }
    LOG( L"The graph is expanded" );
    return Result::OK;
}
