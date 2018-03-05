#pragma once
#include <vector>
struct Vertex;
struct Face;
struct Params;
Result buildGraph(const Params& params, std::vector<Vertex>& graph, std::vector<Face>& faces);
