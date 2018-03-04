#include <algorithm>
#include <vector>
#include <iostream>

#include "fullerenTypes.h" 
#include "buildGraph.h"




class Expand
{
public:
    Result expand(std::vector<Vertex>& graph)
    {
        const double L0 = 10.0;
        const int iter = 10;

        int all, count, countN;

        double *f, *v; // : array[0..maxVertex];
        double *pExt;
        double x, y, residue, residueOld, fL, fX, fY, fXY;
        
        bool quit;
        
        double rm = 1.0;//gRm;
                 //graph = TMyParam(Param^).graph;
        int nSide = 5;//TMyParam(Param^).nSide;
                  //LeaveCriticalSection(cs);

        if ((nSide != 5) && (nSide != 6))
        {
            std::wcerr << L"Last face contain incorrect number of edges:" << nSide << std::endl;
            return Result::FAIL;
        }
        count = graph.size() - 1;// graph.count - 1;
        for (int i = 0; i <= count; ++i)
        {
            v[i * 2] = graph[i].e[3];
            v[i * 2 + 1] = graph[i].e[4];
        }
        all = count * 2 + 1 - nSide * 2;
        /*if (all > maxVertex)
        {
            std::wcerr << L"Число вершин превысило maxVertex" << std::endl;
            return Result::FAIL;
        }*/
        countN = count - nSide;
        //Randomize;
        do {
            quit = true;
            for (int i = 0; i <= count; ++i)
            {
                for (int j = 0; j <= count; ++j)
                {
                    if ((i != j) && (sqrt(sqr(v[i * 2] - v[j * 2]) + sqr(v[i * 2 + 1] - v[j * 2 + 1])) < L0))
                    {
                        for (int i1 = 0; i1 <= countN; ++i1)
                        {
                            v[i1 * 2] += (1.0 + L0 * 3.0) * (std::rand() / RAND_MAX);//Random + Random(Round(L0) * 3);
                            v[i1 * 2 + 1] += (1.0 - L0 * 3.0) * (std::rand() / RAND_MAX);// Random - Random(Round(L0) * 3);
                        }
                        quit = false;
                        break;
                    }
                }
                if (!quit) break;
            }
            //Log('Взбивание.');
        } while (!quit);
        //основной счет
        for (int l = 0; l <= (enlarge - 1) * iter; ++l)
        {
            for (int i = 0; i <= nSide - 1; ++i)
            {
                double dv = double(enlarge * iter - l) / double(enlarge * iter + 1 - l);
                v[all + i * 2 + 1] = (v[all + i * 2 + 1] - rm) * dv + rm;
                v[all + i * 2 + 2] = (v[all + i * 2 + 2] - rm) * dv + rm;
            }
            int k = 25;
            residueOld = 1E10;
            //Log('Растягивание сети...');
            do {
                for (int i = 0; i <= countN; ++i)
                {
                    //подсчет функции
                    f[i * 2] = 0;
                    f[i * 2 + 1] = 0;
                    for (int Edge = 0; Edge <= 2; ++Edge)
                    {
                        int j = graph[i].e[Edge] * 2;
                        residue = sqrt(sqr(v[j] - v[i * 2]) + sqr(v[j + 1] - v[i * 2 + 1]));
                        fL = residue - L0;
                        if (residue != 0)
                        {
                            f[i * 2] = f[i * 2] + fL * (v[j] - v[i * 2]) / residue;
                            f[i * 2 + 1] = f[i * 2 + 1] + fL * (v[j + 1] - v[i * 2 + 1])
                                / residue;
                        }
                        else
                        {
                            //raise ERangeError.Create('Положение вершин совпало. Можно запустить снова, возможно, повезет.');
                        }
                    }
                    //обнуление матрицы производных
                    for (int j = 0; j <= countN; ++j)
                    {
                        wp[j * 2][i * 2] = 0;
                        wp[j * 2][i * 2 + 1] = 0;
                        wp[j * 2 + 1][i * 2] = 0;
                        wp[j * 2 + 1][i * 2 + 1] = 0;
                    }
                }
                //подсчет матрицы производных
                for (int i = 0; i <= countN; ++i)
                {
                    for (int Edge = 0; Edge <= 2; ++Edge)
                    {
                        int j = graph[i].e[Edge] * 2;
                        fL = sqr(v[j] - v[i * 2]) + sqr(v[j + 1] - v[i * 2 + 1]);
                        residue = 1 / (sqrt(fL) * fL);
                        fX = L0 * sqr(v[j + 1] - v[i * 2 + 1]) * residue - 1;
                        fY = L0 * sqr(v[j] - v[i * 2]) * residue - 1;
                        fXY = L0 * (v[j + 1] - v[i * 2 + 1]) * (v[i * 2] - v[j]) * residue;
                        wp[i * 2][i * 2] = wp[i * 2][i * 2] + fX;
                        wp[i * 2 + 1][i * 2 + 1] = wp[i * 2 + 1][i * 2 + 1] + fY;
                        wp[i * 2][i * 2 + 1] = wp[i * 2][i * 2 + 1] + fXY;
                        wp[i * 2 + 1][i * 2] = wp[i * 2 + 1][i * 2] + fXY;
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
                    //Log('Запушен расчет обратной матрицы из ' + IntToStr(all) + ' строк');
                    negativeMatrixOp(all);
                    k = 0;
                }
                ++k;
                for (int i = 0; i <= all; ++i)
                {
                    for (int j = 0; j <= all; ++j)
                    {
                        v[i] = v[i] - wp1[i][j] * f[j];
                    }
                }
                residue = 0;
                for (int i = 0; i <= all; ++i) residue = residue + fabs(f[i]);
                if (residue > residueOld) k = 25;
                residueOld = residue;

                for (int i = 0; i <= count; ++i)
                {
                    //TMyParam(Param^).Vx[i] = v[i * 2];
                    //TMyParam(Param^).Vy[i] = v[i * 2 + 1];
                    graph[i].e[3] = round(v[i * 2]);
                    graph[i].e[4] = round(v[i * 2 + 1]);
                }
                //Log('Результат = ' + FloatToStr(residue));
            } while (residue > 0.001);//until residue < 0.001;

            quit = true;
            for (int i = 0; i <= countN; ++i)
            {
                if (sqr(v[i * 2] - rm) + sqr(v[i * 2 + 1] - rm) >= sqr(rm * 0.9))
                {
                    quit = false;
                    break;
                }
            }
            if (quit)
            {
                int j = l;
                for (int i = 0; i <= nSide - 1; ++i)
                {
                    v[all + i * 2 + 1] = rm + rm * (v[all + i * 2 + 1] - rm)
                        / sqrt(sqr(v[all + i * 2 + 1] - rm) + sqr(v[all + i * 2 + 2] - rm));
                    v[all + i * 2 + 2] = rm + rm * (v[all + i * 2 + 2] - rm)
                        / sqrt(sqr(v[all + i * 2 + 1] - rm) + sqr(v[all + i * 2 + 2] - rm));
                }
                for (int i = 0; i <= count; ++i)
                {
                    //TMyParam(Param^).Vx[i] = v[i * 2];
                    //TMyParam(Param^).Vy[i] = v[i * 2 + 1];
                    graph[i].e[3] = round(v[i * 2]);
                    graph[i].e[4] = round(v[i * 2 + 1]);
                }
                break;
            }
        }
        //Log('Граф растянут.');
        return Result::OK;
    }
    
private:
    //: array[0..maxVertex, 0..maxVertex] of Extended;
    double **wm;
    double **wp;
    double **wp1;
    void dive(int n, bool p)
    {
        double det, tEx;
        int n_1;
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
            dive(n_1, ~p);
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
    void negativeMatrixOp(int n)
    {
        dive(n, true);
    }

    inline double sqr(double x)
    {
        return x * x;
    }

    
};