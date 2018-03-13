#pragma once
#include <vector>

struct Vertex;
struct Params;

class Build
{
public:
    Result graph(const Params& params, std::vector<Vertex>& graph);
private:
    std::vector<Face> faces;
    std::vector<Edge> edges;
    std::vector<Vertex> vertexes;
    bool fillEV(int faceIndex, int edgeIndex, const Vertex& vertex, Edge **edge);
};