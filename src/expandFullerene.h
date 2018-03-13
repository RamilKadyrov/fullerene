#pragma once
#include <vector>

struct Vertex;

const double L0 = 10.0;
const std::size_t iter = 10;
const double epsilon = 0.01;

Result expand(std::vector<Vertex>& graph, const double gRadius, const std::size_t nSide);
