#include <algorithm>
#include <vector>
#include <iostream>
#include <math.h>
#include "fullereneTypes.h" 
#include "..\external\simple_svg_1.0.0.hpp"

Result paint(const wchar_t* outfileName, std::vector<Vertex>& graph)
{
    svg::Dimensions dimensions(2000, 2000);
    svg::Document doc(outfileName, svg::Layout(dimensions, svg::Layout::BottomLeft));
        for (size_t vertexIndex = 0; vertexIndex < graph.size(); ++vertexIndex)
    {
        Vertex &vertex = graph[vertexIndex];
        for (size_t edge = 0;  edge < 3; ++edge)
        {
            if( vertex.e[edge] < vertexIndex) continue;
            /*if( (vertex.e[3] > 1000) || (vertex.e[4] > 1000)
                || (vertex.e[3] < -1000) || (vertex.e[4] < -1000)
                || (graph[vertex.e[edge]].e[3] > 1000)
                || (graph[vertex.e[edge]].e[4] > 1000)
                || (graph[vertex.e[edge]].e[3] < -1000)
                || (graph[vertex.e[edge]].e[4] < -1000)) continue;*/
            //Pen.Color = clWhite;
            //MoveTo(vertex.e[3], vertex.e[4]);
            //LineTo(graph[vertex.e[edge]].e[3], graph[vertex.e[edge]].e[4]);
            svg::Line line(svg::Point(vertex.e[3]+1000, vertex.e[4]+1000),
                           svg::Point(graph[vertex.e[edge]].e[3]+1000, graph[vertex.e[edge]].e[4]+1000),
                           svg::Stroke(1, svg::Color::Red));
            doc << line;

        }
    }
    doc.save();
    return Result::OK;
}