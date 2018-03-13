#include <algorithm>
#include <vector>
#include <iostream>
#include <math.h>
#include "fullereneTypes.h" 
#include "..\external\simple_svg_1.0.0.hpp"

Result paint(const wchar_t* outfileName, std::vector<Vertex>& graph)
{
    int minX = graph[0].x;
    int maxX = minX;
    int minY = graph[0].y;
    int maxY = minY;
    for (auto v : graph)
    {
        if (v.x < minX) minX = v.x;
        if (v.x > maxX) maxX = v.x;
        if (v.y < minY) minY = v.y;
        if (v.y > maxY) maxY = v.y;
    }

    svg::Dimensions dimensions(maxX - minX, maxY - minY);
    svg::Document doc(outfileName, svg::Layout(dimensions, svg::Layout::BottomLeft));
    for (size_t i = 0; i < graph.size(); ++i)
    {
        Vertex &v = graph[i];
        for (size_t edge = 0; edge < 3; ++edge)
        {
            if (static_cast<size_t>(v.e[edge]) < i) continue;

            svg::Line line(svg::Point(v.x - minX, v.y - minY),
                svg::Point(graph[v.e[edge]].x - minX, graph[v.e[edge]].y - minY),
                svg::Stroke(1, svg::Color::Red));
            doc << line;
        }
    }
    doc.save();
    return Result::OK;
}