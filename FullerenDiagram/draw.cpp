#include <algorithm>
#include <vector>
#include <iostream>
#include <math.h>
#include "fullereneTypes.h" 

Result draw(std::vector<Vertex>& graph)
{
    std::vector<int> sp;
    
    const double R = 20.0;
    
    sp.reserve( 10);
    for (size_t VertexIndex = 0; VertexIndex < graph.size(); ++VertexIndex)
    {
        Vertex& vertex = graph[VertexIndex];
        while (vertex.e[3] >= sp.size())
        {
            sp.push_back(0);
        }
        if (vertex.e[5] != 1000) vertex.e[5] = vertex.e[3];
        ++sp[vertex.e[3]];
        vertex.e[3] = static_cast<int>(R * pow(1.5, vertex.e[3]));
    }
    int edge = 0;
    const double gRm = (R * pow(1.5,  sp.size() - 2.0) + 5.0);

    const double re = gRm * enlarge;
    for (size_t VertexIndex = 0; VertexIndex < graph.size(); ++VertexIndex)
    {
        Vertex& vertex = graph[VertexIndex];
        if (vertex.e[5] = 1000)
        {
            vertex.e[5] = static_cast<int>(sp.size());
            vertex.e[3] = static_cast<int>(re);
            vertex.e[4] = edge;
            ++edge;
        }
    }
    if (edge > 0)
    {
        sp.push_back(edge);
    }
    const double Cx = gRm;
    const double Cy = gRm;
    for (size_t VertexIndex = 0; VertexIndex < graph.size() ; ++VertexIndex)
    {
        Vertex& vertex = graph[VertexIndex];
        double r = vertex.e[3]; // cycle index
        double nq = vertex.e[4]; // vertex index in cycle
        double nn = sp[vertex.e[5]];//points on cycle
        double angle = nq * 2 * 3.14 / nn;
        
        vertex.e[3] = static_cast<int>((r * cos(angle) + Cx));
        vertex.e[4] = static_cast<int>((r * sin(angle) + Cy));
    }
    return Result::OK;
}