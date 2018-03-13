#include <algorithm>
#include <vector>
#include <iostream>
#include <math.h>
#include "fullereneTypes.h" 

Result draw(std::vector<Vertex>& graph, double& gRadius)
{
    std::vector<int> sp;

    const double R = 20.0;

    sp.reserve(10);
    for (size_t vertexIndex = 0; vertexIndex < graph.size(); ++vertexIndex)
    {
        Vertex& vertex = graph[vertexIndex];
        while (vertex.cycle >= sp.size())
        {
            sp.push_back(0);
        }
        ++sp[vertex.cycle];
        vertex.r = static_cast<int>(R * pow(1.5, vertex.e[3]));
    }

    gRadius = (R * pow(1.5, sp.size() - 2.0) + 5.0);
    const double radius = gRadius * enlarge;
    //set last face vertexes coordinates 
    int edge = 0;
    for (size_t vertexIndex = graph.size() - 1; vertexIndex >= 0; --vertexIndex)
    {
        Vertex& vertex = graph[vertexIndex];
        if (vertex.lastFace)
        {
            vertex.cycle = static_cast<int>(sp.size());
            vertex.r = static_cast<int>(radius);
            vertex.cycleVertexIndex = edge;
            ++edge;
        }
        else
        {
            break;
        }
    }
    sp.push_back(edge);
    const double Cx = gRadius;
    const double Cy = gRadius;
    for (size_t vertexIndex = 0; vertexIndex < graph.size(); ++vertexIndex)
    {
        Vertex& vertex = graph[vertexIndex];
        double r = vertex.r; // radius
        double nq = static_cast<double>(vertex.cycleVertexIndex); // vertex index in cycle
        double nn = sp[vertex.cycle];//points on cycle
        double angle = nq * 2.0 * 3.14 / nn;

        vertex.x = static_cast<int>((r * cos(angle) + Cx));
        vertex.y = static_cast<int>((r * sin(angle) + Cy));
    }
    return Result::OK;
}