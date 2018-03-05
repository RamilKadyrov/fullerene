#include <algorithm>
#include <vector>
#include <iostream>
#include <random> 

#include "fullereneTypes.h" 
//#include "buildFullerene.h"
#include "expandFullerene.h" 

inline double sqr(double x)
{
    return x * x;
}

Result Expand::expand(std::vector<Vertex>& graph, const double rm, const size_t nSide)
{
    std::vector<double> f, v; 
    double residue, residueOld, fL, fX, fY, fXY;
    bool quit;
    
    if ((nSide != 5) && (nSide != 6))
    {
        std::wcerr << L"Last face contain incorrect number of edges:" << nSide << std::endl;
        return Result::FAIL;
    }
    const size_t graph_size = graph.size();
    size_t countN = graph_size - nSide;
    size_t all = countN * 2;
    wp = new double*[all];
    wp1 = new double*[all];
    wm = new double*[all];
    for (size_t i = 0; i < all; ++i) {
        wp[i] = new double[all];
        wp1[i] = new double[all];
        wm[i] = new double[all];
    }
    for (size_t i = 0; i < graph_size; ++i)
    {
        v.push_back(static_cast<double>(graph[i].e[3]));
        v.push_back(static_cast<double>(graph[i].e[4]));
    }

    //Randomize;
    do {
        quit = true;
        for (size_t i = 0; i < graph_size; ++i)
        {
            for (size_t j = 0; j < graph_size; ++j)
            {
                auto d = sqrt(sqr(v[i * 2] - v[j * 2]) + sqr(v[i * 2 + 1] - v[j * 2 + 1]));
                if ((i != j) && (d < L0))
                {
                    for (size_t i1 = 0; i1 < countN; ++i1)
                    {
                        double rnd = (double)rand() / RAND_MAX;
                        v[i1 * 2] += (1.0 + L0 * 3.0) * rnd;
                        v[i1 * 2 + 1] += (1.0 - L0 * 3.0) * rnd;
                    }
                    quit = false;
                    break;
                }
            }
            if (!quit) break;
        }
        std::wcout << L"Взбивание." << std::endl;
    } while (!quit);
    //основной счет
    for (size_t l = 0; l <= (enlarge - 1) * iter; ++l)
    {
        for (size_t i = 0; i < nSide; ++i)
        {
            double dv = double(enlarge * iter - l) / double(enlarge * iter + 1 - l);
            v[all + i * 2] = (v[all + i * 2] - rm) * dv + rm;
            v[all + i * 2 + 1] = (v[all + i * 2 + 1] - rm) * dv + rm;
        }
        int k = 25;
        residueOld = 1E10;
        std::wcout << L"Растягивание сети..." << std::endl;
        do {
            for (size_t i = 0; i < countN; ++i)
            {
                //подсчет функции
                f.push_back(0.0);
                f.push_back(0.0);
                for (size_t edge = 0; edge < 3; ++edge)
                {
                    auto j = graph[i].e[edge] * 2;
                    residue = sqrt(sqr(v[j] - v[i * 2]) + sqr(v[j + 1] - v[i * 2 + 1]));
                    fL = residue - L0;
                    if (residue != 0)
                    {
                        f[i * 2] += fL * (v[j] - v[i * 2]) / residue;
                        f[i * 2 + 1] += fL * (v[j + 1] - v[i * 2 + 1]) / residue;
                    }
                    else
                    {
                        std::wcerr << L"Положение вершин совпало. Можно запустить снова, возможно, повезет." << std::endl;
                        return Result::FAIL;
                    }
                }
                //обнуление матрицы производных
                for (size_t j = 0; j < countN; ++j)
                {
                    wp[j * 2][i * 2] = 0.0;
                    wp[j * 2][i * 2 + 1] = 0.0;
                    wp[j * 2 + 1][i * 2] = 0.0;
                    wp[j * 2 + 1][i * 2 + 1] = 0.0;
                }
            }
            //подсчет матрицы производных
            for (size_t i = 0; i < countN; ++i)
            {
                for (size_t edge = 0; edge < 3; ++edge)
                {
                    auto j = graph[i].e[edge] * 2;
                    fL = sqr(v[j] - v[i * 2]) + sqr(v[j + 1] - v[i * 2 + 1]);
                    residue = 1 / (sqrt(fL) * fL);
                    fX = L0 * sqr(v[j + 1] - v[i * 2 + 1]) * residue - 1;
                    fY = L0 * sqr(v[j] - v[i * 2]) * residue - 1;
                    fXY = L0 * (v[j + 1] - v[i * 2 + 1]) * (v[i * 2] - v[j]) * residue;
                    wp[i * 2][i * 2] += fX;
                    wp[i * 2 + 1][i * 2 + 1] += fY;
                    wp[i * 2][i * 2 + 1] += fXY;
                    wp[i * 2 + 1][i * 2] += fXY;
                    //далее производная по xi
                    wp[i * 2][j] = -fX;
                    wp[i * 2 + 1][j] = -fXY;
                    //далее производная по yi
                    wp[i * 2 + 1][j + 1] = -fY;
                    wp[i * 2][j + 1] = -fXY;
                }
            }
            //обратная матрица
            if (k > 10)
            {
                //Log('Наберитесь терпения...');
                std::wcout << L"Запущен расчет обратной матрицы из " << all << L" строк" << std::endl;
                negativeMatrixOp(all - 1);
                k = 0;
            }
            ++k;
            for (size_t i = 0; i < all; ++i)
            {
                for (size_t j = 0; j < all; ++j)
                {
                    v[i] = v[i] - wp1[i][j] * f[j];
                }
            }
            residue = 0;
            for (size_t i = 0; i < all; ++i) residue = residue + fabs(f[i]);
            if (residue > residueOld) k = 25;
            residueOld = residue;

            for (size_t i = 0; i < graph_size; ++i)
            {
                graph[i].e[3] = static_cast<int>(v[i * 2]);
                graph[i].e[4] = static_cast<int>(v[i * 2 + 1]);
            }
            std::wcout << L"Результат = " << residue << std::endl;
        } while (residue > epsilon);

        quit = true;
        for (size_t i = 0; i < countN; ++i)
        {
            if (sqr(v[i * 2] - rm) + sqr(v[i * 2 + 1] - rm) >= sqr(rm * 0.9))
            {
                quit = false;
                break;
            }
        }
        if (quit)
        {
            size_t j = l;
            for (size_t i = 0; i < nSide; ++i)
            {
                auto div = 1.0 / sqrt(sqr(v[all + i * 2] - rm) + sqr(v[all + i * 2 + 1] - rm));
                v[all + i * 2] = rm + rm * (v[all + i * 2] - rm) * div;
                v[all + i * 2 + 1] = rm + rm * (v[all + i * 2 + 1] - rm) * div;
            }
            for (size_t i = 0; i < graph_size; ++i)
            {
                graph[i].e[3] = static_cast<int>(v[i * 2]);
                graph[i].e[4] = static_cast<int>(v[i * 2 + 1]);
            }
            break;
        }
    }
    for (size_t i = 0; i < all; ++i) {
        delete[] wm[i];
        delete[] wp1[i];
        delete[] wp[i];
    }
    delete[] wm;
    delete[] wp1;
    delete[] wm;
    std::wcout << L"Граф растянут." << std::endl;
    return Result::OK;
}



void Expand::dive(size_t n, bool p)
{
    double det, tEx;
    size_t n_1;
    if (1 == n)
    {
        auto det1 = 1.0 / (wp[0][0] * wp[1][1] - wp[0][1] * wp[1][0]);
        if (p)
        {
            wp1[0][0] = wp[1][1] * det1;
            wp1[1][1] = wp[0][0] * det1;
            wp1[1][0] = -wp[0][1] * det1;
            wp1[0][1] = -wp[1][0] * det1;
        }
        else
        {
            wm[0][0] = wp[1][1] * det1;
            wm[1][1] = wp[0][0] * det1;
            wm[1][0] = -wp[0][1] * det1;
            wm[0][1] = -wp[1][0] * det1;
        }
    }
    else
    {
        n_1 = n - 1;
        dive(n_1, !p);
        if (p)
            //итог в wp1----------------------------
        {
            //вычисляем  - w * A_1
            for (int i = 0; i <= n_1; ++i)
            {
                tEx = 0.0;
                for (int j = 0; j <= n_1; ++j)
                {
                    tEx -= wp[n][j] * wm[j][i];
                }
                wp1[n][i] = tEx;
            }
            //beta
            det = wp[n][n];
            for (int j = 0; j <= n_1; ++j)
            {
                det += wp1[n][j] * wp[j][n];
            }
            auto det1 = 1.0 / det;
            wp1[n][n] = det1;

            // - beta * A_1 * v
            for (int i = 0; i <= n_1; ++i)
            {
                tEx = 0;
                for (int j = 0; j <= n_1; ++j)
                {
                    tEx = tEx - wm[i][j] * wp[j][n];
                }
                tEx *= det1;
                wp1[i][n] = tEx;

                // A_1 + beta * A_1 * v * w * A_1
                for (int j = 0; j <= n_1; ++j)
                {
                    wp1[i][j] = wm[i][j] + wp1[n][j] * tEx;
                }
            }

            //вычисляем  - beta * w * A_1
            for (int i = 0; i <= n_1; ++i)
            {
                wp1[n][i] *= det1;
            }
        }
        //итог в wm----------------------------
        else
        {
            //вычисляем  - w * A_1
            for (int i = 0; i <= n_1; ++i)
            {
                tEx = 0;
                for (int j = 0; j <= n_1; ++j)
                {
                    tEx -= wp[n][j] * wp1[j][i];
                }
                wm[n][i] = tEx;
            }

            //beta
            det = wp[n][n];
            for (int j = 0; j <= n_1; ++j)
            {
                det += wm[n][j] * wp[j][n];
            }
            auto det1 = 1.0 / det;
            wm[n][n] = det1;

            // - beta * A_1 * v
            for (int i = 0; i <= n_1; ++i)
            {
                tEx = 0;
                for (int j = 0; j <= n_1; ++j)
                {
                    tEx -= wp1[i][j] * wp[j][n];
                }
                tEx *= det1;
                wm[i][n] = tEx;

                // A_1 + beta * A_1 * v * w * A_1

                for (int j = 0; j <= n_1; ++j)
                {
                    wm[i][j] = wp1[i][j] + wm[n][j] * tEx;
                }
            }

            //вычисляем  - beta * w * A_1
            for (int i = 0; i <= n_1; ++i)
            {
                wm[n][i] *= det1;
            }
        }
    }
    //Log('Вынырнули с ' + IntToStr(n) + 'уровня');
}

//--------- обратная матрица - оптимизированный расчет -------------------------
void Expand::negativeMatrixOp(size_t n)
{
    dive(n, true);
}
