#pragma once
#include <vector>

#include "fullereneTypes.h"
class Expand
{
public:
    const double L0 = 10.0;
    const size_t iter = 10;
    const double epsilon = 0.01;

    Result expand(std::vector<Vertex>& graph, const double gRm, const size_t nSide);
private:
     //: array[0..maxVertex, 0..maxVertex] of Extended;
    size_t sizeXY;
    std::vector<double> wm;
    std::vector<double> wp;
    std::vector<double> wp1;
    void dive(size_t n, bool p);
    void inverseMatrixOp(size_t n);
};