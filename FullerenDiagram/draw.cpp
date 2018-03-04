#include <algorithm>
#include <vector>
#include <iostream>
#include <math.h>
#include "fullerenTypes.h" 

bool draw(std::vector<Vertex>& graph)
{
    std::vector<int> sp;
    
    const double R = 20.0;
    
    sp.reserve( 10);
    for (int VertexIndex = 0; VertexIndex <= graph.size() - 1; ++VertexIndex)
    {
        Vertex& pVer = graph[VertexIndex];
        //if( pVer.e[3] = 1000 ) pVer.e[3] := 0;
        while (pVer.e[3] > (sp.size() - 1))
        {
            sp.push_back(0);
        }
        if (pVer.e[5] != 1000) pVer.e[5] = pVer.e[3];
        ++sp[pVer.e[3]];
        pVer.e[3] = round(R * exp(pVer.e[3]));
    }
    int edge = 0;
    double gRm = (R * exp( sp.size() - 2) + 5);

    const double re = gRm * enlarge;
    for (int VertexIndex = 0; VertexIndex <= (graph.size() - 1); ++VertexIndex)
    {
        Vertex& pVer = graph[VertexIndex];
        if (pVer.e[5] = 1000)
        {
            pVer.e[5] = sp.size();
            pVer.e[3] = round(re);
            pVer.e[4] = edge;
            ++edge;
        }
    }
    if (edge > 0)
    {
        sp.push_back(edge);
    }
    double Cx = gRm;
    double Cy = gRm;
    for (int VertexIndex = 0; VertexIndex <= (graph.size() - 1); ++VertexIndex)
    {
        Vertex& pVer = graph[VertexIndex];
        double r = pVer.e[3];
        double nq = pVer.e[4];
        double nn = sp[pVer.e[5]];
        double angle = nq * 2 * 3.14 / nn;
        
        pVer.e[3] = round(R * cos(angle) + Cx);
        pVer.e[4] = round(R * sin(angle) + Cy);
    }
 
}