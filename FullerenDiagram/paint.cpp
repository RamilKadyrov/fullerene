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
    for (auto v : graph)
    {
        if (v.e[3] < minX) minX = v.e[3];
        if (v.e[3] > maxX) maxX = v.e[3];
        if (v.e[4] < minY) minY = v.e[4];
        if (v.e[4] > maxY) maxY = v.e[4];
    }

    svg::Dimensions dimensions(maxX - minX, maxY - minY);
    svg::Document doc(outfileName, svg::Layout(dimensions, svg::Layout::BottomLeft));
        for (size_t i = 0; i < graph.size(); ++i)
    {
        Vertex &v = graph[i];
        for (size_t edge = 0;  edge < 3; ++edge)
        {
            if( static_cast<size_t>(v.e[edge]) < i) continue;
                        
            svg::Line line(svg::Point(v.e[3] - minX, v.e[4] - minY),
                           svg::Point(graph[v.e[edge]].e[3] - minX, graph[v.e[edge]].e[4] - minY),
                           svg::Stroke(1, svg::Color::Red));
            doc << line;

        }
    }
    doc.save();
    return Result::OK;
}