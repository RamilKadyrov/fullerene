#include <algorithm>
#include <vector>
#include <iostream>
#include <math.h>
#include "fullereneTypes.h" 
#include "..\external\simple_svg_1.0.0.hpp"

Result paint(const wchar_t* outfileName, std::vector<Vertex>& graph)
{
    int minX = graph[0].e[3];
    int maxX = minX;
    int minY = graph[0].e[4];
    int maxY = minY;
    for (size_t vertexIndex = 1; vertexIndex < graph.size(); ++vertexIndex)
    {
        Vertex &vertex = graph[vertexIndex];
        if (vertex.e[3] < minX) minX = vertex.e[3];
        if (vertex.e[3] > maxX) maxX = vertex.e[3];
        if (vertex.e[4] < minY) minY = vertex.e[4];
        if (vertex.e[4] > maxY) maxY = vertex.e[4];
    }

    svg::Dimensions dimensions(maxX - minX, maxY - minY);
    svg::Document doc(outfileName, svg::Layout(dimensions, svg::Layout::BottomLeft));
        for (size_t vertexIndex = 0; vertexIndex < graph.size(); ++vertexIndex)
    {
        Vertex &vertex = graph[vertexIndex];
        for (size_t edge = 0;  edge < 3; ++edge)
        {
            if( static_cast<size_t>(vertex.e[edge]) < vertexIndex) continue;
                        
            svg::Line line(svg::Point(vertex.e[3] - minX, vertex.e[4] - minY),
                           svg::Point(graph[vertex.e[edge]].e[3] - minX, graph[vertex.e[edge]].e[4] - minY),
                           svg::Stroke(1, svg::Color::Red));
            doc << line;

        }
    }
    doc.save();
    return Result::OK;
}