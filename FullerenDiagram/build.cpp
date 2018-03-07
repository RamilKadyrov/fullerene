#include <algorithm>
#include <vector>
#include <iostream>

#include "fullereneTypes.h" 
#include "buildFullerene.h"

Result buildGraph(const Params& params, std::vector<Vertex>& graph)
{
    std::wcout << L"Building graph..." << std::endl;

    std::vector<Face> faces;
    std::vector<Edge> edges;
    std::vector<Vertex> vertexes;
    
    faces.resize(params.faceNum);
    vertexes.reserve(params.faceNum * 2);
    edges.reserve(params.faceNum * 3);
    
    try
    {
        //initializing number of edges for pentagons
        for (auto p : params.pentagons)
        {
            faces[p].size = 5;
        }
        //building first face
        auto pSd = &faces[0];
        for (int edgeIndex = 0; edgeIndex < pSd->size; ++edgeIndex)
        {
            int v2 = 0;
            if (edgeIndex + 1 != pSd->size) v2 = edgeIndex + 1;
            edges.push_back(Edge(0,-1,edgeIndex, v2));
                        
            pSd->edge[edgeIndex] = edgeIndex;
            int e0 = pSd->size - 1;
            if (0 != edgeIndex) e0 = edgeIndex - 1;
            vertexes.push_back(Vertex(e0, edgeIndex, -1, 0, static_cast<int>(vertexes.size()), 0));
        }
        //loop on the remainig faces
        size_t freeFace = 0;
        size_t targetFace = 1;
        int curentVertex = 0;
        bool cycle = false;
        unsigned int curentCycle = 1;
        for (int faceIndex = 1; faceIndex < params.faceNum; ++faceIndex)
        {
            if (cycle)
            {
                cycle = false;
                targetFace = faceIndex + 1;
                ++curentCycle;
                curentVertex = 0;
            }
            pSd = &faces[faceIndex];
            auto fSd = &faces[freeFace];
            std::wcout << L"Face " << faceIndex << L", edges number" << pSd->size << std::endl;
             //search for an unoccupied free face's edge 
            int edgeIndex = 0;
            while (-1 != edges[fSd->edge[edgeIndex]].s2)
            {
                ++edgeIndex;
                if (edgeIndex == fSd->size)
                {
                    ++freeFace;
                    if (freeFace == params.faceNum )
                    {
                        std::wcerr << L"Can't find a free face" << std::endl;
                        throw std::exception();
                    }
                    std::wcout << L"Free face " << freeFace << std::endl;
                    fSd = &faces[freeFace];
                    edgeIndex = 0;
                }
            } 
            //Founded it. Use it.
            std::wcout << L"Unoccupied edge " << edgeIndex << L", free face " << faceIndex << std::endl;
            pSd->edge[0] = fSd->edge[edgeIndex];

            auto pEd = &(edges[pSd->edge[0]]);
            pEd->s2 = faceIndex;
            //fill edges and vertexes of the current face
            for (edgeIndex = 1; edgeIndex < pSd->size; ++edgeIndex)
            {
                //if the vertex have 3 connections
                int vertexIndex = 3;
                do
                {
                    --vertexIndex;
                } while ((-1 != vertexes[pEd->v1].e[vertexIndex]) && (0 != vertexIndex));
                //then search a free edge of the vertex 
                if (vertexes[pEd->v1].e[vertexIndex] != -1)
                {
                    vertexIndex = 3;
                    do
                    {
                        --vertexIndex;
                    } while ((-1 != edges[vertexes[pEd->v1].e[vertexIndex]].s2) && (0 != vertexIndex));
                    if (-1 != edges[vertexes[pEd->v1].e[vertexIndex]].s2)
                    {
                        std::wcerr << L"No free edge found." << std::endl;
                        throw std::exception(); 
                    }
                    //occuping it
                    pSd->edge[edgeIndex] = vertexes[pEd->v1].e[vertexIndex];
                    pEd = &edges[vertexes[pEd->v1].e[vertexIndex]];
                    pEd->s2 = faceIndex;
                    std::wcout << L"Edge occupied by the face " << pEd->s1 << L", edge" 
                        << pSd->edge[edgeIndex] << L", edge index on face " << edgeIndex << std::endl;
                }
                else
                {
                    break;
                }
            }
            //If all edges occupied, then continue
            if (edgeIndex == pSd->size)
            {
                //проверим, а туда ли мы пришли
                if (edges[pSd->edge[0]].v2 != edges[pSd->edge[pSd->size - 1]].v1)
                {
                    std::wcerr << L"Face " << faceIndex << L" did not close." << std::endl;
                    throw std::exception(); 
                }
                else std::wcout << L"Ok! The graph is closed." << std::endl;
                continue;
            }
            int edgeBegin = edgeIndex;
            //revers order
            pEd = &edges[pSd->edge[0]];
            for (edgeIndex = pSd->size - 1; edgeIndex >= 2; --edgeIndex)
            {
                //if vertex have three connection
                int vertexIndex = 3;
                do
                {
                    --vertexIndex;
                } while ((-1 != vertexes[pEd->v2].e[vertexIndex]) && (0 != vertexIndex));

                //then search a free edge
                if (-1 != vertexes[pEd->v2].e[vertexIndex])
                {
                    vertexIndex = 3;
                    do
                    {
                        --vertexIndex;
                    } while ((-1 != edges[vertexes[pEd->v2].e[vertexIndex]].s2) && (0 != vertexIndex));
                    if (edges[vertexes[pEd->v2].e[vertexIndex]].s2 != -1)
                    {
                        std::wcerr << L"No free edge found." << std::endl;
                        throw std::exception();
                    }
                    //occuping it
                    pSd->edge[edgeIndex] = vertexes[pEd->v2].e[vertexIndex];
                    pEd = &edges[vertexes[pEd->v2].e[vertexIndex]];
                    pEd->s2 = faceIndex;
                    std::wcout << L"The edge occupied by the face " << pEd->s1 << L", edge" << pSd->edge[edgeIndex] 
                        << L", edge index on the face " << edgeIndex << std::endl;
                    if (pEd->s1 == targetFace)
                    {
                        std::wcout << L"Cycle " << targetFace << std::endl;
                        cycle = true;
                    }
                }
                else
                {
                    break;
                }
            }
            //creating the face's new edges and vertexes
            int edgeEnd = edgeIndex;
            vertexes[edges[pSd->edge[edgeBegin - 1]].v1].e[2] = static_cast<int>(edges.size());
            for (edgeIndex = edgeBegin; edgeIndex < edgeEnd; ++edgeIndex)
            {
                pSd->edge[edgeIndex] = static_cast<int>(edges.size());

                int v1 = edges[pSd->edge[edgeIndex - 1]].v1;
                if (edgeIndex != edgeBegin) v1 = edges[pSd->edge[edgeIndex - 1]].v2;
                edges.push_back(Edge(faceIndex, -1, v1, static_cast<int>(vertexes.size())));
                
                vertexes.push_back(Vertex(static_cast<int>(edges.size()) - 1, 
                    static_cast<int>(edges.size()), -1, curentCycle, curentVertex, 0));
                                
                ++curentVertex;
            }
            //creating the face's last edge 
            pSd->edge[edgeIndex] = static_cast<int>(edges.size());
            int v2 = edges[pSd->edge[0]].v2;
            if (edgeIndex != pSd->size - 1) v2 = edges[pSd->edge[edgeIndex + 1]].v2;
            vertexes[v2].e[2] = static_cast<int>(edges.size());
            int v1 = edges[pSd->edge[edgeIndex - 1]].v2;
            if (edgeBegin == edgeEnd)
            {
                v1 = edges[pSd->edge[edgeIndex - 1]].v1;
                vertexes[v1].e[2] = static_cast<int>(edges.size());
            }
            edges.push_back(Edge(faceIndex, -1, v1, v2));
        }// main loop of creating faces
        //marking last face
        pSd = &faces[params.faceToExpand];
        ++curentCycle;
        if ((edges[pSd->edge[0]].v1 == edges[pSd->edge[1]].v1)
            || (edges[pSd->edge[0]].v1 == edges[pSd->edge[1]].v2))
        {
            auto v2 = edges[pSd->edge[0]].v2;
            vertexes[v2].e[5] = 1000;
            vertexes[v2].e[4] = 0;
            vertexes[v2].e[3] = curentCycle;
        }
        else
        {
            auto v1 = edges[pSd->edge[0]].v1;
            vertexes[v1].e[5] = 1000;
            vertexes[v1].e[4] = 0;
            vertexes[v1].e[3] = curentCycle;
        }
        for (int i = 1; i < pSd->size; ++i)
        {
            int j = pSd->edge[i];
            if ((edges[j].v1 == edges[pSd->edge[i - 1]].v1)
                || (edges[j].v1 == edges[pSd->edge[i - 1]].v2))
            {
                auto v1 = edges[j].v1;
                vertexes[v1].e[5] = 1000;
                vertexes[v1].e[4] = i;
                vertexes[v1].e[3] = curentCycle;
            }
            else
            {
                auto v2 = edges[j].v2;
                vertexes[v2].e[5] = 1000;
                vertexes[v2].e[4] = i;
                vertexes[v2].e[3] = curentCycle;
            }
        }
        //creating output graph
        graph.resize(vertexes.size());
        for (size_t i = 0; i < vertexes.size(); ++i)
        {
            graph[i].e[4] = static_cast<int>(i);
            graph[i].e[2] = vertexes[i].e[3];
            graph[i].e[3] = vertexes[i].e[4];
            graph[i].e[5] = vertexes[i].e[5];
        }
        //sorting, last face vertexes will be on the end
        std::sort(graph.begin(), graph.end(), [](Vertex& a, Vertex& b) {
            if ( a.e[5] == b.e[5])
            {
                if (a.e[2] > b.e[2]) return false;
                else
                {
                    if (a.e[2] < b.e[2]) return true;
                    else
                    {
                        if (a.e[3] > b.e[3]) return false;
                        else
                        {
                            if (a.e[3] < b.e[3]) return true;
                            else return true;
                        }
                    }
                }
            }
            else
            {
                if( a.e[5] > b.e[5]) return false;
                else return true;
            }
            return true;
        });
        //reindexing vertexes
        for (size_t vertexIndex = 0; vertexIndex < vertexes.size(); ++vertexIndex)
        {
            curentVertex = graph[vertexIndex].e[4];
            for (size_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
            {
                if (edges[vertexes[curentVertex].e[edgeIndex]].v1 == curentVertex)
                    edges[vertexes[curentVertex].e[edgeIndex]].v1 = static_cast<int>(vertexIndex);
                else edges[vertexes[curentVertex].e[edgeIndex]].v2 = static_cast<int>(vertexIndex);
            }
        }
        //creating vertexes output array
        for (size_t vertexIndex = 0; vertexIndex < vertexes.size(); ++vertexIndex)
        {
            auto pVr = &(graph[vertexIndex]);
            curentVertex = pVr->e[4];
            for (int edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
            {
                if (edges[vertexes[curentVertex].e[edgeIndex]].v1 == vertexIndex)
                    pVr->e[edgeIndex] = edges[vertexes[curentVertex].e[edgeIndex]].v2;
                else pVr->e[edgeIndex] = edges[vertexes[curentVertex].e[edgeIndex]].v1;
            }
            pVr->e[3] = vertexes[curentVertex].e[3];
            pVr->e[4] = vertexes[curentVertex].e[4];
            pVr->e[5] = vertexes[curentVertex].e[5];
        }
       
        std::wcout << L"The graph is complete." << std::endl;
     
     }
    catch (...)
    {
        std::wcerr << L"Error! The graph is not built. " << std::endl;
        return Result::FAIL;
    }
    return Result::OK;
}