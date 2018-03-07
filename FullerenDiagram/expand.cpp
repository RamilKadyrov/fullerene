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

double mulMatrix()
{
    /*double norm = 0.0;
    for (size_t i = 0; i < sizeXYN; ++i)
    {
    auto ai = sizeXY * i;
    for (size_t j = 0; j < sizeXYN; ++j)
    {
    wm[ai + j] = 0.0;
    for (size_t i1 = 0; i1 < sizeXYN; ++i1)
    {
    wm[ai + j] += wp[ai + i1] * wp1[sizeXY * i1 + j];
    }
    norm += abs(wm[ai + j]);
    }
    }*/
    return 0.0;
}

Result Expand::expand(std::vector<Vertex>& graph, const double rm, const size_t nSide)
{
    std::vector<double> f, v; 
    double residue, residueOld;
    bool quit;
    
    if ((nSide != 5) && (nSide != 6))
    {
        std::wcerr << L"Last face contain incorrect number of edges:" << nSide << std::endl;
        return Result::FAIL;
    }

    const size_t graphSize = graph.size();
    size_t sizeN = graphSize - nSide;
    size_t sizeXYN = sizeN * 2;
    sizeXY = 2 * graphSize;
    const size_t mtrxSize = 4 * graphSize * graphSize;
    wp.resize(mtrxSize);
    wp1.resize(mtrxSize);
    wm.resize(mtrxSize);
    f.resize(sizeXY);
    v.reserve(sizeXY);
    for (size_t i = 0; i < graphSize; ++i)
    {
        v.push_back(static_cast<double>(graph[i].e[3]));
        v.push_back(static_cast<double>(graph[i].e[4]));
    }
       
    //�������� ����
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
        int k = 25;
        residueOld = std::numeric_limits<double>::max();
        std::wcout << L"������������ ����..." << std::endl;
        do {
            bool overlap = true;
            while (overlap)
            {
                overlap = false;
                for (size_t i = 0; i < sizeN && !overlap; ++i)
                {
                    //������� �������
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
                            //std::wcerr << L"��������� ������ �������. ����� ��������� �����, ��������, �������." << std::endl;
                            //return Result::FAIL;
                            overlap = true;
                            auto rnd = (double)rand() / RAND_MAX - 0.5;
                            v[i * 2] = v[i * 2] + rnd;
                            v[i * 2] = v[i * 2 + 1] - rnd;
                        }
                    }

                }
            }
            std::fill(wp.begin(), wp.end(), 0.0);
            //������� ������� �����������
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
                    //����� ����������� �� xi
                    wp[ia2 + j] = -fX;
                    wp[ia2 + sizeXY + j] = -fXY;
                    //����� ����������� �� yi
                    wp[ia2 + sizeXY + j + 1] = -fY;
                    wp[ia2 + j + 1] = -fXY;
                }
            }
            //�������� �������
            if (k > 10)
            {
                std::wcout << L"������� ������ �������� ������� �� " << sizeXYN << L" �����" << std::endl;
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
            /*double norm = mulMatrix();
            std::wcout << L"Test inverse matrix = " << norm / (double)sizeXYN << std::endl;*/
            residue = 0.0;
            for (size_t i = 0; i < sizeXYN; ++i)
            {
                residue += abs(f[i]);
            }
            if (residue > residueOld)
            {
                residueOld = std::numeric_limits<double>::max();
                k = 25;
            }
            else
            {
                residueOld = residue;
            }
            std::wcout << L"��������� = " << residue << std::endl;
        } while (residue > epsilon);
        std::wcout << L"Iteration number: "<< l << std::endl;
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
        graph[i].e[3] = static_cast<int>(v[i * 2]);
        graph[i].e[4] = static_cast<int>(v[i * 2 + 1]);
    }
    std::wcout << L"���� ��������." << std::endl;
    return Result::OK;
}

void Expand::dive(size_t n, bool p)
{
    double det, tEx;
    size_t n_1;
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
        n_1 = n - 1;
        dive(n_1, !p);
        auto an = sizeXY * n;
        if (p)
            //���� � wp1----------------------------
        {
            //���������  - w * A_1
            for (size_t i = 0; i <= n_1; ++i)
            {
                tEx = 0.0;
                for (size_t j = 0; j <= n_1; ++j)
                {
                    tEx -= wp[an + j] * wm[sizeXY * j + i];
                }
                wp1[an + i] = tEx;
            }
            //beta
            det = wp[an + n];
            for (size_t j = 0; j <= n_1; ++j)
            {
                det += wp1[an + j] * wp[sizeXY * j + n];
            }
            auto det1 = 1.0 / det;
            wp1[an + n] = det1;

            // - beta * A_1 * v
            for (size_t i = 0; i <= n_1; ++i)
            {
                tEx = 0;
                auto ai = sizeXY * i;
                for (size_t j = 0; j <= n_1; ++j)
                {
                    tEx = tEx - wm[ai + j] * wp[sizeXY *j + n];
                }
                tEx *= det1;
                wp1[ai + n] = tEx;
                // A_1 + beta * A_1 * v * w * A_1
                for(size_t j = 0; j <= n_1; ++j)
                {
                    wp1[ai + j] = wm[ai + j] + wp1[an + j] * tEx;
                }
            }
            //���������  - beta * w * A_1
            for (size_t i = 0; i <= n_1; ++i)
            {
                wp1[an + i] *= det1;
            }
        }
        //���� � wm----------------------------
        else
        {
            //���������  - w * A_1
            for (size_t i = 0; i <= n_1; ++i)
            {
                tEx = 0.0;
                for (size_t j = 0; j <= n_1; ++j)
                {
                    tEx -= wp[an + j] * wp1[sizeXY * j + i];
                }
                wm[an + i] = tEx;
            }
            //beta
            det = wp[an + n];
            for (size_t j = 0; j <= n_1; ++j)
            {
                det += wm[an + j] * wp[sizeXY * j + n];
            }
            auto det1 = 1.0 / det;
            wm[an + n] = det1;
            // - beta * A_1 * v
            for (size_t i = 0; i <= n_1; ++i)
            {
                tEx = 0.0;
                auto ai = sizeXY * i;
                for (size_t j = 0; j <= n_1; ++j)
                {
                    tEx -= wp1[ai + j] * wp[sizeXY * j + n];
                }
                tEx *= det1;
                wm[ai + n] = tEx;
                // A_1 + beta * A_1 * v * w * A_1
                for (size_t j = 0; j <= n_1; ++j)
                {
                    wm[ai + j] = wp1[ai + j] + wm[an + j] * tEx;
                }
            }
            //���������  - beta * w * A_1
            for (size_t i = 0; i <= n_1; ++i)
            {
                wm[an + i] *= det1;
            }
        }
    }
    //Log('��������� � ' + IntToStr(n) + '������');
}

//--------- �������� ������� - ���������������� ������ -------------------------
void Expand::inverseMatrixOp(size_t n)
{
    dive(n, true);
}
