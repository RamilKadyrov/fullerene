#pragma once
#include <vector>

struct Vertex;

const double L0 = 10.0;
const size_t iter = 10;
const double epsilon = 0.01;

Result expand(std::vector<Vertex>& graph, const double gRadius, const size_t nSide);
